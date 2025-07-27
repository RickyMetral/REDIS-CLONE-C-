#include "TCPConnection.hpp"



class TCPClient : public TCPConnection {
private:
    int establishEndpoint(int sockfd, struct addrinfo *p) override;//Wrapper to the syscall connect()

public:
    TCPClient(const char* ipaddr, const char* clientPort, int sock_family);//Sets sockfd to our client file descriptor. 
    ~TCPClient() override;
    int32_t sendAll(const void* msg, size_t msglen);//Ensures to send all the bytes unless an error occurs. Returns -1 on fail, 0 on sucess
    int32_t recvAll(char* buffer, size_t buffersize);//Receives all the data sent. Returns -1 on fail, 0 on sucess
    int32_t sendMsg(const void* msg, size_t msglen);//Ensures to send all the bytes unless an error occurs. Returns number of bytes sent
    int32_t recvMsg(char* buffer, size_t buffersize);//Receives all the data sent. Returns number of bytes received. 
    int32_t writeAll(const void* message, size_t msglen);//Ensures to write all the bytes unless an error occurs. Returns -1 on fail, 0 on success
    int32_t readAll(char *buffer, size_t buffersize);//Receives all the data sent. Returns -1 on fail, 0 on success
    int32_t queryServer(const char* msg);//Queries the server with input message
};