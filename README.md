# Maze

A console maze program in C: generate mazes, save/load them to files, walk them
by hand, and watch the computer solve them step by step.

> School project — Технологично училище "Електронни системи" към ТУ-София,
> лятна практика, 10 клас. Three-person team.

## What it does

On launch the program shows a menu and keeps returning to it after each action:

1. **Generate maze** — asks for width and height (and optionally a seed) and
   builds a new random maze.
2. **Read from file** — loads a maze from a text file.
3. **Write to file** — saves the current maze to a text file (empty file if no
   maze has been generated yet).
4. **Player traversal** — draws the maze with a marker at the start and lets you
   move with directional input until you reach the goal.
5. **Computer traversal** — the computer finds a path from start to goal and
   animates every step it takes (not just the final path).
6. **Exit** — quits immediately.

## The maze model

- A maze is a 2D grid of cells. Each cell is either a **wall** (`#`) or **empty**
  (a space) — you cannot pass through walls.
- Sizes range from **3×3** to **40×30** (width × height).
- The **start** is the top-left corner. The **exit** is chosen at generation
  time — any cell — and defaults to the bottom-right corner.
- Every generated maze is guaranteed to have at least one path from start to
  exit.

### Generation

Mazes are built with a **recursive backtracker** (randomised depth-first search)
that carves passages out of a solid grid of walls, producing a perfect maze
(exactly one path between any two cells). A start→exit path is always guaranteed.

**Chosen exit.** When generating, you give a target row and column for the exit;
it snaps to the nearest room cell and is stored with the maze (and in its file),
so the player and the solvers all use that exit.

**Extra paths.** A perfect maze has a single solution. You can ask for a number
of extra paths: generation then opens exactly that many additional walls between
existing corridors, each one adding an independent alternative route. Request 0
for a single-solution maze, or more to make it a looping, multi-route maze. The
count is capped by how many walls are available to open.

A maze can be generated from a **seed** — an integer that fully determines the
result. The same seed, size, exit, and path count always produce the same maze,
on any machine, because generation uses its own deterministic random number
generator rather than the C library's `rand()`.

### Drawing

A helper renders any maze to the console using `#` for walls and spaces for empty
cells. The same character layout is what gets written to and read from files.

### File format

A plain-text file: a header line `width height exit_row exit_col`, followed by
`height` rows of `width` characters each.

```
10 6 4 8
##########
#        #
# ## ### #
# #    # #
#   ##   #
##########
```

### Computer solving

The computer offers four maze-solving algorithms and animates each one step by
step, so you can watch how it explores:

- **Depth-first search** — dives deep along one direction, backtracking at
  dead ends.
- **Breadth-first search** — expands evenly in all directions and finds a
  shortest path.
- **A\*** — guided search using a Manhattan-distance heuristic; finds a shortest
  path while exploring fewer cells than BFS.
- **Right-hand wall follower** — keeps its right hand on the wall and walks until
  it reaches the goal.

During the animation the start is `S`, the goal is `E`, the current cell is `@`,
explored cells fade out, the frontier is `+`, and the final path is drawn with
`o`. A comparison mode runs all four and prints how many cells each explored and
how long its path was.

## Build & run

Requires a C99 compiler (`gcc` or `clang`) and `make`. Targets macOS / Linux.

```bash
make
./maze
```

## Team & layout

| Module                         | File(s)            |
|--------------------------------|--------------------|
| Shared types + drawing         | `maze.h`, `maze.c` |
| Generation (seeded)            | `generate.c`       |
| File read/write                | `fileio.c`         |
| Player traversal               | `traverse.c`       |
| Computer solving               | `solve.c`          |
| Menu / entry point             | `main.c`           |
