#include "webserv.hpp"

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
        {
            //9 number handled only html file; i.g for text should be 10;
            return token.substr(14, 9);
        }
    }
    return "";
}