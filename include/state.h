#include <termios.h>
#include "buffer.h"
#include "escape.h"

#ifndef STATE_H
#define STATE_H

class State
{
    int _cx, _cy;
    int _numrows;
    int _screenrows;
    int _screencols;
    struct escape _esp;
    struct buffer _row;
    struct termios _orig_termios;

public:
    State();
    ~State();
    void getWindowSize();
    void disableRawMode(); 
    void enableRawMode();
    void die(const char *s);
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void escape();
    void writeCharText(char key);
    void deleteCharText();
    void prevLine(int& loc, int& prev_col);
    void charLoc(int& loc);
    void writeCharEsp(char key);
    void deleteCharEsp();
    void espCommand();
    void newLine();
    void openFile(char *filename);
    void writeFile(char *filename);
    void quit();
    const int get_cx();
    const int get_cy();
    const int get_numrows();
    const int get_screenrows();
    const int get_screencols();
    const bool get_esp();
    const struct buffer& get_cmd(); 
    const struct buffer& get_row();
};

#endif