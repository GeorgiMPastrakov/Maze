maze: *.c maze.h
	gcc -o maze *.c

clean:
	rm -f maze
