#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include "utils.c"
#include "game.c"

static inline void ParseArguments(int argc, char *argv[])
{
    for (int counter = 0; counter < argc; ++counter)
    {
        printf("argument[%d]  = %s\n", counter, argv[counter]);
        BIND_ARG("-w")
        {
            int NewWidth = atoi(argv[counter + 1]);
            Pong.WindowWidth = NewWidth;
        }
        BIND_ARG("-h")
        {
            int NewHeight = atoi(argv[counter + 1]);
            Pong.WindowHeight = NewHeight;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        ParseArguments(argc, argv);
    }
    else
    {
        Pong.WindowWidth = 800;
        Pong.WindowHeight = 600;
    }

    InitGame();
    RunGame();
    DestroyGame();
    return 0;
}