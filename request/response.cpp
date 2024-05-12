#include "../headers/request.hpp"
#include "../headers/Client.hpp"

extern std::map<int, Client *> fd_maps;

response::response()
{
    response_message = message_response_stat();
    fill_extentions();
}

response::~response(){
}

void        response::fill_extentions()
{   
    extention["html"] = "text/html; charset=UTF-8"; 
    extention["txt"]  = "text/plain; charset=UTF-8"; 
    extention["jpg"] = "image/jpg"; 
    extention["jpeg"] = "image/jpeg";
    extention["png"] = "image/png";
    extention["mp3"] = "audio/mpeg";
    extention["mp4"] = "video/mp4";
    extention["webm"] = "video/webm";
    extention["pdf"] = "application/pdf";
    extention["zip"] = "application/zip";
    extention["woff"] = "application/font-woff";
    extention["js"] = "application/javascript";
    extention["css"] = "text/css";
    extention["xml"] = "text/xml";
    extention["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    extention["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    extention["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    extention["svg"] = "image/svg+xml";
    extention["json"] = "application/json";
    extention["ico"] = "image/x-icon";
    extention["gif"] = "image/gif";
    extention["mpg"] = "video/mpeg";
    extention["avi"] = "video/x-msvideo";
    extention["mov"] = "video/quicktime";
    extention["m3u8"] = "application/vnd.apple.mpegurl";
    extention["wasm"] = "application/wasm";
    extention["mpd"] = "application/dash+xml";
    extention["db"] = "application/x-sqlite3";
    extention["md"] = "text/markdown";
    extention["py"] =  "text/html; charset=UTF-8";
}

std::string     response::get_exten_type(std::string path)
{
    std::string exten;
    size_t      pos = path.find_last_of(".");
    if (pos != std::string::npos)
        exten = path.substr(pos + 1);
    if (pos == std::string::npos)
        return ("application/octet-stream");        
    std::map<std::string, std::string>::iterator b = extention.find(exten);
    if (b != extention.end())
        return ((*b).second);
    if ((b == extention.end() ))
        return ("text/html; charset=UTF-8");
    return "10";
}

int     response::response_error(std::string stat, int fd)
{
        std::string response;
        std::stringstream size;
        int                stat_;
        std::map<std::string, std::string>::iterator it_ = fd_maps[fd]->err_page.find(stat);

        if( it_ != fd_maps[fd]->err_page.end())
        {
            std::string ext = get_exten_type(it_->second.c_str());
            std::ifstream    err_file;
            std::stringstream sstr;
            err_file.open(it_->second.c_str());
            sstr << err_file.rdbuf();
            response = sstr.str();
            size << response.size();
            response = get_header(stat, ext, size.str(), *fd_maps[fd]);
            response += sstr.str();
            usleep(200000);
            stat_ = send(fd, response.c_str(), response.size(), 0);
            if (stat_ == -1 || stat_ == 0)
                return 1;
            fd_maps[fd]->rd_done = 1;
            return (1);
        }
        else
        {          
            std::map<std::string, std::string>::iterator it_message_error = response_message.find(stat);
            if (it_message_error == response_message.end())
            {
                return (0);
            }
            std::string _respond_stat;
            _respond_stat = "<h1>" + it_message_error->second + "</h1>";
            _respond_stat += "<html><head><title> " + it_message_error->second + "</title></head>";
            _respond_stat +=  "<body> <strong>" + it_message_error->second + " </strong></body></html>";
            size << _respond_stat.size();
            response = get_header(stat, "text/html", size.str(), *fd_maps[fd]);
            response += _respond_stat;
            usleep(200000);
            stat_ = send(fd, response.c_str(), response.size(), 0);
            if (stat_ == -1 || stat_ == 0)
                return 1;
            fd_maps[fd]->rd_done = 1;
            return (1);
        }
        return (0);
}

std::map<std::string, std::string>        response::message_response_stat()
{
    response_message["200"] = "OK";
    response_message["201"] = "Created";
    response_message["202"] = "Accepted";
    response_message["204"] = "No Content";
    response_message["301"] = "Moved Permanently";
    response_message["302"] = "Found";
    response_message["304"] = "Not Modified";
    response_message["400"] = "Bad Request";
    response_message["401"] = "Unauthorized";
    response_message["403"] = "Forbidden";
    response_message["404"] = "Not Found";
    response_message["405"] = "Method Not Allowed";
    response_message["409"] = "Conflict";
    response_message["413"] = "Request Entity Too Large";
    response_message["415"] = "Unsupported Media Type";
    response_message["500"] = "Internal Server Error";
    response_message["408"] = "Request Timeout";
    response_message["505"] = "Version Not Supported";
    response_message["501"] = "Not Implemented";
    response_message["502"] = "Bad Gateway";
    response_message["503"] = "Service Unavailable";
    response_message["504"] = "Gateway Timeout";
    return (response_message);
}

std::string      response::get_header(std::string wich, std::string exten, std::string lentg, Client& fd_inf)
{
    std::string response;
    std::map<std::string , std::string>::iterator it = response_message.find(wich);
    int a = std::atoi(wich.c_str());
    if (it != response_message.end())
    {
        if (a != 301 && (a >= 200 && a < 599))
        {
            response = "HTTP/1.1 ";
            response +=  it->first + " " + it->second + "\r\n";
            response += "Content-Type: " + exten + "\r\n" + "Content-Length: " + lentg + "\r\n\r\n";
            fd_inf.res_header = 1;
            return (response);
        }
        else if (a == 301)
        {
            if (fd_inf.requst.redirection_stat)
            {
                std::string     path_with_slash = fd_inf.redirec_path;
                response = "HTTP/1.1 301 Moved Permanently\r\n";
                response += "Location: " + path_with_slash + "\r\n\r\n";
                fd_inf.res_header = 1;
                return (response);
            }
            else
            {
                std::string     path_with_slash = fd_inf.requst.path + "/";
                response = "HTTP/1.1 301 Moved Permanently\r\n";
                response += "Location: " + path_with_slash + "\r\n\r\n";
                fd_inf.res_header = 1;
                return (response);
            }
        }
    }
    return "";
}