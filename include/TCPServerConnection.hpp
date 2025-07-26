#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h> 
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h> 
#include <errno.h>

#ifndef TCPSERVERCONNECTION_H
#define TCPSERVERCONNECTION_H

#define BACKLOG 10

class TCPServerConnection{
private:
    int socketfd;
    addrinfo hints;
    sockaddr_storage clientaddr, serveraddr;
    int socktype;
    socklen_t s_socksize;
    char buf[1024];
private:
    void die(const char* msg);
    int createSocket(const char* port);
    void bindSocket();
    void connectSocket();
    void reapDeadProcesses(struct sigaction& sa);
    static void sigChildHandler(int s);//Needs to be static for callback

public:
    TCPServerConnection(std::string clientAddr, const char* serverPort, const char* clientPort, int sock_family);
    ~TCPServerConnection();
    int sendAll(const void* message, int msglen, int flags);//Sends all the data passed in(Send might not send all the bytes)
    int receiveAll(char* buffer, int bufferSize = 1024, int flags = 0);//Receives all the data sent
    void queueConnections(const int& sockfd);//Calls listen and queues any incoming connections
    int accept(int sockfd, struct sockaddr* clientaddr);//Accepts one connection from the connection queue
};

#endif
