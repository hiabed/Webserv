#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sys/time.h>
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

#define PORT 8081

typedef std::map<std::string, std::string> map;

char** my_split(const char *str, char delim);
std::string generateUniqueFilename();
void print_keyVal(map m);
map read_file_extensions(const char *filename);
std::string parse_header(std::string to_join);
void PutBodyInFile(std::string to_join, std::string extension);

#endif