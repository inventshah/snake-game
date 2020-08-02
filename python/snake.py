# Sachin
# Aug 1, 2020
# Snake in python
# Run: python3 snake.py

import curses
from random import randint, seed
import time

if __name__ == "__main__":
	seed()

	stdscr = curses.initscr()
	curses.noecho()
	curses.curs_set(0)
	curses.cbreak()
	stdscr.keypad(True)
	stdscr.nodelay(True)

	HEIGHT, WIDTH = stdscr.getmaxyx()

	snake = [[WIDTH // 2, HEIGHT // 2]]
	food = [randint(1, WIDTH - 2), randint(1, HEIGHT - 3)]
	direction = -1
	points = 0

	while True:
		[stdscr.addstr(0, x, '#') for x in range(WIDTH - 1)]
		[stdscr.addstr(HEIGHT - 2, x, '#') for x in range(WIDTH - 1)]
		[stdscr.addstr(y, 0, '#') for y in range(HEIGHT - 1)]
		[stdscr.addstr(y, WIDTH - 2, '#') for y in range(HEIGHT - 1)]

		key = stdscr.getch()

		if key == 113: break

		if key == curses.KEY_RIGHT: direction = curses.KEY_RIGHT
		elif key == curses.KEY_LEFT: direction = curses.KEY_LEFT
		elif key == curses.KEY_UP: direction = curses.KEY_UP
		elif key == curses.KEY_DOWN: direction = curses.KEY_DOWN

		# move
		tail = snake.pop()
		if len(snake) > 0: snake.insert(0, snake[0].copy())
		else: snake.insert(0, tail)

		if direction == curses.KEY_RIGHT: snake[0][0] += 1
		elif direction == curses.KEY_LEFT: snake[0][0] -= 1
		elif direction == curses.KEY_UP: snake[0][1] -= 1
		elif direction == curses.KEY_DOWN: snake[0][1] += 1

		collision = snake[0][0] <= 0 or snake[0][1] <= 0 or snake[0][0] >= WIDTH - 1 or snake[0][1] >= HEIGHT - 2
		for s in snake[1:]:
			if s == snake[0]: collision = True 

		if collision: break

		# eat
		if snake[0] == food:
			snake.append(snake[len(snake) - 1].copy())
			food = [randint(1, WIDTH - 2), randint(1, HEIGHT - 3)]
			points += 1

		[stdscr.addstr(int(y), int(x), '*') for (x, y) in snake]
		stdscr.addstr(food[1], food[0], '&')

		stdscr.refresh()
		time.sleep(0.1)
		stdscr.erase()

	curses.nocbreak()
	stdscr.keypad(False)
	curses.echo()
	curses.endwin()

	print("total points: %d"%points)
