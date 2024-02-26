#include "webserv.hpp"

void print_keyVal(map m)
{
    map::iterator it = m.begin();
    while (it != m.end())
    {
        std::cout << it->first << ": " << it->second << std::endl;
        it++;
    }
    std::cout << "\n";
}

std::string generateUniqueFilename()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    std::ostringstream filename_stream;
    filename_stream << "outfile_" << tv.tv_sec;

    return filename_stream.str();
}

map read_file_extensions(const char *filename)
{
    map extensions;
    std::ifstream file(filename);
    std::string line;
    char **pair;
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return extensions;
    }
    while (getline(file, line)) 
    {
        pair = my_split(line.c_str(), ':');
        extensions[pair[0]] = pair[1];
    }
    file.close();
    return extensions;
}

void PutBodyInFile(char *buffer, std::string extension)
{
    std::string header = buffer;
    std::string body;
    size_t pos = header.find("\r\n\r\n");
    if(pos != std::string::npos) // means found;
    {
        body = header.substr(pos + 4);
        std::string filename = generateUniqueFilename();
        std::ofstream outFile((filename + extension).c_str());
        if (outFile.is_open())
        {
            outFile << body;
            outFile.close();
        } 
        else
            std::cerr << "Error: Unable to open file for writing." << std::endl;
    }
    else
        std::cerr << "Error: Body not found in buffer." << std::endl;
}

std::string parse_header(char *buffer)
{
    std::istringstream stream (buffer);
    std::string token;
    while(getline(stream,token,'\n'))
    {
        std::string content_type = "Content-Type";
        std::string str;
        str = token.substr(0, 12);
        if (str == content_type)
            return token.substr(14, 10);
    }
    return "";
}

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
    epoll_event events[1024];
    while (1)
    {
        int clientSocketFD;
        int numEvent = epoll_wait(epollFD,events,1024,-1); 
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
                    char buffer[1042];
                    /* event for read from fd*/
                    memset(buffer,0,1024);
                    ssize_t readbyte;
                    readbyte = read(events[i].data.fd, buffer, 1023);
                    i = 1;

                    std::string cn_type = parse_header(buffer);
                    map m = read_file_extensions("fileExtensions");
                    map::iterator it = m.find(cn_type);
                    // optional if statement;
                    if (it == m.end())
                        std::cout << "Extension not found\n";
                    PutBodyInFile(buffer, it->second);
                    break;
                }
                if (events[i].events & EPOLLOUT && i == 1)
                {
                    /*event for write to client  */
                    std::string response = "HTTP/1.1\r\nContent-Type: text/html\r\n\r\nhello world";
                    send(events[i].data.fd,response.c_str(), response.length(), 0);
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, clientSocketFD, NULL);
                    close(events[i].data.fd);
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