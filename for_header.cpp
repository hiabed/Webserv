#include "post.hpp"

void post::parse_header(std::string buffer, std::string &content_type, std::string &content_length, std::string &transfer_encoding)
{
    int g = 0;
    std::istringstream stream (buffer);
    std::string line;
    while (getline(stream, line))
    {
        if (line.find("\r") != std::string::npos)
            line.erase(line.find("\r"));
        if (line.substr(0, 14) == "Content-Length")
            content_length = line.substr(16);
        else if (line.substr(0, 12) == "Content-Type" && g == 0)
        {
            content_type = line.substr(14);
            g = 1;
        }
        else if (line.substr(0, 17) == "Transfer-Encoding")
            transfer_encoding = line.substr(19);
        if (line == "\r")
            return ;
    }
}