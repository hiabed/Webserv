#include "webserv.hpp"

int flag = 0;
std::string fileName;
std::ofstream outFile;
std::string contentType;
std::string content_length;
std::string transfer_encoding;
std::string extension;
ssize_t sum = 0;

bool post_method(std::string buffer)
{
    if (buffer.find("\r\n\r\n") != std::string::npos)
    {
        parse_header(buffer, contentType, content_length, transfer_encoding);
        buffer = buffer.substr(buffer.find("\r\n\r\n") + 4);
    }
    if (flag == 0)
    {
        map m = read_file_extensions("fileExtensions");
        map::iterator it = m.find(contentType);
        if (it != m.end())
            extension = it->second;
        else
            std::cerr << "extension not found\n";
        fileName = generateUniqueFilename();
        outFile.open((fileName + extension).c_str());
        flag = 1;
    }
    if (outFile.is_open())
    {
        outFile << buffer;
        sum += buffer.size();
        if (sum == atoi(content_length.c_str()))
        {
            outFile.close();
            flag = 0;
            sum = 0;
            return true;
        }
    }
    else
        std::cerr << "Error opening file for appending." << std::endl;
    return false;
}