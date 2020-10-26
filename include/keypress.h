#include "state.h"

#ifndef KEY_H
#define KEY_H

class KeyPress
{
    State& _state;

public:
    KeyPress(State& state);
    void processKey();
    char readKey();
    char arrowKeys();
};

#endif
