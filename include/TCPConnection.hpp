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

#define MAX_MSGLEN 4096
//TODO maybe add a delete function

class TCPConnection{
protected:
    int sockfd;
    addrinfo hints;

protected:
    void die(const char* msg);//Aborts the program and prints the errno messge
    int initSocket(const char* ipaddr, const char* port);//Returns a file descriptor for a new socket
    virtual int establishEndpoint(int sockfd, struct addrinfo *p) = 0;//Virtual Function used when initializing socket (Server binds/ Client connects)
    static void sigChildHandler(int s);//Needs to be static for callback
    virtual ~TCPConnection();
    TCPConnection(int sock_family, int flags);//Creates an unconnected TCP server. Call queueconnections + accept to create an initial connection

public:
    int getSock() const;
    void* get_in_addrs(struct sockaddr *sa);//Returns unknown sockaddr (IPV4 or IPV6)
    int32_t sendMsg(int socketfd, const void* message, size_t msglen);//Ensures to send all the bytes unless an error occurs.
    int32_t recvMsg(int socketfd, char* buffer, size_t buffersize);//Receives all the data sent. Returns number of bytes received. 
    int32_t sendAll(int socketfd, const void* message, size_t msglen);//Ensures to send all the bytes unless an error occurs. Returns -1 on fail, 0 on success
    int32_t recvAll(int socketfd, char* buffer, size_t buffersize);//Receives data until buffersize num bytes is read. Returns -1 on fail, 0 on success
    int32_t writeAll(int socketfd, const void* message, size_t msglen);//Writes data until buffersize msglen num bytes is sent. Returns -1 on fail, 0 on success
    int32_t readAll(int socketfd, char* buffer, size_t buffersize);//Receives data until buffersize num bytes is read. Returns -1 on fail, 0 on success
};

#endif
