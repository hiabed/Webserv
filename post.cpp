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
std::stringstream ss;
int chunk_length;
std::string hexa;
std::string remain;
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

std::string switch_to_hexa(std::string remain)
{
    hexa = remain.substr(0, remain.find("\r\n"));
    ss << std::hex << hexa;
    ss >> chunk_length;
    ss.str("");
    if (hexa == "10000")
        count++;
    std::cout << hexa << std::endl;
    std::cout << count << std::endl;
    usleep(500000);
    return remain.substr(remain.find("\r\n") + 2); // return only the remaining part of the body without hexa\r\n;
}

bool chunked(std::string buffer)
{
    if (outFile.is_open())
    {
        if (f == 0)
        {
            buffer = switch_to_hexa(buffer);
            f = 1;
        }
        remain += buffer; //add the buffer to the reamin (remaining);
        if (remain.find("\r\n") != std::string::npos && remain.find("\r\n", remain.find("\r\n") + 2) != std::string::npos)
        {
            outFile << remain.substr(0, remain.find("\r\n"));
            if (remain.find("\r\n0\r\n\r\n") != std::string::npos)
            {
                std::cout << "done.\n";
                outFile.close();
                f = 0;
                return true;
            }
            remain = switch_to_hexa(remain.substr(remain.find("\r\n") + 2));
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