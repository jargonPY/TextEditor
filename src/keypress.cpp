#include <unistd.h>
#include "keypress.h"


KeyPress::KeyPress(State& state)
    :  _state (state) {}

void KeyPress::processKey()
{
    char c = readKey();
    switch (c) 
    {
        case '\0':
            break;
        case 'U':
            if (!_state.get_esp())
                _state.moveUp();
            break;
        case 'D':
            if (!_state.get_esp())
                _state.moveDown();
            break;
        case 'L':
            _state.moveLeft();
            break;
        case 'R':
            _state.moveRight();
            break;
        case 'G':
            if (_state.get_esp())
                _state.deleteCharEsp();
            else
                _state.deleteCharText();
            break;
        case '\x1b':
            _state.escape();
            break;
        case '\n':
            if (_state.get_esp())
                _state.espCommand();
            else
                _state.newLine();
            break;
        default:
            if (_state.get_esp())
                _state.writeCharEsp(c);
            else
                _state.writeCharText(c);
    }
}

char KeyPress::readKey()
{
    int value;
    char c;
    // read is non-blocking
    value = read(STDIN_FILENO, &c, 1);
    if (value == 1) {
        if (iscntrl(c)) 
        {
            if (c == (wchar_t)(127)) return 'G';
            else if (c == (wchar_t)(13)) return '\n';
            else c = arrowKeys();
        }
        return c;
    } else {
        return '\0';
    }
}

char KeyPress::arrowKeys()
{
    // Arrow keys are represented as '\x1b', '[', and 'A', 'B', 'C', 'D'
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
    if (seq[0] == '[') 
    {
        switch (seq[1]) 
        {
            case 'A': return 'U';
            case 'B': return 'D';
            case 'C': return 'R';
            case 'D': return 'L';
        }
    }
        return '\x1b';
}