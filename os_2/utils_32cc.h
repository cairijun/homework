char get_kb_char();
void clear_scn();
void write_str(const char *, unsigned short, unsigned short);
void write_char(char);
void set_char(char, unsigned short);
void read_disk(void *, unsigned char, unsigned short, unsigned char);
void sleep(unsigned short);
void move_cursor(unsigned short);
unsigned short get_cursor();
char get_char();

inline void write_str_current(const char *p_str, unsigned short len)
{
    while(len--)
        write_char(*p_str++);
}

inline char * strchr_(char *p_str, char c)
{
    while(*p_str != c && *p_str != '\0')
        ++p_str;
    return p_str;
}

inline void strncpy_(char *dest, const char *source, unsigned short len)
{
    __asm__ volatile(
        ".intel_syntax;"
        "cld;"
        "rep movsb;"
        ".att_syntax;"
        : /* no output */
        : "c"(len), "D"(dest), "S"(source)
        : "memory"
    );
    dest[len] = '\0';
}

inline char strncmp_(const char *a, const char *b, unsigned short len)
{
    __asm__ volatile(
        ".intel_syntax;"
        "cld;"
        "rep cmpsb;"
        ".att_syntax;"
        : "=c"(len)
        : "c"(len+1), "D"(a), "S"(b)
        : "cc"
    );
    return len;
}
