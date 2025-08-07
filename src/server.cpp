#include "TCPServer.hpp"
#include "Epoll.hpp"

void epoll_callback(const struct epoll_event& event, Epoll* epoll, TCPServer* conn){
    // New connection on server socket to accept
    if(event.data.fd == conn->getSock()) {
        sockaddr_storage their_addr;
        char s[INET6_ADDRSTRLEN];
        
        int new_fd = conn->acceptConn((struct sockaddr*)&their_addr);
        if(new_fd == -1) {
            perror("accept");
            return;
        }
        
        inet_ntop(their_addr.ss_family,
                    conn->get_in_addrs((struct sockaddr*)&their_addr),
                    s, sizeof s);
        printf("conn: got connection from %s\n", s);
        
        // Add new client to epoll
        struct epoll_event client_event;
        client_event.data.fd = new_fd;
        client_event.events = EPOLLIN | EPOLLHUP | EPOLLOUT;

        if(!epoll->addfd(new_fd, client_event)){
            std::cerr << "Could not add connection " << s << "to Epoll" << std::endl;
            exit(1);
        }
        std::cout << "Added Connection " << s << " to Epoll" << std::endl;
        

    } else {
        // Data from existing client
        if(event.events & EPOLLIN && event.events & EPOLLOUT) {
            std::cout << "Handling Request From: " << event.data.fd << std::endl;
            conn->handleRequest(event.data.fd);
        }

    }
}

int main(){
    vector<struct epoll_event> events;
    struct epoll_event server_event;

    TCPServer server("3490", AF_INET);

    server_event.data.fd = server.getSock();
    server_event.events = EPOLLIN;
    events.push_back(server_event);

    Epoll epoll(events);

    while(1){
        epoll.mainloop(epoll_callback, &server);
    }
    return 0;
}