#include "state.h"

#ifndef SCREEN_H
#define SCREEN_H

class Screen
{   
    
    struct buffer* buf;
    State& _state;

public:
    Screen(State& state);
    
    void refreshScreen();
    void statusBar();
    void drawRows(); 
    void welcomeMsg();
    void displayText();
    void cursorPosition(const int row, const int col);
};

#endif