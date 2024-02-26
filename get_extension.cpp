#include "webserv.hpp"

map read_file_extensions(const char *filename)
{
    map extensions;
    std::ifstream file(filename);
    std::string line;
    char **pair;
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return extensions;
    }
    while (getline(file, line)) 
    {
        pair = my_split(line.c_str(), ':');
        extensions[pair[0]] = pair[1];
    }
    file.close();
    return extensions;
}