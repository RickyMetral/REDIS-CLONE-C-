#include "TCPConnection.hpp"
#include "TCPClient.hpp"
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

TCPClient::TCPClient(const char* ipaddr, const char* clientPort, int sock_family) : TCPConnection(sock_family, AF_UNSPEC){
    this->initSocket(ipaddr, clientPort);
}

TCPClient::~TCPClient(){
    //TODO: Make sure to free whatever is needed in the future
    std::cout << "Client Destructor Called" << std::endl;
}

int TCPClient::establishEndpoint(int socketfd, struct addrinfo *p){
    return connect(socketfd, p->ai_addr, p->ai_addrlen);
}


int32_t TCPClient::recvMsg(char* buffer, size_t bufferSize){
    return recv(this->sockfd, buffer, bufferSize, 0);
}

int32_t TCPClient::sendMsg(const void* message, size_t msglen){
    return send(this->sockfd, message, msglen, 0);
}

bool TCPClient::sendAll(const void* msg, size_t msglen){
    return TCPConnection::sendAll(this->sockfd, msg, msglen);
}

bool TCPClient::recvAll(char* buffer, size_t buffersize){
    ssize_t bytes_recv = 0;
    while(buffersize > 0){
        bytes_recv = recv(this->sockfd, buffer, buffersize, 0);
        if(bytes_recv < 0){
            perror("RecvAll");
            exit(1);
        }
        buffersize -= bytes_recv;
        buffer += bytes_recv;
    }
    return buffersize != 0 ? false : true;
}


bool TCPClient::readAll(char *buffer, size_t buffersize){
    ssize_t bytes_recv = 0;
    while(buffersize > 0){
        bytes_recv = read(this->sockfd, buffer, buffersize);
        if(bytes_recv < 0){
            perror("Read All failed");
            return bytes_recv;
        }
        buffersize -= bytes_recv;
        buffer += bytes_recv;
    }
    return buffersize != 0 ? false : true;
}

bool TCPClient::writeAll(const void* message, size_t msglen){
    ssize_t total_sent = 0;
    ssize_t bytes_sent = 0;
    while(total_sent < msglen){
        bytes_sent = send(this->sockfd, message + total_sent, msglen - total_sent, 0);
        if(bytes_sent <= -1){
            perror("writeall failed");
            return bytes_sent;
        }
        total_sent += bytes_sent;
    }
    return bytes_sent == -1 ? false : true;//Return -1 on failure
}


int32_t TCPClient::queryServer(const char* msg){
    int32_t err = 0;
    int32_t len = (uint32_t)strlen(msg);
    //4 bytes for header 
    char writebuf[4 + len];
    //Put the len of the msg from the header
    memcpy(writebuf, &len, 4);
    memcpy(&writebuf[4], msg, len);
    //Write the buffer to server
    err = this->writeAll(writebuf, len+4);
    if(err < 0){
        perror("Write All");
        return err;
    }
    char readbuf[4 + MAX_MSGLEN];
    //Read the ACK message header from server
    err = this->readAll(readbuf, 4);
    if(err < 0){
        perror("queryserver->ReadAll failed");
        return err;
    }
    memcpy(&len, readbuf, 4);
    //Read the rest of the message
    err = this->readAll(&readbuf[4], len);
    if(err < 0){
        perror("querserver->ReadAll failed");
        return err;
    }
    readbuf[len+4+1] = '\0';//Null terminate buffer
    std::cout << "Received: " << &readbuf[4] << std::endl;
    return 0;
} 