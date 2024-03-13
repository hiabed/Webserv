#include "webserv.hpp"

void multiplexing()
{
    int serverSocketFD ;
    int epollFD = epoll_create(5);
    epoll_event event;
    int socketFD ;
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1)
        {std::cerr << "Error creating socket\n" << std::endl;exit(1);}
  
    sockaddr_in serverAdress;
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_addr.s_addr = INADDR_ANY;
    serverAdress.sin_port = htons(PORT);
    int reuse = 1;

    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
    if(bind(socketFD,(struct sockaddr*)&serverAdress,sizeof(serverAdress)) != 0)
        std::cerr<<"Cannot bind to port : "<<PORT << "\n";
    if(listen(socketFD,10) == 0)
        std::cout << "listenning to " << PORT << " [...]" << std::endl;
    else
    {
        std::cerr<<"Error listen\n";
        exit(1);
    }
    event.events = EPOLLIN ;
    event.data.fd = socketFD;
    if(epoll_ctl(epollFD,EPOLL_CTL_ADD,socketFD,&event) == -1)
        exit(1);
    int j = 0;
    epoll_event events[1024];
    std::string buffer;
    ssize_t readbyte = 0;
    while (1)
    {
        int clientSocketFD;
        int numEvent = epoll_wait(epollFD,events, 1024, -1);
        for (int i = 0; i < numEvent; ++i)
        {
            if( events[i].data.fd <= 4)
            {
                clientSocketFD = accept(events[i].data.fd,NULL,NULL);
                // std::cout<<"New connections Id : "<< clientSocketFD<<std::endl;
                if(clientSocketFD == -1)
                {
                    std::cerr << "Failed to accept connection ." << std::endl;
                    break;
                }
                event.events = EPOLLIN | EPOLLOUT;
                event.data.fd = clientSocketFD;
                epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSocketFD, &event);
            } 
            else
            {
                if (events[i].events & EPOLLIN)
                {
                    /* event for read from fd*/
                    buffer.resize(1024);
                    readbyte = recv(events[i].data.fd, &buffer[0], 1024, 0);
                    if (readbyte == -1)
                    {
                        std::cerr << "read function failed to read from the socket file descriptor\n";
                        exit(EXIT_FAILURE);
                    }
                    buffer.resize(readbyte);
                    if (post_method(buffer))
                        j = 1;
                }
                if (events[i].events & EPOLLOUT && j == 1)
                {
                    // std::cout << "\n\n========== Enter here ============\n\n";
                    /*event to write for client  */
                    std::string response = "HTTP/1.1 201 OK\r\nContent-Type: text/html\r\n\r\nhello";
                    if (send(events[i].data.fd,response.c_str(), response.length(), 0) == - 1)
                        std::cout << "=====here=====\n";
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, clientSocketFD, NULL);
                    close(events[i].data.fd);
                    j = 0;
                }
            }
        }
    }
    close(epollFD);
    close(serverSocketFD);
}

int main()
{
    multiplexing();
}

// chunked;
// \r\n16
// asgflakflaskfha;sf
// \r\n200
// welkfhvsdlkjvhsd;dsgjksdh
// \r\n\r\n0