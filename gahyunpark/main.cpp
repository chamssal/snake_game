#include "SnakeGame.h"
#include <ncurses.h>
#include <cstdio>

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    SnakeGame game(22, 22, 1);
    game.run();

    endwin();
    return 0;
}
