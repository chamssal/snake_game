#include "SnakeGame.h"
#include <ncurses.h>
#include <cstdio>

int main() {
    int level;
    printf("Choose a level (1, 2, 3, 4): ");
    scanf("%d", &level);

    SnakeGame game(21, 21, level);
    game.run();
    return 0;
}
