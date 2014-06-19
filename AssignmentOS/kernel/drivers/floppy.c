#include <stdint.h>
#include <stdbool.h>
#include "scheduler.h"
#include "constants.h"
#include "interrupts.h"
#include "floppy.h"
#include "utils.h"

void inthandler26();
volatile bool fdc_reset = false;

bool fdc_send_byte(uint8_t data)
{
    int counter = FDC_TIMEOUT;
    bool success = false;
    while(counter--) {
        if((inb(MAIN_STATUS_REGISTER) & 0xc0) == 0x80) {
            success = true;
            break;
        }
    }
    if(success)
        outb(DATA_FIFO, data);
    return success;
}

bool fdc_get_byte(uint8_t *data)
{
    int counter = FDC_TIMEOUT;
    bool success = false;
    while(counter--) {
        if((inb(MAIN_STATUS_REGISTER) & 0xc0) == 0xc0) {
            success = true;
            break;
        }
    }
    if(success)
        *data = inb(DATA_FIFO);
    return success;
}

bool fdc_recalibrate()
{
    bool result = true;
    uint8_t tmp;
    cli();
    fdc_send_byte(RECALIBRATE);
    fdc_send_byte(0);
    wait_event(WAIT_IRQ6);
    fdc_send_byte(SENSE_INTERRUPT);
    fdc_get_byte(&tmp);
    if(tmp & 0xC0)
        result = false;
    fdc_get_byte(&tmp);
    sti();
    return result;
}

bool fdc_seek(uint8_t cyliner)
{
    bool result = true;
    uint8_t tmp;
    cli();
    fdc_send_byte(SEEK);
    fdc_send_byte(0);
    fdc_send_byte(cyliner);
    wait_event(WAIT_IRQ6);
    fdc_send_byte(SENSE_INTERRUPT);
    fdc_get_byte(&tmp);
    if(tmp & 0xC0)
        result = false;
    fdc_get_byte(&tmp);
    sti();
    return result;
}

bool fdc_read(int cyliner, int head, int sector, int nsect, void *target)
{
    bool result = true;
    uint8_t tmp;
    if(!fdc_DMA_init(false, target, 512 * nsect))
        return false;
    cli();
    fdc_send_byte(READ_DATA | 0xC0);
    fdc_send_byte(head << 2);
    fdc_send_byte(cyliner);
    fdc_send_byte(head);
    fdc_send_byte(sector);
    fdc_send_byte(2);
    fdc_send_byte(18);
    fdc_send_byte(0x1B);
    fdc_send_byte(0xFF);
    wait_event(WAIT_IRQ6);
    fdc_get_byte(&tmp);
    if(tmp & 0xC0)
        result = false;
    fdc_get_byte(&tmp);
    fdc_get_byte(&tmp);
    fdc_get_byte(&tmp);
    fdc_get_byte(&tmp);
    fdc_get_byte(&tmp);
    fdc_get_byte(&tmp);
    sti();
    return result;
}

bool init_fdc()
{
    int i;
    uint8_t tmp;
    IDT[0x26] = make_int_desc(0x8, (uint32_t)inthandler26);
    enable_irq(6);
    fdc_reset = false;
    outb(DATARATE_SELECT_REGISTER, 0x80);       /* enter reset mode */
    while(!fdc_reset)                           /* wait for IRQ6 but scheduler has not yet started */
        hlt();
    for(i = 0; i < 4; ++i) {
        fdc_send_byte(SENSE_INTERRUPT);
        fdc_get_byte(&tmp);
        fdc_get_byte(&tmp);
    }
    outb(CONFIGURATION_CONTROL_REGISTER, 0);    /* set data rate */
    fdc_send_byte(SPECIFY);                     /* set fdc parameters */
    fdc_send_byte(0xCF);
    fdc_send_byte(0x6);
    outb(DIGITAL_OUTPUT_REGISTER, 0x1C);          /* select drive 0 and motor on */
    return true;
}

void _inthandler26()
{
    if(fdc_reset)
        wake_up_event(WAIT_IRQ6);
    else
        fdc_reset = true;
    pic_eoi(6);
}

bool fdc_DMA_init(bool write, void *addr, size_t len)
{
    if((uint32_t)addr > 0xFFFFFF)
        return false;
    if(len > (1 << 16))
        return false;
    --len;
    cli();
    outb(0x0A, 0x6);
    outb(0x0C, 0);
    outb(0x0B, write ? 0x4A : 0x46);
    outb(0x04, ((uint32_t)addr) & 0xFF);
    outb(0x04, (((uint32_t)addr) & 0xFF00) >> 8);
    outb(0x81, (((uint32_t)addr) & 0xFF0000) >> 16);
    outb(0x05, len & 0xFF);
    outb(0x05, len >> 8);
    outb(0x0A, 0x2);
    sti();
    return true;
}
