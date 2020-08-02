// Sachin
// Aug 1, 2020
// compile: gcc snake.c -lncurses
// run: ./a.out

#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

// Store each snake segment as a double linked list
struct segment
{
	uint32_t x;
	uint32_t y;
	struct segment *next;
	struct segment *prev;
};

typedef struct segment segment_t;

// Maintain pointers to the first and last element in the list
struct snake {
	struct segment *head;
	struct segment *tail;
};

typedef struct snake snake_t;

enum directions {UP, DOWN, LEFT, RIGHT, NONE};

// helper method to check for null pointers
void check_null(void *ptr, char *msg)
{
	if (ptr == NULL)
	{
		fprintf(stderr, "Null pointer: %s\n", msg);
		exit(1);
	}
}

// draw the border
void draw_border(int height, int width)
{
	attron(COLOR_PAIR(1));

	for (int x = 0; x < width; x++)
	{
		mvaddch(0, x, '-');
		mvaddch(height - 1, x, '-');
	}

	for (int y = 0; y < height; y++)
	{
		mvaddch(y, width - 1, '|');
		mvaddch(y, 0, '|');
	}

	mvaddch(0, 0, '+');
	mvaddch(height - 1, width - 1, '+');
	mvaddch(height - 1, 0, '+');
	mvaddch(0, width - 1, '+');
}

// Push a new element to the tail
void eat(snake_t *snake)
{
	if (snake == NULL) return;
	segment_t *tail = snake->tail;

	if (tail == NULL)
	{
		snake->tail = (segment_t *) malloc(sizeof(segment_t));
		check_null(snake->tail, "malloc failed");
		snake->tail->prev = snake->head;
		snake->tail->next = NULL;
		snake->tail->x = snake->head->x;
		snake->tail->y = snake->head->y;
		return;
	}

	segment_t *segment = (segment_t *) malloc(sizeof(segment_t));
	check_null(segment, "malloc failed");

	tail->next = segment;
	segment->prev = tail;
	segment->next = NULL;
	segment->x = tail->x;
	segment->y = tail->y;

	snake->tail = segment;
}

// Pop tail and move to head
void move_snake(snake_t *snake, enum directions dir)
{
	if (snake == NULL) return;
	uint32_t x = snake->head->x;
	uint32_t y = snake->head->y;

	if (snake->tail != NULL)
	{
		snake->head->prev = snake->tail;
		if (snake->tail->prev != NULL) snake->tail->prev->next = NULL;
		snake->tail = snake->tail->prev;

		snake->head->prev->prev = NULL;
		snake->head->prev->next = snake->head;
		snake->head = snake->head->prev;
	}

	snake->head->y = y;
	snake->head->x = x;

	switch (dir)
	{
		case DOWN:
			snake->head->y = y + 1;
			break;
		case UP:
			snake->head->y = y - 1;
			break;
		case LEFT:
			snake->head->x = x - 1;
			break;
		case RIGHT:
			snake->head->x = x + 1;
			break;
		default:
			break;
	}
}

uint32_t random_point(int max)
{
	return rand() % max + 1;
}

// draw the snake and check if there is a collision
uint8_t draw_snake(snake_t *snake, int width, int height)
{
	attron(COLOR_PAIR(2));
	if (snake->head->x == 0 || snake->head->y == 0 || snake->head->x == width - 1 || snake->head->y == height - 1)
	{
		return 1;
	}

	segment_t *node = snake->head->next;

	while (node != NULL)
	{
		if (node->x == snake->head->x && node->y == snake->head->y) return 1;
		mvaddch(node->y, node->x, '*');
		node = node->next;
	}

	mvaddch(snake->head->y, snake->head->x, '*');

	return 0;
}

void free_segment(segment_t *segment)
{
	if (segment != NULL)
	{
		free_segment(segment->next);
		free(segment);
	}
}

int main()
{
	int HEIGHT, WIDTH;
	wchar_t key;
	enum directions dir = NONE;
	uint32_t foodx, foody;
	uint32_t points = 0;

	srand(time(NULL));

	initscr();
	noecho();
	curs_set(0);
	keypad(stdscr, true);
	nodelay(stdscr, true);
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_GREEN);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

	getmaxyx(stdscr, HEIGHT, WIDTH);

	segment_t *head = (segment_t *) malloc(sizeof(segment_t));
	check_null(head, "malloc failed");

	*head = (segment_t) {
		WIDTH / 2,
		HEIGHT / 2,
		NULL,
		NULL,
	};

	snake_t snake = (snake_t) {
		head,
		NULL,
	};

	foodx = random_point(WIDTH - 2);
	foody = random_point(HEIGHT - 2);

	while((key = getch()) != 'q')
	{
		draw_border(HEIGHT, WIDTH);
		attron(COLOR_PAIR(3));
		mvaddch(foody, foodx, '&');

		switch(key)
		{
			case KEY_RIGHT:
				dir = RIGHT;
				break;
			case KEY_LEFT:
				dir = LEFT;
				break;
			case KEY_UP:
				dir = UP;
				break;
			case KEY_DOWN:
				dir = DOWN;
				break;
		}

		if (draw_snake(&snake, WIDTH, HEIGHT)) break;

		move_snake(&snake, dir);

		if (snake.head->x == foodx && snake.head->y == foody)
		{
			foodx = random_point(WIDTH - 2);
			foody = random_point(HEIGHT - 2);
			eat(&snake);
			points++;
		}
		
		refresh();
		usleep(100000);
		erase();
	}

	endwin();

	printf("total score: %d\n", points);
	free_segment(snake.head);
}