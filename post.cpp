#include "webserv.hpp"

/*-- My Global variables --*/

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
size_t chunk_length = 0;
std::stringstream ss;
std::string hexa;
std::string concat;
int f = 0;

void open_unic_file(std::string contentType)
{
    map m = read_file_extensions("fileExtensions");
    map::iterator it = m.find(contentType);
    if (it != m.end())
        extension = it->second;
    else
        std::cerr << "extension not found\n";
    fileName = generateUniqueFilename();
    outFile.open((fileName + extension).c_str());
}

int flag = 0;

bool post_method(std::string buffer)
{
    if (buffer.find("\r\n\r\n") != std::string::npos && f == 0)
    {
        parse_header(buffer, contentType, content_length, transfer_encoding);
        open_unic_file(contentType);
        buffer = buffer.substr(buffer.find("\r\n\r\n") + 4);
        parse_hexa(buffer);
        f = 1;
    }
    if (transfer_encoding != "chunked")
        return binary(buffer);
    else
        return chunked(buffer);
    return false;
}

void parse_hexa(std::string &remain)
{
    ss << std::hex << remain.substr(0, remain.find("\r\n"));
    ss >> chunk_length;
    ss.str("");
    remain = remain.substr(remain.find("\r\n") + 2); // the remaining body after hexa\r\n. if after hexa is \r\n\r\n it means that "\r\n0\r\n\r\n".
}

bool chunked(std::string buffer)
{
    if (outFile.is_open())
    {
        concat += buffer;
        if (concat.length() >= (chunk_length + 9))
        {
            std::cout << chunk_length << std::endl;
            outFile << concat.substr(0, chunk_length);
            concat = concat.substr(chunk_length + 2);
            parse_hexa(concat); // here is the problem if he cant find the 
        }
        if (concat.find("\r\n0\r\n\r\n") != std::string::npos)
        {
            std::cout << "done.\n";
            outFile << concat.substr(0, chunk_length);
            outFile.close();
            f = 0;
            concat.clear();
            return true;
        }
    }
    else
        std::cerr << "Error opening file.\n";
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
            body_size = 0;
            f = 0;
            return true;
        }
    }
    else
        std::cerr << "Error opening file.\n";
    return false;
}