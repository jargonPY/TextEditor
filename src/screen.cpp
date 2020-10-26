#include <unistd.h>
#include "screen.h"

#define EDITOR_VERSION "0.0.1"


Screen::Screen(State& state)
    :  _state (state) {}

void Screen::refreshScreen() 
{
    struct buffer buffer;
    buf = &buffer;
    buf->bufAppend("\x1b[?25l", 6);
    _state.getWindowSize();
    // clears entire screen
    buf->bufAppend("\x1b[2J", 4);
    
    drawRows();
    if (_state.get_esp()) statusBar();

    cursorPosition(_state.get_cy()+1, _state.get_cx()+1);
    buf->bufAppend("\x1b[?25h", 6); // Shows flicker again
    write(STDOUT_FILENO, buf->ptr, buf->len);
}

void Screen::statusBar() 
{
    buf->bufAppend("\x1b[7m", 4);
    int len = 0;
    while (len < _state.get_screencols()) 
    {
        if (_state.get_cmd().len > len)
        {
            int row = _state.get_cy(), col = _state.get_cx();
            buf->bufAppend(_state.get_cmd().ptr, _state.get_cmd().len);
            len += _state.get_cmd().len;
        } else {
            buf->bufAppend(" ", 1);
            len++;
        }
    }
    buf->bufAppend("\x1b[m", 3);
}

void Screen::drawRows() 
{
    int len = _state.get_row().len;
    for (int y = 0; y < _state.get_screenrows() - 1; y++) 
    {
        if (len > 0 && y == 0)
        {
            displayText();
        } else if (y > _state.get_numrows())
        {
            buf->bufAppend("\r\n", 2);
            buf->bufAppend("~", 1);
        }

        if (len == 0 && y == _state.get_screenrows() / 3)
            welcomeMsg();
    }
    buf->bufAppend("\r\n", 2);
}

void Screen::welcomeMsg()
{
    char welcome[80];
    int welcomelen = snprintf(welcome, sizeof(welcome), "HELVEGEN -- version %s", EDITOR_VERSION);
    if (welcomelen > _state.get_screencols()) welcomelen = _state.get_screencols();
    // Center the welcome message
    int padding = (_state.get_screencols() - welcomelen) / 2;
    if (padding) 
    {
        padding--;
    }
    while (padding--) buf->bufAppend(" ", 1);
    buf->bufAppend(welcome, welcomelen);
}

void Screen::displayText()
{
    int row = 0, col = 0;
    cursorPosition(row, col);
    buf->bufAppend(_state.get_row().ptr, _state.get_row().len);
}

void Screen::cursorPosition(const int row, const int col)
{
    char buff[32];
    snprintf(buff, sizeof(buff), "\033[%d;%dH", row, col);
    buf->bufAppend(buff, strlen(buff));
}
