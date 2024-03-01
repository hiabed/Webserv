#include "webserv.hpp"

std::string parse_header(std::string to_join, std::string &content_type, std::string &content_length)
{
    std::istringstream stream (to_join);
    std::string token;
    content_type = "Content-Type";
    content_length = "Content-Length";
    while(getline(stream,token,'\n'))
    {
        std::string str_type = token.substr(0, 12);
        std::string str_length = token.substr(0, 14);
        if (str_length == content_length)
        {
            content_length = token.substr(16, 6);
            // std::cout << content_length << std::endl;
        }
        if (str_type == content_type)
        {
            //9 number handled only html file; i.g for text/jpg should be 10;
            content_type = token.substr(14, 10);
        }
    }
    return "";
}