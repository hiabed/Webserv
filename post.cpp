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

bool post_method(std::string buffer)
{
    if (buffer.find("\r\n\r\n") != std::string::npos && f == 0)
    {
        parse_header(buffer, contentType, content_length, transfer_encoding);
        open_unic_file(contentType);
        buffer = buffer.substr(buffer.find("\r\n\r\n") + 4);
    }
    if (transfer_encoding != "chunked")
        return binary(buffer);
    else
        return chunked(buffer);
    return false;
}

int cout = 0;

std::string parse_hexa(std::string remain) // parse the hexadecimal and return the remaining.
{
    ss << std::hex << remain.substr(0, remain.find("\r\n"));
    ss >> chunk_length;
    ss.str("");
    ss.clear();
    return remain.substr(remain.find("\r\n") + 2);
}

int dd = 0;

void convert(std::string& buffer)
{
    std::stringstream ss;
    if (dd != 0)
        buffer = buffer.substr(2);
    ss <<std::hex << buffer.substr(0, buffer.find("\r\n"));
    ss >> chunk_length;
    buffer = buffer.substr(buffer.find("\r\n") + 2);
    ss.str("");
}   

bool chunked(std::string buffer)
{
    if (outFile.is_open())
    {
        f = 1;
        concat += buffer;
        if (!chunk_length)
            convert(concat);
        else if (chunk_length <= concat.length() && chunk_length)
        {
            outFile << concat.substr(0, chunk_length);
            concat = concat.substr(chunk_length);
            chunk_length = 0;
            if (concat.find("\r\n\r\n") != std::string::npos)
            {
                chunked("");
                outFile.close();
                f = 0;
                return true;
            }
        }
        dd = 1;
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
        std::cerr << "Error opening file." << std::endl;
    return false;
}