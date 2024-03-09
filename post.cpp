#include "webserv.hpp"

/*-- My Global variables --*/

//for opening the file;
int flag = 0;
std::string fileName;
std::ofstream outFile;
std::string extension;

// for parsing header;
std::string contentType;
std::string content_length;
std::string transfer_encoding;

// for binary;
ssize_t body_size = 0;

//for chunked;
std::stringstream ss;
int chunk_length;
std::string hexa;
std::string remain;
int f = 0;

void open_unic_file()
{
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
}

bool post_method(std::string buffer)
{
    if (buffer.find("\r\n\r\n") != std::string::npos && buffer.find("\r\n0\r\n\r\n") == std::string::npos)
    {
        parse_header(buffer, contentType, content_length, transfer_encoding);
        open_unic_file();
        buffer = buffer.substr(buffer.find("\r\n\r\n") + 4);
        f = 0;
    }
    if (transfer_encoding != "chunked")
        return binary(buffer);
    else
        return chunked(buffer);
    return false;
}

std::string keep_the_remaining(std::string buffer)
{
    size_t pos = buffer.find("\r\n", buffer.find("\r\n") + 2);
    outFile << buffer.substr(0, buffer.find("\r\n"));
    // current chunk done;
    hexa = buffer.substr(buffer.find("\r\n") + 2, pos);
    ss << std::hex << hexa;
    ss >> chunk_length;
    return buffer.substr(pos + 2);
}

bool chunked(std::string buffer)
{
    if (outFile.is_open())
    {
        if (f == 0)
        {
            hexa = buffer.substr(0, buffer.find("\r\n"));
            ss << std::hex << hexa;
            ss >> chunk_length;
            ss.str("");
            buffer = buffer.substr(buffer.find("\r\n") + 2);
            f = 1;
        }
        body_size += buffer.size();
        if (body_size >= chunk_length)
        {
            buffer = keep_the_remaining(buffer);
            body_size = buffer.size();
        }
        if (chunk_length == 0 || buffer.find("\r\n0\r\n\r\n") != std::string::npos)
        {
            std::cout << "here\n";
            outFile << buffer.substr(0, buffer.find("\r\n0\r\n\r\n"));
            outFile.close();
            return true;
        }
        outFile << buffer;
    }
    else
        std::cerr << "Error opening file for appending." << std::endl;
    return false;
}

bool binary(std::string buffer)
{
    if (outFile.is_open())
    {
        outFile << buffer;
        body_size += buffer.size();
        if (body_size == atoi(content_length.c_str()))
        {
            outFile.close();
            flag = 0;
            body_size = 0;
            return true;
        }
    }
    else
        std::cerr << "Error opening file for appending." << std::endl;
    return false;
}