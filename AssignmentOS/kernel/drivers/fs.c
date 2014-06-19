#include <stdint.h>
#include <stdbool.h>
#include "fs.h"
#include "utils.h"

bool open_dir(const struct File_t *dir, struct File_t *sub_item)
{
    if(dir->type != DIRECTORY)
        return false;
    sub_item->p_location = dir->location;
    sub_item->next_item = 0;
    return get_next_file(sub_item);
}

bool open_dir_path(const char *path, struct File_t *sub_item)
{
    if(strncmp(path, "/fd0/", 5) != 0)
        return false;

    struct File_t _sub_item;
    const char *current = path + 5, *next_slash = strchr(current, '/');
    _sub_item.location = 0;
    _sub_item.type = DIRECTORY;
    if(!open_dir(&_sub_item, &_sub_item))
        return false;
    while(next_slash) {
        const char *p = current;
        bool found = true;
        int idx = 0;
        while(p != next_slash && idx < 13) {
            if(toupper(*p++) != _sub_item.filename[idx++]) {
                found = false;
                break;
            }
        }
        if(found && *p == '/' && _sub_item.filename[idx] == '\0') { /* must be sure that the names have ended */
            if(!open_dir(&_sub_item, &_sub_item))
                return false;
            current = p + 1;
            next_slash = strchr(current, '/');
        } else {
            if(!get_next_file(&_sub_item))
                return false;
        }
    }
    *sub_item = _sub_item;
    return true;
}

bool get_next_file(struct File_t *item)
{
    int i, j;
    struct FAT_File_t _item;

    item->next_item = fat12_ls(item->p_location, item->next_item, &_item);
    if(item->next_item == 0)
        return false;

    for(i = 0; i < 8; ++i) {
        if(_item.filename[i] == ' ')
            break;
        else
            item->filename[i] = _item.filename[i];
    }
    if(_item.ext[0] != ' ') {
        item->filename[i++] = '.';
        for(j = 0; j < 3; ++j) {
            if(_item.ext[j] == ' ')
                break;
            else
                item->filename[i + j] = _item.ext[j];
        }
        item->filename[i + j] = '\0';
    } else
        item->filename[i] = '\0';

    item->location = _item.cn;
    item->size = _item.size;
    if(_item.attr & FAT_DIRECTORY)
        item->type = DIRECTORY;
    else if(_item.attr & FAT_ARCHIVE)
        item->type = ARCHIVE;

    return true;
}

size_t read_file(const struct File_t *file, void *dest)
{
    if(file->type != ARCHIVE)
        return 0;
    struct FAT_File_t _file;
    _file.cn = file->location;
    _file.size = file->size;
    if(fat12_read(&_file, dest, 0))
        return _file.size;
    else
        return 0;
}

size_t read_file_path(const char *path, void *dest)
{
    struct File_t file;
    if(!open_file_path(path, &file))
        return 0;
    return read_file(&file, dest);
}

bool open_file_path(const char *path, struct File_t *file)
{
    struct File_t _file;
    if(open_dir_path(path, &_file)) {
        const char *filename = strrchr(path, '/') + 1;
        do{
            const char *current = filename;
            bool found = true;
            int i = 0;
            while(*current && i < 13) {
                if(_file.filename[i++] != toupper(*current++)) {
                    found = false;
                    break;
                }
            }
            if(found && *current == '\0' && _file.filename[i] == '\0') {
                *file = _file;
                return true;
            }
        }while(get_next_file(&_file));
    }
    return false;
}
