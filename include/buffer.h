#include <string>

#ifndef BUFFER_H
#define BUFFER_H

struct buffer 
{
        int len;
        char *ptr;

        buffer();
        void bufAppend(const char *s, int new_len);
        void bufAppend(const char *s, int new_len, int loc);
        void bufOpenFile(const std::string *s);
        void bufAppendChar(const char s);
        void bufAppendChar(const char s, int loc);
        void bufDelete(int loc);
        void clearAppend(const char* str, int size);
        ~buffer();
};

#endif
