#include "webserv.hpp"

std::string parse_header(std::string buffer, std::string &content_type, std::string &content_length, std::string &transfer_encoding)
{
    // std::cout << buffer << std::endl;
    std::istringstream stream (buffer);
    std::string token;
    content_type = "Content-Type";
    content_length = "Content-Length";
    transfer_encoding = "Transfer-Encoding";
    while(getline(stream, token, '\n'))
    {
        // Remove \r character; 
        token.erase(token.end() - 1);
        std::string str_type = token.substr(0, 12);
        std::string str_length = token.substr(0, 14);
        std::string is_chunked = token.substr(0, 17);
        
        if (str_length == content_length)
            content_length = token.substr(16);
        else if (str_type == content_type)
            content_type = token.substr(14);
        else if (is_chunked == transfer_encoding)
            transfer_encoding = token.substr(19);
    }
    return "";
}