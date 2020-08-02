// Sachin
// Aug 1, 2020
// Simpler version of snake in c. Uses
// a single linked list and no color.
// compile: gcc snake_light.c -lncurses
// run: ./a.out

#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

// Store each snake segment as a linked list
struct segment
{
	int x;
	int y;
	struct segment *next;
};
typedef struct segment segment_t;

enum directions { UP, DOWN, LEFT, RIGHT, NONE };

// Push a new element to the tail
void eat(segment_t *snake)
{
	while (snake->next != NULL) snake = snake->next;

	segment_t *tail = (segment_t *) malloc(sizeof(segment_t));
	if (tail == NULL)
	{
		fprintf(stderr, "malloc failed");
		exit(1);
	}

	*tail = *snake;
	snake->next = tail;
}

// Pop the tail and move to head, return new head
segment_t *move_snake(segment_t *snake, enum directions dir)
{
	uint32_t x = snake->x;
	uint32_t y = snake->y;
	segment_t *node = snake;
	segment_t *tail = NULL;

	if (snake->next != NULL)
	{
		while (node->next->next != NULL) node = node->next;

		tail = node->next;
		node->next = NULL;
		tail->next = snake;
		snake = tail;
	}

	snake->y = y + (dir == DOWN ? 1 : dir == UP ? -1 : 0);
	snake->x = x + (dir == RIGHT ? 1 : dir == LEFT ? -1 : 0);

	return snake;
}

// draw the snake and check if there is a collision
uint8_t draw_snake(segment_t *snake, int width, int height)
{
	if (snake->x < 0 || snake->y < 0 || snake->x >= width || snake->y >= height) return 1;

	segment_t *node = snake;

	while (node != NULL)
	{
		if (node != snake && node->x == snake->x && node->y == snake->y) return 1;
		mvaddch(node->y, node->x, '#');
		node = node->next;
	}

	return 0;
}

void free_snake(segment_t *snake)
{
	if (snake == NULL) return;
	free_snake(snake->next);
	free(snake);
}

int main()
{
	int HEIGHT, WIDTH, points = 0;
	wchar_t key;
	enum directions dir = NONE;

	srand(time(NULL));
	initscr();
	noecho();
	curs_set(0);
	keypad(stdscr, true);
	nodelay(stdscr, true);
	getmaxyx(stdscr, HEIGHT, WIDTH);

	segment_t *snake = (segment_t *) malloc(sizeof(segment_t));
	if (snake == NULL)
	{
		fprintf(stderr, "malloc failed\n");
		exit(1);
	}
	*snake = (segment_t) { WIDTH / 2, HEIGHT / 2, NULL };

	uint32_t foodx = rand() % (WIDTH - 2) + 1;
	uint32_t foody = rand() % (HEIGHT - 2) + 1;

	while((key = getch()) != 'q')
	{
		mvaddch(foody, foodx, '&');

		if (key == KEY_RIGHT) dir = RIGHT;
		else if (key == KEY_LEFT) dir = LEFT;
		else if (key == KEY_UP) dir = UP;
		else if (key == KEY_DOWN) dir = DOWN;

		snake = move_snake(snake, dir);

		if (draw_snake(snake, WIDTH, HEIGHT)) break;

		if (snake->x == foodx && snake->y == foody)
		{
			foodx = rand() % (WIDTH - 2) + 1;
			foody = rand() % (HEIGHT - 2) + 1;
			eat(snake);
			points++;
		}
		
		refresh();
		usleep(100000);
		erase();
	}

	endwin();

	printf("total score: %d\n", points);
	free_snake(snake);
}