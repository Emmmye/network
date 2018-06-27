.PHONY:all
#all:tcpServer tcpClient
#all:server client
#server:server.c
#client:client.c
#tcpServer:tcpServer.c
#	gcc -o $@ -g $^ -lpthread
#tcpClient:tcpClient.c
#all:tcpClient epoll_server
#tcpClient:tcpClient.c
#poll_server:epoll_server.c
#	gcc -o $@ -g $^
#.PHONY:clean
#clean:
#	rm -f client server
#	rm -f tcpClient tcpServer
#	rm -f epoll_server tcpClient
all:client server
client:client.c
	gcc -o $@ -g @^
server:server.c
	gcc -o $@ -g @^
.PHONY:clean
clean:
	rm -f client server
