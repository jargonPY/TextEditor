#include "buffer.h"

buffer::buffer()
    : ptr (NULL), len (0) {}

void buffer::bufAppend(const char *s, int new_len)
{
    char *holder = new char [len + new_len];
    for (int i = 0; i < len + new_len; i++) 
    {
        if (i < len) holder[i] = ptr[i];
        else holder[i] = s[i - len];
    }
    delete [] ptr;
    ptr = holder;
    len += new_len;
}

void buffer::bufAppend(const char *s, int new_len, int loc)
{
    int counter = 0;
    char *holder = new char [len + new_len];
    for (int i = 0; i < len + new_len; i++) 
    {
        if (counter == 0) holder[i] = ptr[i];
        if (i >= loc && i < (loc+new_len)) {
            holder[i] = s[i - loc];
            counter++;
        } 
        else holder[i] = ptr[i-counter];
    }
    delete [] ptr;
    ptr = holder;
    len += new_len;
}

void buffer::bufOpenFile(const std::string *s)
{
    char *holder = new char [len + s->size()];
    for (int i = 0; i < len + s->size(); i++)
    {
        if (i < len) holder[i] = ptr[i];
        else holder[i] = s->c_str()[i - len];
    }
    delete [] ptr;
    ptr = holder;
    len += s->size();
}

void buffer::bufAppendChar(const char s)
{
    char *holder = new char [len + 1];
    for (int i = 0; i < len + 1; i++) 
    {
        if (i < len){
            holder[i] = ptr[i];
        } else {
            holder[i] = s;
        }
    }
    delete [] ptr;
    ptr = holder;
    len += 1;
}

void buffer::bufAppendChar(const char s, int loc)
{
    bool counter = false;
    char *holder = new char [len + 1];
    for (int i = 0; i < len + 1; i++) 
    {
        if (i == loc) {
            holder[i] = s;
            counter = true;
            continue;
        }
        if (!counter) holder[i] = ptr[i];
        else holder[i] = ptr[i-1];
    }
    delete [] ptr;
    ptr = holder;
    len += 1;
}

void buffer::bufDelete(int loc)
{
    char *holder = new char [len - 1];
    for (int i = 0; i < len; i++) 
    {
        if (i < loc) holder[i] = ptr[i];
        else if (i > loc) holder[i-1] = ptr[i];
    }
    delete [] ptr;
    ptr = holder;
    len -= 1;
}

void buffer::clearAppend(const char* str, int size)
{
    delete [] ptr;
    len = size;
    ptr = new char [len];
    for (int i = 0; i < len; i++)
        ptr[i] = str[i];
}

buffer::~buffer()
{
    delete [] ptr;
}
