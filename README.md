# Text Editor

A text editor similar in style to the vi editor (albeit not as many features).
Instructions to run the editor:
  1. Clone the repo
  2. Open the cloned directory in the terminal and type
  `code()`   
  g++ ./src/*.cpp -g  -I ../include -o ../bin/editor
  
  (alternatively you can run the compiled file in the bin folder if you are comfortable running an executable)
  3. Type `code()` ./bin/editor to run editor
  4. Enjoy!
 
 P.S. <br />
 Credits to https://viewsourcecode.org/snaptoken/kilo for helping me understand the terminal library (termios) and the VT100 escape sequences.

