typedef unsigned char bool;
typedef unsigned char byte;

#define false 0
#define true 1

#define BIND_ARG(ARG) if (!SDL_strcmp(argv[counter], ARG))

#define APP_TITLE "Pong"

#define Length(array) (sizeof(array) / sizeof(array[0]))
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define NORMALIZE(x, max) (x > 0) ? max : -max

#define KEY_PRESSED(SDL_CODE, EVENT) if (EVENT.key.keysym.sym == SDL_CODE)

#define foreach(array) for (int counter = 0; counter < Length(array); ++counter)

#define PADDLE_PADDING 40
#define PADDLEW_COEFICIENT 0.0275
#define PADDLEH_COEFICIENT 0.11
#define BALL_RADIUS_COEFICIENT 0.01875

#define CENTER(SCREEN, DIMENSION) ((SCREEN / 2) - (DIMENSION / 2))

#define BALL Pong.ball
#define PADDLES Pong.paddles

#define BOTTOM(element) (element.Rect.y + element.Rect.h)
#define MIDDLE(element) (element.Rect.y + (element.Rect.h / 2))
#define RIGHT(element) (element.Rect.x + element.Rect.w)

#define WIDTH_VELOCITY ((Pong.WindowWidth) / 700)

#define BALL_VELOCITY 0.4f
#define BALL_Y_RANDOM (((rand() % (4 + 1)) / 10) - 0.2f)