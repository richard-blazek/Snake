#include <SDL2/SDL.h>
#include <time.h>

#define UNIT 25
#define WIDTH 40
#define HEIGHT 20

typedef struct Snake
{
	SDL_Point body[WIDTH * HEIGHT];
	int body_length;
	SDL_Point dir;
} Snake;

Snake make_snake(int len, int head_x, int head_y, int dir_x, int dir_y)
{
	Snake s = {{}, len, {dir_x, dir_y}};
	for (int i = 0; i < len; ++i)
	{
		s.body[i].x = head_x - dir_x * i;
		s.body[i].y = head_y - dir_y * i;
	}
	return s;
}

void move_snake(Snake *s)
{
	for (size_t i = s->body_length - 1; i > 0; --i)
	{
		s->body[i] = s->body[i - 1];
	}
	s->body[0].x += s->dir.x;
	s->body[0].y += s->dir.y;
}

void grow_snake(Snake *s)
{
	s->body[s->body_length] = s->body[s->body_length - 1];
	++s->body_length;
}

void turn_snake(Snake *s, SDL_Point new_dir)
{
	if (!(new_dir.x == -s->dir.x && new_dir.y == -s->dir.y) && abs(s->dir.x) + abs(s->dir.y) == 1)
	{
		s->dir = new_dir;
	}
}

int crashed(Snake *s)
{
	for (int i = 1; i < s->body_length; ++i)
	{
		if (s->body[i].x == s->body[0].x && s->body[i].y == s->body[0].y)
		{
			return 1;
		}
	}
	return 0;
}

int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	srand(time(0));

	SDL_Window *screen = SDL_CreateWindow("Snake", 50, 50, WIDTH * UNIT, HEIGHT * UNIT, 0);
	SDL_Renderer *rend = SDL_CreateRenderer(screen, -1, 0);
	Snake snake = make_snake(2, 10, 10, -1, 0);

	SDL_Point fruit = {12, 12};

	SDL_Event event;
	while (!SDL_QuitRequested() && !crashed(&snake))
	{
		SDL_SetRenderDrawColor(rend, 255, 255, 160, 255);
		SDL_RenderClear(rend);

		SDL_Rect fruit_rect = {fruit.x * UNIT, fruit.y * UNIT, UNIT, UNIT};
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
		SDL_RenderFillRect(rend, &fruit_rect);

		SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
		for (int i = 0; i < snake.body_length; ++i)
		{
			SDL_Rect body_rect = {snake.body[i].x * UNIT, snake.body[i].y * UNIT, UNIT, UNIT};
			SDL_RenderFillRect(rend, &body_rect);
		}
		SDL_RenderPresent(rend);

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				SDL_Point new_dir;
				new_dir.x = event.key.keysym.sym == SDLK_RIGHT ? 1 : event.key.keysym.sym == SDLK_LEFT ? -1 : 0;
				new_dir.y = event.key.keysym.sym == SDLK_DOWN  ? 1 : event.key.keysym.sym == SDLK_UP   ? -1 : 0;
				turn_snake(&snake, new_dir);
				break;
			}
		}
		move_snake(&snake);
		snake.body[0].x = (snake.body[0].x + WIDTH) % WIDTH;
		snake.body[0].y = (snake.body[0].y + HEIGHT) % HEIGHT;

		if (snake.body[0].x == fruit.x && snake.body[0].y == fruit.y)
		{
			grow_snake(&snake);
			fruit.x = rand() % WIDTH;
			fruit.y = rand() % HEIGHT;
		}
		SDL_Delay(200);
	}

	char score_message[64];
	snprintf(score_message, 64, "Score: %d", snake.body_length - 2);
	SDL_ShowSimpleMessageBox(0, "Game over", score_message, 0);

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(screen);
	SDL_Quit();
	return 0;
}