#include "TCPServer.hpp"
#include "Epoll.hpp"

void epoll_callback(const struct epoll_event& event, TCPServer* conn){
    conn->handleRequest(event.data.fd);
}

int main(){
    int new_fd;
    sockaddr_storage their_addr;
    char s[INET6_ADDRSTRLEN];
    vector<struct epoll_event> events;

    TCPServer test("3490", AF_INET);
    // test.queueConns();

    while(1) {  // main accept() loop
        struct epoll_event event;
        new_fd = test.acceptConn( (struct sockaddr *)&their_addr);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            test.get_in_addrs((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        event.data.fd = new_fd;
        event.events = EPOLLIN | EPOLLOUT;

        events.push_back(event);

        inet_ntop(their_addr.ss_family,
            test.get_in_addrs((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        Epoll epoll(events);
        epoll.mainloop(epoll_callback, &test);
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}