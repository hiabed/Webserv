#include "webserv.hpp"

// split leads to a heap-buffer-overflow;

std::string readUntilSeparator(int fd, std::string& contentType, std::string& content_length, std::string &transfer_encoding)
{
    std::string buffer;
    char temp[1024];
    ssize_t readbyte;
    
    while ((readbyte = read(fd, temp, 1024)) > 0)
    {
        buffer.append(temp, readbyte);
        size_t pos = buffer.find("\r\n\r\n");
        if (pos != std::string::npos)
        {
            parse_header(buffer, contentType, content_length, transfer_encoding);
            return buffer.substr(pos + 4); // Exclude the separator and header
        }
    }
    return buffer; // In case read returns 0 bytes
}

// std::string extractBody(int fd)
// {
//     std::string buffer;
//     return "";
// }

void multiplexing()
{
    std::string to_join;
    std::string extension;
    std::string buffer;
    // int count = 0;
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
    // int flag = 0;
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
                    ssize_t sum = 0;
                    std::string contentType;
                    std::string content_length;
                    std::string transfer_encoding;
                    std::string body = readUntilSeparator(events[i].data.fd, contentType, content_length, transfer_encoding);
                    if (!body.empty() && transfer_encoding != "chunked")
                    {
                        map m = read_file_extensions("fileExtensions");
                        map::iterator it = m.find(contentType);
                        std::string extension;
                        if (it != m.end())
                            extension = it->second;
                        else
                            std::cerr << "extension not found\n";
                        std::string fileName = generateUniqueFilename();
                        std::ofstream outFile((fileName + extension).c_str());
                        if (outFile.is_open()) 
                        {
                            ssize_t readbyte = 0;
                            outFile << body;
                            sum += body.size();
                            while (sum != atoi(content_length.c_str()))
                            {
                                readbyte = read(events[i].data.fd, &body[0], 1024);
                                outFile << body; // Write body to file
                                body.resize(readbyte);
                                sum += body.size();
                            }
                            outFile.close();
                            j = 1;
                        }
                        else
                            std::cerr << "Error opening file for appending." << std::endl;
                    }
                    else if (!body.empty() && transfer_encoding == "chunked")
                    {
                        map m = read_file_extensions("fileExtensions");
                        map::iterator it = m.find(contentType);
                        std::string extension;
                        std::stringstream ss;
                        if (it != m.end())
                            extension = it->second;
                        else
                            std::cerr << "extension not found\n";
                        std::string fileName = generateUniqueFilename();
                        std::ofstream outFile((fileName + extension).c_str());
                        std::string hexaStr;
                        std::string remaining;
                        int decimal = 0;
                        if (outFile.is_open())
                        {
                            // std::cout << body << std::endl;
                            hexaStr = body.substr(0, body.find("\r\n"));
                            ss << std::hex << hexaStr;
                            ss >> decimal;
                            // std::cout << "first : " << decimal << std::endl;
                            ss.str("");
                            remaining = body.substr(body.find("\r\n") + 2);
                            outFile << remaining; // Write body excluding the hexastr to the file.
                            sum += remaining.size();
                            std::string buffer;
                            buffer.resize(1024);
                            ssize_t readbyte = 0;
                            int remain = 0;
                            while (sum < decimal)
                            {
                                readbyte = read(events[i].data.fd, &buffer[0], 1024);
                                buffer.resize(readbyte);
                                sum += readbyte;
                                if (sum > decimal)
                                {
                                    remain = sum - decimal;
                                    std::cout << "decimal: " << decimal << std::endl;
                                    std::cout << "sum: " << sum << std::endl;
                                    if (buffer.find("0\r\n\r\n") != std::string::npos)
                                    {
                                        outFile << buffer.substr(0, buffer.find("\r\n0\r\n\r\n"));
                                        std::cout << "here\n";
                                        break;
                                    }
                                    else
                                    {
                                    }
                                    // std::cout << "sum\n" << sum;
                                    continue;
                                }
                                outFile << buffer;
                                std::cout << sum << std::endl;
                            }
                            outFile.close();
                            j = 1;
                            std::cout << "done\n";
                        }
                        else
                            std::cerr << "Error opening file for appending." << std::endl;
                    }
                    else
                        std::cerr << "Error reading request." << std::endl;
                }
                if (events[i].events & EPOLLOUT && j == 1)
                {
                    // std::cout << "\n\n========== Enter here ============\n\n";
                    /*event for write to client  */
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