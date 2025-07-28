
#include "TCPConnection.hpp"

#define BACKLOG 10

class TCPServer : public TCPConnection {

private:
    int32_t establishEndpoint(int sockfd, struct addrinfo *p) override;//Wrapper to syscall bind()
    void reapDeadProcesses(struct sigaction& sa);//Makes sure to kill any forked processes
    void sigchld_handler(int s);

public:
    TCPServer(const char* serverPort, int sock_family, bool blocking = true);//Sets sockfd to our server file descriptor. Unconnected when instantiated
    ~TCPServer() override;
    void queueConns();//Calls listen and queues any incoming connections, exits on fail
    int32_t acceptConn(struct sockaddr* clientaddr);//Accepts one connection from the connection queue, returns -1 on fail, does block I/O
    int32_t handleRequest(int socketfd);//Receives msg and sends ACK 
};