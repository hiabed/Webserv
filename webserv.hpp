#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <cstring>
#include <cstdlib>
#include <sys/wait.h>
#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>

#define PORT 8082

typedef std::map<std::string, std::string> map;

char** my_split(char *str, char delim);

#endif