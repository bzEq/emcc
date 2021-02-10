#include <unistd.h>

#include "editor/line_buffer.h"
#include "support/sys.h"

#include <ncurses.h>
#include <stdio.h>

using namespace emcc;

int main(int argc, char *argv[]) {
  if (argc != 2)
    emcc::Die("Usage %s: <filename>", argv[0]);
  int width, height;
  initscr();
  atexit([]() { endwin(); });
  raw();
  noecho();
  keypad(stdscr, false);
  getmaxyx(stdscr, height, width);
  while (true) {
    int c = getch();
    if (c == 27)
      break;
    addch(c);
    int x, y;
    getyx(stdscr, y, x);
    move(height - 1, 0);
    clrtoeol();
    printw("%d %d", y, x);
    move(y, x);
  }
  return 0;
}
