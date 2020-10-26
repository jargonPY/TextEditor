#include "state.h"
#include "keypress.h"
#include "screen.h"


int main(int argc, char *argv[]) 
{ 
    State state;
    KeyPress press (state);
    Screen screen (state);

    if (argc >= 2) {
      state.openFile(argv[1]);
    }

    while (1) {
      // render the user's interface after each keypress
      screen.refreshScreen();
      // waits for a keypress and handles it
      press.processKey();
    }  
}