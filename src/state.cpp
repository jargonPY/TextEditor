#include <ctype.h>
#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <string>
#include "state.h"


State::State()
    : _cx (0), _cy (0), _numrows (0), _esp (), _row ()
{
    getWindowSize();
    enableRawMode();       
}

State::~State()
{
    disableRawMode();
}


/* Set Up and Tear Down Methods */


void State::getWindowSize()
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        die("getWindowSize");
    } else
        {
            _screenrows = ws.ws_row;
            _screencols = ws.ws_col;
        }
}

void State::disableRawMode() 
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &_orig_termios) == -1)
        die("tcsetattr");
}

void State::enableRawMode() 
{
    if (tcgetattr(STDIN_FILENO, &_orig_termios) == -1) die("tcgetattr");

    struct termios raw = _orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void State::die(const char *s) 
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    
    perror(s);
    exit(1);
}

void State::quit()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    disableRawMode();
    exit(0);
}

void State::moveLeft()
{
    if (_cx == 0 && _cy == 0) return;
    if (_esp.esp && _cx > 0) {
        _cx--;
        return;
    }
    if (_cx == 0) {
        int x = 0, y = 0;
        for (int i = 0; i < _row.len; i++)
        {
            if (y == _cy - 1) 
            {   
                x++;
                if (_row.ptr[i] == '\r')
                {
                    _cx = x - 1;
                    _cy--;
                    break;
                }
            }
            if (_row.ptr[i] == '\n') y++;
        }
    } else _cx--;
}

void State::moveRight()
{
    if (_esp.esp) {
        _cx++;
        return;
    }
    int x = 0, y = 0;
    for (int i = 0; i < _row.len; i++)
    {
        if (y == _cy) 
        {
            if (_row.ptr[i] == '\r'){
                _cy++;
                _cx = 0;
                break;
            } else if (_cx == x && _row.ptr[i] != '\n') {
                _cx++;
                break;
            }
            x++;
        }
        if (_row.ptr[i] == '\n') y++;
    }
}

void State::moveUp()
{
    if (_cy == 0) return;

    int x = 0, y = 0;
    for (int i = 0; i < _row.len; i++)
    {
        if (_row.ptr[i] == '\n') y++;
        if (y == _cy-1) 
        {
            if (_cx == x) {
                _cy--;
                break;
            } else if (_row.ptr[i+1] == '\r') {
                _cy--;
                _cx = x;
                break;
            }
            x++;
        }
    }
}

void State::moveDown()
{
    if (_cy >= _numrows) return;

    int x = 0, y = 0;
    for (int i = 0; i < _row.len; i++)
    {
        if (_row.ptr[i] == '\n') y++;
        if (y == _cy+1) 
        {
            if (_cx == x) {
                _cy++;
                break;
            } else if (_row.ptr[i+1] == '\r' || i+1 == _row.len) {
                _cy++;
                _cx = x;
                break;
            }
            x++;
        }
    }
}

void State::escape()
{
    _esp.esp = !_esp.esp;
    if (_esp.esp)
    {
        _esp.pos_y = _cy;
        _esp.pos_x = _cx;
        _cy = _screenrows;
        _cx = 0;
    } else {
        _cy = _esp.pos_y;
        _cx = _esp.pos_x;
        // might cause a memory leak
        _esp.cmd.ptr = NULL;
        _esp.cmd.len = 0;
    }
}

/* Methods for editing text */

void State::newLine(bool readFile)
{
    if (!readFile) {
        int loc;
        charLoc(loc);
        _row.bufAppend("\r\n", 2, loc);
    } else {
        _row.bufAppend("\r\n", 2);
    }
    _cy += 1;
    _cx = 0;
    _numrows++;
}

void State::prevLine(int& loc, int& prev_col)
{
    int counter = 0;
    for (int i = 0; i < _row.len; i++)
    {
        prev_col++;
        if (_row.ptr[i] == '\n') {
            counter++;
            if (counter == _cy) {
                loc = i;
                prev_col = prev_col - 2;
                break;
            } else prev_col = 0;
        }
    }
    _numrows--;
}

void State::charLoc(int& loc)
{
    int x_count = 0, y_count = 0;
    if (_row.len == 0) loc = 0;
    else
    {
        for (int i = 0; i < _row.len; i++)
        {
            if (y_count == _cy) 
            {
                if (x_count == _cx) 
                {
                    loc = i;
                    break;
                }
                x_count++; 
            }
            if (_row.ptr[i] == '\n') y_count++;
            loc = i+1;
        }
    }
}

/* Methods for appending/deleting from main text */

void State::writeCharText(char key)
{   
    int loc;
    charLoc(loc);
    _row.bufAppendChar(key, loc);
    moveRight();
}

void State::deleteCharText()
{
    int loc;
    if (_cy == 0 && _cx == 0) return;
    else if (_cy == 0) loc = _cx - 1;
    else if (_cx == 0) {
        int prev_col = 0;
        // deletes the letters
        prevLine(loc, prev_col);
        _row.bufDelete(loc);
        _row.bufDelete(loc - 1);
        _cy--;
        _cx = prev_col;
        return;
    }
    else {
        charLoc(loc);
        loc--;
    }
    _row.bufDelete(loc);
    moveLeft();
}

/* Methods for appending/deleting from escape text */

void State::writeCharEsp(char key)
    {
        _esp.cmd.bufAppendChar(key);
        moveRight();
    }

void State::deleteCharEsp()
{
    int loc;
    if (_cx == 0) return;
    else loc = _cx;
    _esp.cmd.bufDelete(loc);
    moveLeft();
}

void State::espCommand()
{
    char command [4], arg [30];
    int len_cmd = 0;
    for (int i = 0; i < _esp.cmd.len; i++)
    {
        if (_esp.cmd.ptr[i] == ' ') {
            len_cmd = i;
            continue;
        }

        if (len_cmd == 0) command[i] = _esp.cmd.ptr[i];
        else arg[i - len_cmd - 1] = _esp.cmd.ptr[i];

        if (i+1 == _esp.cmd.len && len_cmd == 0) len_cmd = i+1;
    }
    command[len_cmd] = '\0';
    arg[_esp.cmd.len - len_cmd - 1] = '\0';

    if (strcmp(command, "w") == 0) {
        writeFile(arg);
        _esp.cmd.clearAppend("File saved successfully!", 25);

    } else if (strcmp(command, "q") == 0)
        quit();
}

/* Methods for handling files */

void State::openFile(char *filename)
{
    std::ifstream file (filename);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            _row.bufOpenFile(&line);
            newLine(true);
        }
        file.close();
    } else die("Failed to open file");
}

void State::writeFile(char *filename)
{
    std::ofstream file (filename);
    if (file.is_open())
    {
        file << _row.ptr;
        file.close();
    }
    else die("Failed to open file");
}

/* Methods for accessing private members */

const int State::get_cx() {return _cx;}
const int State::get_cy() {return _cy;}
const int State::get_numrows() {return _numrows;}
const int State::get_screenrows() {return _screenrows;}
const int State::get_screencols() {return _screencols;}
const bool State::get_esp() {return _esp.esp;}
const struct buffer& State::get_cmd() {return _esp.cmd;}
const struct buffer& State::get_row() {return _row;}
