#include"webserv.hpp"

void serveFIle(Data& datacleint, int cfd);

std::string    getContentType(Method &method)
{
    std::map<std::string, std::string> contentTypeMap;
    contentTypeMap[".html"] = "text/html";
    contentTypeMap[".htm"] = "text/html";
    contentTypeMap[".txt"] = "text/plain";
    contentTypeMap[".jpg"] = "image/jpeg";
    contentTypeMap[".jpeg"] = "image/jpeg";
    contentTypeMap[".png"] = "image/png";
    contentTypeMap[".gif"] = "image/gif";
    contentTypeMap[".mp4"] = "video/mp4";
    contentTypeMap[".php"] = ".php";
    contentTypeMap[".py"] = ".py";
    contentTypeMap[".css"] = "text/css";
    contentTypeMap[".js"] = "application/javascript";

    size_t pos = method.path.find_last_of(".");
    if(pos != std::string::npos)
    {
        std::string extension = method.path.substr(pos);
        std::map<std::string,std::string>::iterator it = contentTypeMap.find(extension);
        if(it != contentTypeMap.end())
            return it->second;
    }
    return "application/json";
}

void    openFileAndSendHeader(Data& datacleint,Method &method, int cfd);


int    listingDirectory(Data data,Method &method,int cfd)
{
    (void)data;
    (void)cfd;
    std::ostringstream list;
    list << "<html><head><title>Directory Listing</title></head><body>";
    list << "<h1>Index of: " << method.path << "</h1>";
    list << "<table>";
    std::string directoryPath = method.rootLocation + method.path + "/";

    DIR *dir =  opendir(directoryPath.c_str());
    struct dirent *it;
    if(dir)
    {
        while((it = readdir(dir)) != NULL)
        {   
            std::string directoryChildPath = directoryPath  + it->d_name;
            struct stat statInfo;
            if(strcmp(it->d_name , ".") == 0 || strcmp(it->d_name , "..") == 0)
                continue;
            if(strcmp(it->d_name,method.autoFile.c_str()) == 0)
            {
                method.path = method.path + method.autoFile;
                data.modeAutoIndex = true;
                return (1);
            }
            else if (stat(directoryChildPath .c_str(), &statInfo) == 0)
            { 
                list << "<tr>";
                if (S_ISREG(statInfo.st_mode))
                    list << "<td>"<< "<a href='" << it->d_name << "'>" << it->d_name << "</a></td>";
                if (S_ISDIR(statInfo.st_mode))
                    list << "<td>"<< "<a href='" << it->d_name << "/" << "'>" << it->d_name << "</a></td>";
                list << "<td>"<< ctime(&statInfo.st_mtime) <<"</td>";
                list << "<td>"<< statInfo.st_size << " bytes</td>";
                list << "</tr>";
            }
        }
        closedir(dir);
    }
    list << "</table></body></html>";
    method.buff =  list.str();
    return(0);
}

ServerConfig    getServer(std::vector<std::pair<std::string,ServerConfig> >& Servers,std::string &requestHost)
{
    std::vector<std::pair<std::string,ServerConfig> >::iterator it;
    for(it = Servers.begin();it != Servers.end();it++)
    {
        if(requestHost == it->first)
           return( it->second);
    }
    it--;
    return(it->second);
}

void sendChunk(int clientSocket, const char* data, ssize_t length,Data& datacleint)
{
    std::string totalChuncked ;
    std::stringstream chunkHeader;
    chunkHeader << std::hex << length << "\r\n";
    std::string chunkHeaderStr = chunkHeader.str();
    std::string chunkData(data,length);

    totalChuncked = chunkHeaderStr + chunkData + "\r\n";
    if (send(clientSocket, totalChuncked.c_str(), totalChuncked.size(),0) ==  -1)
    {
        close(datacleint.fd);
        datacleint.readyForClose = 1;
        throw std::runtime_error("An error aka client disconnect");
    }
}

void    openFileAndSendHeader(Data& datacleint,Method &method, int cfd)
{
    char buffer[BUFFER_SIZE];
    std::string contentType = getContentType(method);
    method.path = method.rootLocation + method.path;
    if(contentType == ".php" || contentType == ".py")
    {
        std::string type;
        if(contentType == ".php")
            type = "php";
        else
            type = "py";
        fastCGI(method.path,type);
        method.path = "/tmp/tmpFile";
        datacleint.isCgi = true;
    }
    memset(buffer,0,sizeof(buffer));
    if(checkPermission(method.path.c_str(),cfd,method.version,R_OK,datacleint.readyForClose) == true)
        return;
    datacleint.isReading = true;
    datacleint.fd = open(method.path.c_str(), O_RDONLY);
    if (datacleint.fd == -1)
    {
        close(datacleint.fd);
        throw std::runtime_error("internal server error");
    }
    std::string httpResponse = method.version + " 200 OK\r\nContent-Type:" +contentType+ " \r\nTransfer-Encoding: chunked\r\n\r\n";
    if(send(cfd, httpResponse.c_str(), httpResponse.size(),0) == -1)
    {
        close(datacleint.fd);
        datacleint.readyForClose = true;
        throw std::runtime_error("An errror aka client disconnect"); 
    } 
}

void serveFIle(Data& datacleint, int cfd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer,0,sizeof(buffer));
    ssize_t byteRead = read (datacleint.fd,buffer,BUFFER_SIZE);
    if(byteRead == -1)
    {
        close(datacleint.fd);
        datacleint.readyForClose = true;
        throw std::runtime_error("EROOR reading from file");
    }
    if(byteRead == 0)
    {
        close(datacleint.fd);
        datacleint.readyForClose = true;
        if(send(cfd, "0\r\n\r\n", sizeof("0\r\n\r\n") - 1,0) == -1)
           throw std::runtime_error("An error aka client disconnect");
        unlink("/tmp/tmpFile");
        return ;
    }
    std::string httpresponse(buffer,byteRead);
    if(datacleint.isCgi == true)
    {
        size_t pos = httpresponse.find("\r\n\r\n");
        if(pos != std::string::npos)
        {
            httpresponse = httpresponse.substr(pos);
        }
        datacleint.isCgi = false;
    }
    sendChunk(cfd,httpresponse.c_str(),httpresponse.size(),datacleint);  
}

int checkFileOrDirectoryPermission(Method &method,int fd,bool &radyForClose)
{
    std::string fullPath = method.rootLocation + method.path;
    if(access(fullPath.c_str(),F_OK) != 0)
        return 0;
    if(checkPermission(fullPath.c_str(),fd,method.version,R_OK,radyForClose) == true)
        return 4;
    struct stat file;
    stat(fullPath.c_str(), &file);
    if (S_ISREG(file.st_mode))
        return 1;
    if (S_ISDIR(file.st_mode))
    {
        if(checkPermission(fullPath.c_str(),fd,method.version,X_OK,radyForClose) == true)
            return 4;;
        return 2;
    }
    return 3;
}

void getMethod(Data & datacleint,Method &method, std::vector<std::pair<std::string,ServerConfig> >&Servers,int cfd)
{
    try
    {
        if(datacleint.modeAutoIndex == true)
        {
            if(datacleint.isReading == false)
                openFileAndSendHeader(datacleint,method,cfd);
            else
                serveFIle(datacleint,cfd);
        }
        else if(datacleint.isReading == false)
        {
            ServerConfig config =  getServer(Servers,method.host);
            method.autoFile = config.autoFile;
            method.rootLocation = config.root;
            int i = checkFileOrDirectoryPermission(method,cfd,datacleint.readyForClose);
            if( i == 4)
                return;
            if(i == 2)
            {
                /* hundle DIRECTORY */
                if(listingDirectory(datacleint,method,cfd) == 0)
                {
                    const std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + method.buff;
                    if(send(cfd, httpResponse.c_str(), httpResponse.size(),0) == -1)
                    {
                        datacleint.readyForClose = true;
                        throw std::runtime_error("An error aka client disconnect");
                    }
                    method.buff.clear(); 
                    datacleint.readyForClose = true;
                    return ;
                }
            }
            else if(i == 0)
            {
                /* hundle  not found*/
                std::string httpResponse = "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n" ;
                httpResponse += "<html><head><center><h1>404 NOT FOUND</h1></center></head></html>";
                if(send(cfd, httpResponse.c_str(), httpResponse.size(),0) == -1)
                {
                    datacleint.readyForClose = true;
                    throw std::runtime_error("An error aka client disconnect");
                }
                datacleint.readyForClose = true;
            }
            else if(i == 1)
            {
                /* hundle file */
                openFileAndSendHeader(datacleint,method,cfd);
            }
        }
        else
            serveFIle(datacleint,cfd);
    }
    catch (const std::runtime_error &e)
    {
        if(strcmp(e.what() ,"internal server error") == 0)
        {
            std::string status = " 500 Internal Server Error";
            sendResponse(cfd,method.version,status,datacleint.readyForClose);
        }
        // std::cout << e.what() << std::endl;
    }
}
