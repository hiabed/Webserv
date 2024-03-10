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

// for debugging;
int count = 0;

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

std::string remove_hexa(std::string remain)
{
    hexa = remain.substr(0, remain.find("\r\n"));
    ss << std::hex << hexa;
    ss >> chunk_length;
    ss.str("");
    ss.clear();
    std::cout << hexa << std::endl;
    std::cout << chunk_length << std::endl;
    usleep(100000); // just for debugging;
    return remain.substr(remain.find("\r\n") + 2);
}

bool chunked(std::string buffer)
{
    if (outFile.is_open())
    {
        if (f == 0)
        {
            concat = remove_hexa(buffer);
            f = 1;
            return false;
        }
        concat += buffer; //add the buffer to the reamin (remaining);
        if (concat.find("\r\n", concat.find("\r\n") + 2) != std::string::npos && concat.size() > chunk_length)
        {
            outFile << concat.substr(0, concat.find("\r\n"));
            if (concat.find("\r\n0\r\n\r\n") != std::string::npos)
            {
                std::cout << "done.\n";
                outFile.close();
                f = 0;
                return true;
            }
            concat = remove_hexa(concat.substr(concat.find("\r\n") + 2));
            // keep only the body part without hexa\r\n;
        }
    }
    else
        std::cerr << "Error opening file for appending.\n";
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
        std::cerr << "Error opening file for appending." << std::endl;
    return false;
}