#include "webserv.hpp"

std::string parse_header(std::string to_join)
{
    std::istringstream stream (to_join);
    std::string token;
    while(getline(stream,token,'\n'))
    {
        std::string content_type = "Content-Type";
        std::string str;
        str = token.substr(0, 12);
        if (str == content_type)
        {
            // std::cout << "content-type: " << token.substr(14, 10) << std::endl;
            //9 number handled only html file; i.g for text/jpg should be 10;
            return token.substr(14, 10);
        }
    }
    return "";
}