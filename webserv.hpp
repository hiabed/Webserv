#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <string>
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

std::string generateUniqueFilename();
void print_keyVal(map m);
map read_file_extensions(const char *filename);
void parse_header(std::string buffer, std::string &content_type, std::string &content_length, std::string &transfer_encoding);
void PutBodyInFile(std::string buffer, std::string extension);
bool post_method(std::string buffer);
std::string readUntilSeparator(std::string buffer);
bool binary(std::string buffer);
bool chunked(std::string buffer);
void open_unic_file(std::string contentType);
std::string switch_to_hexa(std::string remain);

#endif