typedef struct
{
	int y;
	bool PaddleUp;
	bool PaddleDown;
	SDL_Rect Rect;
	int KeyUp;
	int KeyDown;
} Paddle;

typedef struct
{
	SDL_Rect Rect;
	float XVel;
	float YVel;
	bool Cooldown;
	float StartingXVelocity;
	float VelocityCoeficient;
} Ball;

typedef struct
{
	Paddle paddles[2];
	Ball ball;
	bool IsRunning;
	bool IsPaused;
	double delta;
	long int CurrentTick;
	long int LastCollisionTick;
	long int PauseTick;
	unsigned int LastScoreMS;
	bool CollisionDetected;
	byte PlayerScore[2];
	int WindowWidth;
	int WindowHeight;
	TTF_Font *Font;
} Game;

SDL_Window *MainWindow;
SDL_Renderer *Renderer;
Game Pong;

static void InitGame();
static void RunGame();
static inline void DestroyGame();
static void Simulate();
static inline void RestartBall();

static inline void PaddleResize()
{
	foreach (PADDLES)
	{
		Pong.paddles[counter].Rect.w = Pong.WindowWidth * PADDLEW_COEFICIENT;
		Pong.paddles[counter].Rect.h = Pong.WindowHeight * PADDLEH_COEFICIENT;
	}
	Pong.paddles[0].Rect.x = PADDLE_PADDING;
	Pong.paddles[1].Rect.x = Pong.WindowWidth - Pong.paddles[1].Rect.w - PADDLE_PADDING;
}

static inline void BallResize()
{
	BALL.Rect.w = Pong.WindowWidth * BALL_RADIUS_COEFICIENT;
	BALL.Rect.h = Pong.WindowWidth * BALL_RADIUS_COEFICIENT;
}

static inline void ResizeScreen()
{
	SDL_GetWindowSize(MainWindow, &Pong.WindowWidth, &Pong.WindowHeight);
	BallResize();
	PaddleResize();
	printf("Screen resized to: %dx%d\n", Pong.WindowWidth, Pong.WindowHeight);
}

static inline void AddScore(unsigned short int PlayerIndex)
{
	Pong.PlayerScore[PlayerIndex - 1]++;
	printf("\n\n=====================\n");
	printf("Player %d scored!\n", PlayerIndex);
	printf("Current score:\n");
	printf("Player 1: %d | Player 2: %d\n", Pong.PlayerScore[0], Pong.PlayerScore[1]);
	printf("=====================\n\n");
	Pong.LastScoreMS = SDL_GetTicks();
	BALL.Cooldown = true;
	RestartBall();
}

static inline void RestartBall()
{
	BALL.Rect.x = CENTER(Pong.WindowWidth, Pong.ball.Rect.w);
	BALL.Rect.y = CENTER(Pong.WindowHeight, Pong.ball.Rect.h);
	BALL.YVel = BALL_Y_RANDOM;
	int RandomXVelCoef = (rand() > RAND_MAX / 2) ? 1 : -1;
	BALL.XVel = BALL.StartingXVelocity * RandomXVelCoef;
}

static void Simulate()
{
	// Simulate the paddles
	foreach (Pong.paddles)
	{
#define PADDLE_VELOCITY ((1 * Pong.delta) * 0.5f * (Pong.WindowHeight / 570))
		if (!(Pong.paddles[counter].PaddleUp && Pong.paddles[counter].PaddleDown))
		{
			if (Pong.paddles[counter].PaddleUp == true)
			{
				Pong.paddles[counter].y -= PADDLE_VELOCITY;
			}
			else if (Pong.paddles[counter].PaddleDown == true)
			{
				Pong.paddles[counter].y += PADDLE_VELOCITY;
			}
		}
		Pong.paddles[counter].y = CLAMP(Pong.paddles[counter].y, 0, Pong.WindowHeight - Pong.paddles[counter].Rect.h);
		Pong.paddles[counter].Rect.y = Pong.paddles[counter].y;
	}

	// Simulate the "ball". It only moves if the cooldown is set to false.
	if (BALL.Cooldown == false)
	{
		BALL.Rect.x += BALL.XVel * Pong.delta * WIDTH_VELOCITY;
		BALL.Rect.y += BALL.YVel * Pong.delta;
	}

	// Collision Detection
	foreach (PADDLES) // Collision with the paddles
	{
		if ((RIGHT(BALL) >= PADDLES[counter].Rect.x && BALL.Rect.x <= RIGHT(PADDLES[counter])) &&
				(BALL.Rect.y <= BOTTOM(PADDLES[counter]) && BOTTOM(BALL) >= PADDLES[counter].Rect.y))
		{
			// Makes sure that the collision resolution is only run once, thus avoiding bugs on the ball direction change
			if (Pong.CurrentTick > Pong.LastCollisionTick + 2)
			{
				Pong.CollisionDetected = true;
				BALL.XVel = -(BALL.XVel);

				float TopRatio = 0;
				float BottomRatio = 0;

				// The ball hit the top half of the current paddle
				if (MIDDLE(BALL) < MIDDLE(PADDLES[counter]))
				{
					TopRatio = (int)((MIDDLE(PADDLES[counter]) - MIDDLE(BALL)) * 100) / (MIDDLE(PADDLES[counter]) - PADDLES[counter].Rect.y);
					BALL.YVel = -BALL_VELOCITY * (TopRatio / 100);
				}
				else
				{
					BottomRatio = (int)abs((((BOTTOM(PADDLES[counter]) - MIDDLE(BALL)) * 100) / (BOTTOM(PADDLES[counter]) - MIDDLE(PADDLES[counter]))) - 100);
					BALL.YVel = BALL_VELOCITY * (BottomRatio / 100);
				}

				// This makes so when the ball is traveling solely on the x axis, it gains more speed, allowing more interesting plays.
				BALL.XVel = (1.f - fabs(BALL.YVel) + BALL.VelocityCoeficient) * ((BALL.XVel > 0) ? 1 : -1);
				BALL.XVel = CLAMP(BALL.XVel, -0.6f, 0.6f);
				printf("Yvel = %f XVel = %f\n", BALL.YVel, BALL.XVel);
				Pong.LastCollisionTick = Pong.CurrentTick;
			}
		}
		else
		{
			Pong.CollisionDetected = false;
		}
	}

	// Collision with the screen borders
	if (BOTTOM(BALL) >= Pong.WindowHeight || BALL.Rect.y <= 1 && Pong.CurrentTick > (Pong.LastCollisionTick + 12))
	{
		BALL.YVel = -BALL.YVel;
		Pong.LastCollisionTick = Pong.CurrentTick;
	}
	if (BALL.Rect.x <= 0)
	{
		AddScore(2);
	}
	if (BALL.Rect.x >= Pong.WindowWidth)
	{
		AddScore(1);
	}
}

static void InitGame()
{
	printf("Starting window with: %dx%d\n", Pong.WindowWidth, Pong.WindowHeight);

	Pong.CurrentTick = 0;
	Pong.IsRunning = true;
	Pong.IsPaused = false;
	Pong.CollisionDetected = false;
	foreach (Pong.PlayerScore)
	{
		Pong.PlayerScore[counter] = 0;
	}

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	Pong.Font = TTF_OpenFont("AtariSmall.ttf", 16);

	MainWindow = SDL_CreateWindow(APP_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Pong.WindowWidth, Pong.WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	Renderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_GetWindowSize(MainWindow, &Pong.WindowWidth, &Pong.WindowHeight);

	{ // Init all the "ball" properties
		Pong.ball.Rect.w = Pong.WindowWidth * BALL_RADIUS_COEFICIENT;
		Pong.ball.Rect.h = Pong.WindowWidth * BALL_RADIUS_COEFICIENT;

		Pong.ball.Rect.x = CENTER(Pong.WindowWidth, Pong.ball.Rect.w);
		Pong.ball.Rect.y = CENTER(Pong.WindowHeight, Pong.ball.Rect.h);
		BALL.StartingXVelocity = 0.24f;
		Pong.ball.XVel = BALL.StartingXVelocity;
		Pong.ball.YVel = BALL_Y_RANDOM; // Generates random number between -0.2 to 0.2
		Pong.ball.Cooldown = false;
		BALL.VelocityCoeficient = 0.03f;
	}

	// Defining paddles width as 50px and height as 190px and centering on the y axis
	foreach (Pong.paddles)
	{
		Pong.paddles[counter].y = CENTER(Pong.WindowHeight, Pong.paddles[counter].Rect.h);
	}

	// Init paddles x coordinate
	PaddleResize();

	Pong.paddles[0].KeyUp = SDLK_w;
	Pong.paddles[0].KeyDown = SDLK_s;

	Pong.paddles[1].KeyUp = SDLK_UP;
	Pong.paddles[1].KeyDown = SDLK_DOWN;
}

static void RunGame()
{
	Uint64 Now = SDL_GetPerformanceCounter();
	Uint64 Last = 0;
	unsigned int CurrentTime = 0, LastTime = 0;
	while (Pong.IsRunning == true)
	{
		Last = Now;
		Now = SDL_GetPerformanceCounter();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{

				case SDL_WINDOWEVENT_RESIZED:
				{
					printf("WINDOW EVENT\n");
					ResizeScreen();
					break;
				}
				}
				break;
			}
			case SDL_KEYDOWN:
			{
				foreach (Pong.paddles)
				{
					KEY_PRESSED(Pong.paddles[counter].KeyUp, event)
					{
						Pong.paddles[counter].PaddleUp = true;
					}
					KEY_PRESSED(Pong.paddles[counter].KeyDown, event)
					{
						Pong.paddles[counter].PaddleDown = true;
					}
				}
				break;
			}
			case SDL_KEYUP:
			{
				foreach (Pong.paddles)
				{
					KEY_PRESSED(Pong.paddles[counter].KeyUp, event)
					{
						Pong.paddles[counter].PaddleUp = false;
					}
					KEY_PRESSED(Pong.paddles[counter].KeyDown, event)
					{
						Pong.paddles[counter].PaddleDown = false;
					}
					KEY_PRESSED(SDLK_RETURN, event)
					{
						// This makes sure the pause button is not spammed, thus not allowing the command
						if (Pong.CurrentTick > (Pong.PauseTick + 10))
						{
							Pong.IsPaused = (Pong.IsPaused == false) ? true : false;
							Pong.PauseTick = Pong.CurrentTick;
						}
					}
				}
				break;
			}
			case SDL_QUIT:
			{
				Pong.IsRunning = false;
				break;
			}
			}
		}

		if (Pong.IsPaused == false)
		{
			Simulate();
		}

		{ // This draws the entire screen

			//	Clearing the screen
			SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
			SDL_RenderClear(Renderer);

			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);

			char ScoreString[3];
			sprintf(ScoreString, "%d %d", Pong.PlayerScore[0], Pong.PlayerScore[1]);

			SDL_Surface *ScoreMessage = TTF_RenderText_Solid(Pong.Font, (const char *)ScoreString, (SDL_Color){255, 255, 255});

			SDL_Texture *ScoreTexture = SDL_CreateTextureFromSurface(Renderer, ScoreMessage);

			SDL_Rect ScoreRect = {CENTER(Pong.WindowWidth, Pong.WindowWidth * 0.2), 20, Pong.WindowWidth * 0.2, (Pong.WindowHeight * 0.19)};

			SDL_RenderCopy(Renderer, ScoreTexture, NULL, &ScoreRect);

			SDL_FreeSurface(ScoreMessage);
			SDL_DestroyTexture(ScoreTexture);

			foreach (Pong.paddles)
			{
				SDL_RenderFillRect(Renderer, &Pong.paddles[counter].Rect);
			}
			SDL_RenderFillRect(Renderer, &Pong.ball.Rect);
			SDL_RenderPresent(Renderer);
		}

		CurrentTime = SDL_GetTicks();

		if (BALL.Cooldown == true && CurrentTime > Pong.LastScoreMS + 1200) // This allows a 1.2 second cooldown after a player scores
		{
			BALL.Cooldown = false;
		}

		if (CurrentTime > LastTime + 1000)
		{
			// Event to be run every second
			LastTime = CurrentTime;
		}

		Pong.CurrentTick++;
		Now = SDL_GetPerformanceCounter();
		Pong.delta = (double)((Now - Last) * 1000 / (double)SDL_GetPerformanceFrequency());
	}
}

static inline void DestroyGame()
{
	SDL_DestroyWindow(MainWindow);
	TTF_Quit();
	SDL_Quit();
}