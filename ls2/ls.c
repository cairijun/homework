#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>

#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DIRENT 0
#define CSTRING 1

#if defined(__APPLE__) && !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
/* Darwin kernel has different names of the time fields
 * in ``struct stat`` from those of the newest POSIX standard */
    #define ST_MTIM st_mtimespec
#else
    #define ST_MTIM st_mtim
#endif

/*
 * Structure to store information to be printed in long mode.
 * REMEMBER TO RELEASE THE MEMORY!
 */
struct FILE_DESC {
    char mode[11];
    char *nlink_str;
    char *username;
    char *groupname;
    char *size_str;
    char mtime_str[12];
    const char *filename;
};

/*
 * Calculate the maximum width of the output.
 */
int cal_max_width()
{
    struct winsize ws;
    if(!ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) && ws.ws_col > 0)  /* get the width of tty */
        return ws.ws_col;
    return 80;  /* fallback (fail to get the width of tty) */
}

/*
 * Calculate the printed width of a multibyte character string.
 */
size_t mbs_pwidth(const char *s, size_t n)
{
    mbstate_t mbs;
    wchar_t wc;
    size_t clen, width = 0;
    const char * const s_end = s + n;

    memset(&mbs, 0, sizeof(mbs));
    while((clen = mbrtowc(&wc, s, s_end - s, &mbs))) {
        switch(clen) {
            case (size_t)(-1):  /* truncated string (incomplete character) */
                s = s_end;
                ++width;
                break;
            case (size_t)(-2):  /* invalid character */
                ++s;
                ++width;
                break;
            default:  /* valid wide character */
                s += clen;
                if(iswprint(wc))
                    width += wcwidth(wc);
        }
    }

    return width;
}

/*
 * Append ``name`` to ``dir`` to construct a path.
 */
void path_join(char *dest, const char *dir, const char *name)
{
    size_t dir_len = strlen(dir);
    strcpy(dest, dir);
    if(dest[dir_len - 1] != '/')
        dest[dir_len++] = '/';
    strcpy(dest + dir_len, name);
}

/*
 * Print character c for n times.
 */
void repeatedly_print(size_t n, char c)
{
    while(n--)
        putchar(c);
}

/*
 * Print the items of a file list.
 * When ``list_type == 0``, ``items`` is a ``struct dirent **``,
 * otherwise, ``items`` is a ``const char **``;
 */
void print_items(int list_type, void *items, size_t n_items)
{
    if(n_items == 0)
        return;
    size_t i, j, col_len, n_cols, n_rows;
    size_t name_widths[n_items];
    struct dirent **items_dirent;
    const char **items_cstr;
    if(list_type == CSTRING)
        items_cstr = (const char **)items;
    else if(list_type == DIRENT)
        items_dirent = (struct dirent **)items;

    col_len = 0;
    for(i = 0; i < n_items; ++i) {
        if(list_type == CSTRING)
            name_widths[i] = mbs_pwidth(items_cstr[i], strlen(items_cstr[i]) + 1);
        else if(list_type == DIRENT)
            name_widths[i] = mbs_pwidth(items_dirent[i]->d_name, strlen(items_dirent[i]->d_name) + 1);
        if(name_widths[i] > col_len)
            col_len = name_widths[i];
    }
    col_len += 1;

    n_cols = cal_max_width() / col_len;
    if(n_cols == 0)
        n_cols = 1;
    n_rows = (n_items - 1) / n_cols + 1;

    for(i = 0; i < n_rows; ++i) {
        for(j = 0; j < n_cols; ++j) {
            if(n_rows * j + i < n_items) {
                if(list_type == CSTRING)
                    printf("%s", items_cstr[n_rows * j + i]);
                else if(list_type == DIRENT)
                    printf("%s", items_dirent[n_rows * j + i]->d_name);
                repeatedly_print(col_len - name_widths[n_rows * j + i], ' ');
            }
        }
        printf("\n");
    }
}

/*
 * Construct a permission mode string with the given ``mode``.
 * ``mode_str`` should be at least 11 bytes long.
 */
void permission_mode_string(char *mode_str, mode_t mode)
{
    char c;
    switch(mode & S_IFMT) {
        case S_IFBLK:  c = 'b'; break;
        case S_IFCHR:  c = 'c'; break;
        case S_IFDIR:  c = 'd'; break;
        case S_IFLNK:  c = 'l'; break;
        case S_IFSOCK: c = 's'; break;
        case S_IFIFO:  c = 'p'; break;
        default:       c = '-';
    }
    mode_str[0] = c;

    mode_str[1] = (mode & S_IRUSR) ? 'r' : '-';
    mode_str[2] = (mode & S_IWUSR) ? 'w' : '-';
    mode_str[3] = (mode & S_IXUSR) ? 'x' : '-';
    if(mode & S_ISUID)
        mode_str[3] = 's';

    mode_str[4] = (mode & S_IRGRP) ? 'r' : '-';
    mode_str[5] = (mode & S_IWGRP) ? 'w' : '-';
    mode_str[6] = (mode & S_IXGRP) ? 'x' : '-';
    if(mode & S_ISGID)
        mode_str[6] = 's';

    mode_str[7] = (mode & S_IROTH) ? 'r' : '-';
    mode_str[8] = (mode & S_IWOTH) ? 'w' : '-';
    mode_str[9] = (mode & S_IXOTH) ? 'x' : '-';

    mode_str[10] = '\0';
}

/*
 * Get the username of a user specified by ``uid``.
 * THE CALLER IS RESPONSIBLE FOR RELEASING THE RETURNING STRING!
 */
char *get_username(uid_t uid)
{
    errno = 0;  /* to distinguish between non-existence and an error */
    struct passwd *pw = getpwuid(uid);
    if(!pw) {
        if(errno)
            perror(NULL);
        return NULL;
    }

    char *username = (char *)malloc(strlen(pw->pw_name) + 1);
    strcpy(username, pw->pw_name);
    return username;
}

/*
 * Get the groupname of a group specified by ``gid``.
 * THE CALLER IS RESPONSIBLE FOR RELEASING THE RETURNING STRING!
 */
char *get_groupname(gid_t gid)
{
    errno = 0;  /* to distinguish between non-existence and an error */
    struct group *gr = getgrgid(gid);
    if(!gr) {
        if(errno)
            perror(NULL);
        return NULL;
    }

    char *groupname = (char *)malloc(strlen(gr->gr_name) + 1);
    strcpy(groupname, gr->gr_name);
    return groupname;
}

/*
 * Print the items of a file list in long mode.
 */
void print_items_long(int list_type, void *items, size_t n_items, const char *base_dir)
{
    if(n_items == 0)
        return;

    size_t i, base_dir_len = base_dir ? strlen(base_dir) : 0;
    struct stat s;
    const char *filename;
    char *path, *format;
    struct FILE_DESC *file_desc, *tmp_fd;
    size_t fd_i, tmp_len, total_n_blocks;
    size_t max_width_nlink_str = 0, max_width_username = 0,
           max_width_groupname = 0, max_width_size_str = 0;

    fd_i = 0;
    total_n_blocks = 0;
    file_desc = (struct FILE_DESC *)malloc(sizeof(struct FILE_DESC) * n_items);
    /* retrieve detailed information of the files */
    for(i = 0; i < n_items; ++i) {
        tmp_fd = &file_desc[fd_i];
        if(list_type == CSTRING)
            filename = ((const char **)items)[i];
        else if(list_type == DIRENT)
            filename = ((struct dirent **)items)[i]->d_name;

        if(base_dir) {
            /* ``base_dir`` is provided, join ``base_dir`` and ``filename`` */
            path = (char *)malloc(base_dir_len + strlen(filename) + 2);
            path_join(path, base_dir, filename);
            if(lstat(path, &s)) {
                perror(path);
                free(path);
                continue;
            }
            free(path);
        } else {
            /* ``base_dir`` is not provided, use ``filename`` only */
            if(lstat(filename, &s)) {
                perror(filename);
                continue;
            }
        }

        total_n_blocks += s.st_blocks;

        permission_mode_string(tmp_fd->mode, s.st_mode);  /* construct the mode string */

        tmp_len = snprintf(NULL, 0, "%d", s.st_nlink);  /* calulate the required size of memory */
        if(tmp_len > max_width_nlink_str)
            max_width_nlink_str = tmp_len;
        tmp_fd->nlink_str = (char *)malloc(tmp_len + 1);  /* REMEMBER TO RELEASE THIS! */
        snprintf(tmp_fd->nlink_str, tmp_len + 1, "%d", s.st_nlink);

        tmp_fd->username = get_username(s.st_uid);  /* REMEMBER TO RELEASE THIS! */
        tmp_len = strlen(tmp_fd->username);
        if(tmp_len > max_width_username)
            max_width_username = tmp_len;

        tmp_fd->groupname = get_groupname(s.st_gid);  /* REMEMBER TO RELEASE THIS! */
        tmp_len = strlen(tmp_fd->groupname);
        if(tmp_len > max_width_groupname)
            max_width_groupname = tmp_len;

        tmp_len = snprintf(NULL, 0, "%lld", s.st_size);
        if(tmp_len > max_width_size_str)
            max_width_size_str = tmp_len;
        tmp_fd->size_str = (char *)malloc(tmp_len + 1);  /* REMEMBER TO RELEASE THIS! */
        snprintf(tmp_fd->size_str, tmp_len + 1, "%lld", s.st_size);

        strftime(tmp_fd->mtime_str, 12, "%m %e %H:%M",  /* DON'T RELEASE THIS! */
                localtime(&s.ST_MTIM.tv_sec));

        tmp_fd->filename = filename;

        ++fd_i;
    }

    printf("total %ld\n", total_n_blocks);  /* print the total number of blocks */

    tmp_len = snprintf(NULL, 0, "%%-10s %%%lds %%-%lds %%-%lds %%%lds %%11s %%s\n",
            max_width_nlink_str, max_width_username, max_width_groupname, max_width_size_str);
    format = (char *)malloc(tmp_len + 1);
    snprintf(format, tmp_len + 1, "%%-10s %%%lds %%-%lds %%-%lds %%%lds %%11s %%s\n",
            max_width_nlink_str, max_width_username, max_width_groupname, max_width_size_str);

    /* print the file information and release the memory */
    for(i = 0; i < fd_i; ++i) {
        tmp_fd = &file_desc[i];
        printf(format, tmp_fd->mode, tmp_fd->nlink_str,
                tmp_fd->username, tmp_fd->groupname,
                tmp_fd->size_str, tmp_fd->mtime_str,
                tmp_fd->filename);
        free(tmp_fd->nlink_str);
        free(tmp_fd->username);
        free(tmp_fd->groupname);
        free(tmp_fd->size_str);
    }

    free(format);
    free(file_desc);
}

/*
 * Filter when calling ``scandir``. Ignore the hidden files.
 */
int _ignore_hidden(const struct dirent *item)
{
    return !(item->d_name[0] == '.');
}

/*
 * Scan a given directory and print the file list.
 */
int scan_and_print(const char *path, int long_output)
{
    size_t n_items;
    struct dirent **namelist;

    n_items = scandir(path, &namelist, _ignore_hidden, alphasort);
    if(n_items == (size_t)(-1)) {
        perror(path);
        return EXIT_FAILURE;
    }

    if(long_output)
        print_items_long(DIRENT, namelist, n_items, path);
    else
        print_items(DIRENT, namelist, n_items);
    while(n_items--)
        free(namelist[n_items]);
    free(namelist);
    return EXIT_SUCCESS;
}

/*
 * Check if a file specified by ``path`` is a directory.
 */
int is_dir(const char *path)
{
    struct stat s;
    if(stat(path, &s) != 0) {
        perror(path);
        return -1;
    }
    return S_ISDIR(s.st_mode);
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");  /* set locale to make wide character support works correctly */
    int ret, f_long_output = 0;
    if(argc > 1) {  /* check if the "-l" option is provided */
        if(strcmp(argv[1], "-l") == 0) {
            f_long_output = 1;
            --argc;
            ++argv;
        }
    }
    if(argc == 1) {  /* current directory */
        return scan_and_print(".", f_long_output);
    } else if(argc == 2) {  /* one argument */
        if((ret = is_dir(argv[1])) > 0) {  /* a directory */
            return scan_and_print(argv[1], f_long_output);
        } else if(ret == 0) {  /* not a directory */
            if(f_long_output)
                print_items_long(CSTRING, argv + 1, 1, NULL);
            else
                print_items(CSTRING, argv + 1, 1);
            return EXIT_SUCCESS;
        }
    } else {  /* multiple arguments */
        int i, n_files = 0, n_dirs = 0;
        char *files[argc - 1], *dirs[argc - 1];
        for(i = 1; i < argc; ++i) {  /* First, find non directories and output them */
            if((ret = is_dir(argv[i])) > 0)  /* a directory */
                dirs[n_dirs++] = argv[i];
            else if(ret == 0)  /* not a directory */
                files[n_files++] = argv[i];
            /* else: does not exist, skip */
        }
        if(n_files) {
            if(f_long_output)
                print_items_long(CSTRING, files, n_files, NULL);
            else
                print_items(CSTRING, files, n_files);
            if(n_dirs)
                printf("\n");
        }

        for(i = 0; i < n_dirs; ++i) {  /* Then, list the directories */
            printf("%s:\n", dirs[i]);  /* ought to print the directory name first */
            scan_and_print(dirs[i], f_long_output);
            if(i + 1 < n_dirs)  /* not the last one, print a newline */
                printf("\n");
        }
    }
    return EXIT_SUCCESS;
}

