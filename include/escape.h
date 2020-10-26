#include "buffer.h"

#ifndef ESCAPE_H
#define ESCAPE_H

struct escape 
{
    bool esp;
    int pos_x;
    int pos_y;
    struct buffer cmd;

    escape();
};

#endif