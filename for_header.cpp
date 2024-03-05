#include "webserv.hpp"

std::string parse_header(std::string buffer, std::string &content_type, std::string &content_length, std::string &transfer_encoding)
{
    // std::cout << buffer << std::endl;
    std::istringstream stream (buffer);
    std::string line;
    content_type = "Content-Type";
    content_length = "Content-Length";
    transfer_encoding = "Transfer-Encoding";
    while(getline(stream, line))
    {
        line.erase(line.end() - 1);
        std::string str_type = line.substr(0, 12);
        std::string str_length = line.substr(0, 14);
        std::string is_chunked = line.substr(0, 17);

        if (str_length == content_length)
            content_length = line.substr(16);
        else if (str_type == content_type)
            content_type = line.substr(14);
        else if (is_chunked == transfer_encoding)
            transfer_encoding = line.substr(19);
    }
    return "";
}