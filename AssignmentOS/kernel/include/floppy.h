#ifndef _FLOPPY_H
#define _FLOPPY_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

enum FDC_REG
{
    STATUS_REGISTER_A                = 0x3F0,
    STATUS_REGISTER_B                = 0x3F1,
    DIGITAL_OUTPUT_REGISTER          = 0x3F2,
    TAPE_DRIVE_REGISTER              = 0x3F3,
    MAIN_STATUS_REGISTER             = 0x3F4,
    DATARATE_SELECT_REGISTER         = 0x3F4,
    DATA_FIFO                        = 0x3F5,
    DIGITAL_INPUT_REGISTER           = 0x3F7,
    CONFIGURATION_CONTROL_REGISTER   = 0x3F7 
};

enum FDC_CMD
{
    READ_TRACK           = 2,
    SPECIFY              = 3,
    SENSE_DRIVE_STATUS   = 4,
    WRITE_DATA           = 5,
    READ_DATA            = 6,
    RECALIBRATE          = 7,
    SENSE_INTERRUPT      = 8,
    WRITE_DELETED_DATA   = 9,
    READ_ID              = 10,
    READ_DELETED_DATA    = 12,
    FORMAT_TRACK         = 13,
    SEEK                 = 15,
    VERSION              = 16,
    SCAN_EQUAL           = 17,
    PERPENDICULAR_MODE   = 18,
    CONFIGURE            = 19,
    LOCK                 = 20,
    VERIFY               = 22,
    SCAN_LOW_OR_EQUAL    = 25,
    SCAN_HIGH_OR_EQUAL   = 29
};

bool init_fdc();
bool fdc_DMA_init(bool write, void *addr, size_t len);
bool fdc_read(int cyliner, int head, int sector, int nsect, void *target);
bool fdc_recalibrate();
bool fdc_seek(uint8_t cyliner);

#endif
