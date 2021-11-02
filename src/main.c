#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "utils.c"
#include "game.c"

int main(int argc, char *argv[])
{
    InitGame();
    RunGame();
    DestroyGame();
    return 0;
}