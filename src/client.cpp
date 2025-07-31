#include "TCPClient.hpp"
#include <chrono>
#include <thread>

#define PORT "3490"

int main(int argc, char** argv){
    TCPClient client(argv[1], PORT, AF_INET);
    if(client.queryServer("Hello") != 0){
        perror("queryServer Failed");
        exit(1);
    }
    return 0;
}