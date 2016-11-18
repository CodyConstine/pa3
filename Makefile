all: dfc dfs

client: dfc.c
	gcc -Wall -o dfc dfc.c -lpthread

server: dfs.c
	gcc -Wall -o dfs dfs.c -lpthread
clean:
	rm dfs dfc
