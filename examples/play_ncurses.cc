#include <unistd.h>

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
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
    printw("%d %d: %d", y, x, c);
    move(y, x);
  }
  return 0;
}
