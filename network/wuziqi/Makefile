.PHONY:all
all:server client
server: game.c server.c 
	gcc -o $@ -g $^
client: game.c client.c
.PHONY:clean
clean:
	rm -f server client
