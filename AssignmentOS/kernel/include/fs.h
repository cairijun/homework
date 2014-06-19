#ifndef _FS_H
#define _FS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

enum FileType{DIRECTORY, ARCHIVE};

struct File_t
{
    char filename[13];
    enum FileType type;
    uint32_t size;
    uint32_t location;      /* where is the file */
    uint32_t p_location;    /* where is the parent */
    uint32_t next_item;     /* how to find the next file */
};

bool open_dir(const struct File_t *dir, struct File_t *sub_item);
bool open_dir_path(const char *path, struct File_t *sub_item);
bool get_next_file(struct File_t *item);
size_t read_file(const struct File_t *file, void *dest);
size_t read_file_path(const char *path, void *dest);
bool open_file_path(const char *path, struct File_t *file);

enum FAT_FileAttr
{
    FAT_READ_ONLY=0x01,
    FAT_HIDDEN=0x02,
    FAT_SYSTEM=0x04,
    FAT_VOLUME_ID=0x08,
    FAT_DIRECTORY=0x10,
    FAT_ARCHIVE=0x20
};

struct FAT_File_t
{
    char filename[8];
    char ext[3];
    uint8_t attr;
    uint8_t _res;
    uint8_t creation_timestamp;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_time;
    uint16_t cn_high;
    uint16_t modification_time;
    uint16_t modification_date;
    uint16_t cn;
    uint32_t size;
}__attribute__((packed));

struct BPBHeader_t
{
    uint8_t _jmp[3];
    char oem_iden[8];
    uint16_t sector_size;
    uint8_t n_sectors_per_cluster;
    uint16_t n_reserved_clusters;
    uint8_t n_fats;
    uint16_t n_dir_entities;
    uint16_t n_total_sectors;
    uint8_t media_type;
    uint16_t n_sectors_per_fat;
    uint16_t n_sectors_per_cyliner;
    uint16_t n_heads;
    uint32_t n_hidden_sectors;
    uint32_t n_total_sectors_large;
}__attribute__((packed));

extern struct BPBHeader_t BPBHEADER;
bool init_fat();
int fat12_ls(int rel_cn, int offset, struct FAT_File_t *file);
bool fat12_read(const struct FAT_File_t *file, void *dest, size_t size);

#endif
