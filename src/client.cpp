#include "TCPClient.hpp"

#define PORT "3490"

int main(int argc, char** argv){
    char buf[1024];
    TCPClient client(argv[1], PORT, AF_INET);
    if(client.queryServer("Hello") != 0){
        perror("queryServer Failed");
        exit(1);
    }
    // if(client.writeAll("Poopy", 5) != 0){
    //     std::cout << "write all failed" << std::endl;
    //     exit(1);
    // }
    // if(client.readAll(buf, 13) != 0){
    //     std::cout << "read all failed" << std::endl;
    //     exit(1);
    // }
    // buf[14] = '\0';
    // std::cout << "Received Message: " << buf << std::endl;
    return 0;
}