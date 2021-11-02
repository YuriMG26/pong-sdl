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
	bool CollisionDetected;
	byte PlayerScore[2];
} Game;

SDL_Window *MainWindow;
SDL_Renderer *Renderer;
Game Pong;

static void InitGame();
static void RunGame();
static inline void DestroyGame();
static void Simulate();
static inline void RestartBall();

static inline void AddScore(unsigned int PlayerIndex)
{
	Pong.PlayerScore[PlayerIndex - 1]++;
	printf("\n\nPlayer %d scored!\n", PlayerIndex);
	printf("Current score:\n");
	printf("Player 1: %d | Player 2: %d\n", Pong.PlayerScore[0], Pong.PlayerScore[1]);
	RestartBall();
}

static inline void RestartBall()
{
	BALL.Rect.x = CENTER(SCREEN_WIDTH, Pong.ball.Rect.w);
	BALL.Rect.y = CENTER(SCREEN_HEIGHT, Pong.ball.Rect.h);
}

static void Simulate()
{
	// Simulate the paddles
	foreach (Pong.paddles)
	{
		if (!(Pong.paddles[counter].PaddleUp && Pong.paddles[counter].PaddleDown))
		{
			if (Pong.paddles[counter].PaddleUp == true)
			{
				Pong.paddles[counter].y -= (1 * Pong.delta) * 0.5f;
			}
			else if (Pong.paddles[counter].PaddleDown == true)
			{
				Pong.paddles[counter].y += (1 * Pong.delta) * 0.5f;
			}
		}
		Pong.paddles[counter].y = CLAMP(Pong.paddles[counter].y, 0, SCREEN_HEIGHT - Pong.paddles[counter].Rect.h);
		Pong.paddles[counter].Rect.y = Pong.paddles[counter].y;
	}

	// Simulate the "ball"

	float xvel = BALL.XVel * Pong.delta;
	float yvel = BALL.YVel * Pong.delta;

	Pong.ball.Rect.x += xvel;
	Pong.ball.Rect.y += yvel;

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
				Pong.LastCollisionTick = Pong.CurrentTick;
			}
		}
		else
		{
			Pong.CollisionDetected = false;
		}
	}

	// Collision with the screen borders
	if (BOTTOM(BALL) >= SCREEN_HEIGHT || BALL.Rect.y <= 1 && Pong.CurrentTick > (Pong.LastCollisionTick + 12))
	{
		BALL.YVel = -BALL.YVel;
		Pong.LastCollisionTick = Pong.CurrentTick;
	}
	if (BALL.Rect.x <= 0)
	{
		AddScore(2);
	}
	if (BALL.Rect.x >= SCREEN_WIDTH)
	{
		AddScore(1);
	}
}

static void InitGame()
{
	Pong.CurrentTick = 0;
	Pong.IsRunning = true;
	Pong.IsPaused = false;
	Pong.CollisionDetected = false;
	foreach (Pong.PlayerScore)
	{
		Pong.PlayerScore[counter] = 0;
	}
	SDL_Init(SDL_INIT_EVERYTHING);

	MainWindow = SDL_CreateWindow("PONG", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

	Renderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	{ // Init all the "ball" properties
		Pong.ball.Rect.w = 15;
		Pong.ball.Rect.h = 15;

		Pong.ball.Rect.x = CENTER(SCREEN_WIDTH, Pong.ball.Rect.w);
		Pong.ball.Rect.y = CENTER(SCREEN_HEIGHT, Pong.ball.Rect.h);
		Pong.ball.XVel = 0.4f;
		Pong.ball.YVel = (rand() % (4 + 1)) / 10 - 0.2f; // Generates random number between -0.2 to 0.2
	}

	// Defining paddles width as 50px and height as 190px and centering on the y axis
	foreach (Pong.paddles)
	{
		Pong.paddles[counter].Rect.w = 22;
		Pong.paddles[counter].Rect.h = 70;
		Pong.paddles[counter].y = CENTER(SCREEN_HEIGHT, Pong.paddles[counter].Rect.h);
	}

	// Init paddles x coordinate
	Pong.paddles[0].Rect.x = PADDLE_PADDING;
	Pong.paddles[1].Rect.x = SCREEN_WIDTH - Pong.paddles[1].Rect.w - PADDLE_PADDING;

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

		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);

		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);

		foreach (Pong.paddles)
		{
			SDL_RenderFillRect(Renderer, &Pong.paddles[counter].Rect);
		}
		SDL_RenderFillRect(Renderer, &Pong.ball.Rect);
		SDL_RenderPresent(Renderer);

		CurrentTime = SDL_GetTicks();
		if (CurrentTime > LastTime + 1000)
		{
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
	SDL_Quit();
}