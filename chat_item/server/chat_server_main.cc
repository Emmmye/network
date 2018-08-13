#include <iostream>
#include "chat_server.h"
#include "../common/util.hpp"
int main(int argc, char* argv[])
{
    if(argc != 3){
        LOG(ERROR) << "Usage: ./chat_server [ip] [port]\n";
    }

    server::ChatServer server;
    server.Start(argv[1],htons(atoi(argv[2])));
    return 0;
}
