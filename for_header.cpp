#include "webserv.hpp"

std::string parse_header(std::string to_join, std::string &content_type, std::string &content_length)
{
    std::istringstream stream (to_join);
    std::string token;
    content_type = "Content-Type";
    content_length = "Content-Length";
    while(getline(stream, token, '\n'))
    {
        // Remove any trailing newline characters
        token.erase(token.end() - 1); // \r;
        std::string str_type = token.substr(0, 12);
        std::string str_length = token.substr(0, 14);
        if (str_length == content_length)
            content_length = token.substr(16); // Exclude "Content-Length: "
        else if (str_type == content_type)
            content_type = token.substr(14); // Exclude "Content-Type: "
    }
    return "";
}