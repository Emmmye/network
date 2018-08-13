.PHONY:all
all:chat_client_main 
chat_client_main:chat_client_main.cc chat_client.cc window.cc
	g++ -o $@ -g $^ -std=c++11 -lpthread -lboost_filesystem -lboost_system -lncursesw -ljosn
# test_win:window.cc
# 	g++ -o $@ -g $^ -std=c++11 -lpthread -lncursesw
# chat_client:chat_client.cc 
# 	g++ -o $@ -g $^ -std=c++11 -lpthread -lboost_filesystem -lboost_system -ljosn
.PHONY:clean
clean:
	rm  chat_client_main 
