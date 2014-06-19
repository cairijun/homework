#include <stdint.h>
#include <stdbool.h>
#include "fs.h"
#include "floppy.h"
#include "scheduler.h"
#include "utils.h"
#include "mm.h"

struct BPBHeader_t BPBHEADER;
static int major_fat_cn;
static int root_dir_cn;
static int rel_cn_offset;
static int n_files_per_sector;
static uint8_t major_fat[8192];

static inline int LBA_C(int lba)
{
    return lba / (BPBHEADER.n_sectors_per_cyliner * BPBHEADER.n_heads);
}

static inline int LBA_H(int lba)
{
    return (lba / BPBHEADER.n_sectors_per_cyliner) % BPBHEADER.n_heads;
}

static inline int LBA_S(int lba)
{
    return lba % BPBHEADER.n_sectors_per_cyliner + 1;
}

static inline uint16_t fat12_next_cn(int cn)
{
    int n = cn + (cn >> 1);
    if(cn & 1)
        return *(uint16_t *)&major_fat[n] >> 4;
    else
        return *(uint16_t *)&major_fat[n] & 0x0FFF;
}

static bool floppy_buf_read(int lba)
{
    if(CURRENT_TASK->io_buf_valid && CURRENT_TASK->io_buf_idx == lba)
        return true;
    if(fdc_seek(LBA_C(lba)) &&
            fdc_read(LBA_C(lba), LBA_H(lba), LBA_S(lba), 1, CURRENT_TASK->io_buf)) {
        CURRENT_TASK->io_buf_valid = true;
        CURRENT_TASK->io_buf_idx = lba;
        return true;
    }
    return false;
}

bool init_fat()
{
    if(fdc_recalibrate() && fdc_seek(0) && fdc_read(0, 0, 1, 1, CURRENT_TASK->io_buf))
        memcpy(&BPBHEADER, CURRENT_TASK->io_buf, sizeof(BPBHEADER));
    else
        return false;
    CURRENT_TASK->io_buf_valid = true;
    CURRENT_TASK->io_buf_idx = 0;
    major_fat_cn = BPBHEADER.n_reserved_clusters;
    root_dir_cn = BPBHEADER.n_reserved_clusters +
        BPBHEADER.n_fats * BPBHEADER.n_sectors_per_fat;
    rel_cn_offset = ((BPBHEADER.n_dir_entities * 32) + (BPBHEADER.sector_size - 1))
        / BPBHEADER.sector_size
        + root_dir_cn - 2;
    n_files_per_sector = BPBHEADER.sector_size / sizeof(struct FAT_File_t);

    fdc_read(LBA_C(major_fat_cn), LBA_H(major_fat_cn), LBA_S(major_fat_cn),
        BPBHEADER.n_sectors_per_fat, major_fat);

    return true;
}

int fat12_ls(int rel_cn, int offset, struct FAT_File_t *file)
{
    int sector, offset_for_offset;
    if(rel_cn == 0) {   /* root dir */
        sector = root_dir_cn;
        if(offset)
            --offset;
        else {          /* manual add a '.' for the FAT root dir */
            file->cn = 0;
            file->filename[0] = '.';
            file->filename[1] = ' ';
            file->ext[0] = ' ';
            return 1;
        }
        offset_for_offset = (offset / n_files_per_sector) * n_files_per_sector + 2;
    }
    else {
        sector = rel_cn + rel_cn_offset;
        offset_for_offset = (offset / n_files_per_sector) * n_files_per_sector + 1;
    }
    sector += offset / n_files_per_sector;
    offset %= n_files_per_sector;

    floppy_buf_read(sector);

    struct FAT_File_t *p = (struct FAT_File_t *)CURRENT_TASK->io_buf;
    while(offset < n_files_per_sector) {
        if(p[offset].filename[0] == 0)
            return 0;
        if(p[offset].filename[0] == (char)0xE5 || p[offset].attr == 0x0F) {
            ++offset;
        } else {
            *file = p[offset];
            return offset + offset_for_offset;
        }
    }
    return 0;
}

bool fat12_read(const struct FAT_File_t *file, void *dest, size_t size)
{
    int i = file->cn, tmp;
    size = size ? size : file->size;
    while(size && i < 0xFF8) {
        tmp = size > 512 ? 512 : size;
        if(!floppy_buf_read(i + rel_cn_offset))
            return false;
        memcpy(dest, CURRENT_TASK->io_buf, tmp);
        dest += tmp;
        size -= tmp;
        i = fat12_next_cn(i);
    }
    return true;
}

