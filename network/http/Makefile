all:simple_http tcpClient
simple_http:simple_http.c
	gcc -o $@ -g $^
tcpClient:tcpClient.c
	gcc -o $@ -g $^
.PHONY:clean
clean:
	rm -f simple_http tcpClient
