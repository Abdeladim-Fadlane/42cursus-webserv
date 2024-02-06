#include"webserv.hpp"

void serveFIle(Data& dataClient);

std::string    getContentType(Data &method)
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
    contentTypeMap[".sh"] = ".sh";
    contentTypeMap[".css"] = "text/css";
    contentTypeMap[".pdf"] = " application/pdf";
    contentTypeMap[".js"] = "application/javascript";

    size_t pos = method.Path.find_last_of(".");
    if(pos != std::string::npos)
    {
        std::string extension = method.Path.substr(pos);
        std::map<std::string,std::string>::iterator it = contentTypeMap.find(extension);
        if(it != contentTypeMap.end())
            return it->second;
    }
    return "application/json";
}

void    openFileAndSendHeader(Data& dataClient);

bool getAutoFile(Data & dataClient,char * path)
{
    for(size_t i = 0;i < dataClient.autoFile.size(); i++)
    {
        if(strcmp(path,dataClient.autoFile[i].c_str()) == 0)
        {
            dataClient.Path += dataClient.autoFile[i];
            dataClient.modeAutoIndex = true;
            return true ;
        }
    }
    return false;
}

int    listingDirectory(Data &dataClient)
{
    std::ostringstream list;
    list << "<html><head><title>Directory Listing</title></head><body>";
    list << "<h1>Index of: " << dataClient.requeste->path << "</h1>";
    list << "<table>";
    std::string directoryPath = dataClient.Path + "/";
    DIR *dir =  opendir(directoryPath.c_str());
    struct dirent *it;
    while((it = readdir(dir)) != NULL)
    {   
        std::string directoryChildPath = directoryPath  + it->d_name;
        struct stat statInfo;
        if(strcmp(it->d_name , ".") == 0 || strcmp(it->d_name , "..") == 0)
            continue;
        if(getAutoFile(dataClient,it->d_name) == true)
            return (1);
        else if (stat(directoryChildPath .c_str(), &statInfo) == 0)
        {
            list << "<tr>";
            if (S_ISREG(statInfo.st_mode))
                list << "<td>"<< "<a href='" << dataClient.requeste->path +  std::string(it->d_name) << "'>" << it->d_name << "</a></td>";
            if (S_ISDIR(statInfo.st_mode))
                list << "<td>"<< "<a href='" << dataClient.requeste->path + std::string(it->d_name)  << "/" << "'>" << it->d_name << "</a></td>";
            list << "<td>"<< ctime(&statInfo.st_mtime) <<"</td>";
            list << "<td>"<< statInfo.st_size << " bytes</td>";
            list << "</tr>";
        }
    }
    closedir(dir);
    list << "</table></body></html>";
    dataClient.listDirectory =  list.str();
    return(0);
}

void sendChunk(int clientSocket, const char* data, ssize_t length,Data& dataClient)
{
    std::string totalChuncked ;
    std::stringstream chunkHeader;
    chunkHeader << std::hex << length << "\r\n";
    std::string chunkHeaderStr = chunkHeader.str();
    std::string chunkData(data,length);

    totalChuncked = chunkHeaderStr + chunkData + "\r\n";
    if (send(clientSocket, totalChuncked.c_str(), totalChuncked.size(),0) ==  -1)
    {
        close(dataClient.fileFd);
        dataClient.readyForClose = true;
    }
}
bool checkIsCgi(std::string &contentType)
{
    if((contentType == ".sh" || contentType == ".php" || contentType == ".py") )
        return true;
    return false;
}
void    openFileAndSendHeader(Data& dataClient)
{
    char buffer[BUFFER_SIZE];
    std::string contentType = getContentType(dataClient);
    if(checkIsCgi(contentType) == true && dataClient.requeste->Location_Server.cgi_allowed == "ON")
    {
        fastCGI(dataClient,contentType);
        return ;
    }
    memset(buffer,0,sizeof(buffer));
    if(checkPermission(dataClient,dataClient.Path.c_str(),R_OK) == true)
        return;
    dataClient.isReading = true;
    dataClient.fileFd = open(dataClient.Path.c_str(), O_RDONLY);
    if (dataClient.fileFd == -1)
    {
        close(dataClient.fileFd);
        throw std::runtime_error("internal server error");
    }
    std::string httpResponse = dataClient.requeste->http_v + " 200 OK\r\nContent-Type:" +contentType+ " \r\nTransfer-Encoding: chunked\r\n\r\n";
    if(send(dataClient.fd, httpResponse.c_str(), httpResponse.size(),0) == -1)
    {
        close(dataClient.fileFd);
        dataClient.readyForClose = true;
    } 
}

void serveFIle(Data& dataClient)
{
    char buffer[BUFFER_SIZE];
    ssize_t byteRead = read (dataClient.fileFd,buffer,BUFFER_SIZE);
    if(byteRead == -1)
    {
        close(dataClient.fileFd);
        dataClient.readyForClose = true;
        throw std::runtime_error("internal server error");
    }
    if(byteRead == 0)
    {
        close(dataClient.fileFd);
        dataClient.readyForClose = true;
        send(dataClient.fd, "0\r\n\r\n", sizeof("0\r\n\r\n") - 1,0);
        return ;
    }
    std::string httpresponse(buffer,byteRead);
    sendChunk(dataClient.fd,httpresponse.c_str(),httpresponse.size(),dataClient);  
}

int checkFileOrDirectoryPermission(Data &dataClient)
{
    if(access(dataClient.Path.c_str(),F_OK) != 0)
        return 0;
    if(checkPermission(dataClient,dataClient.Path.c_str(),R_OK) == true)
        return 4;
    struct stat file;
    stat(dataClient.Path.c_str(), &file);
    if (S_ISREG(file.st_mode))
        return 1;
    if (S_ISDIR(file.st_mode))
    {
        if(checkPermission(dataClient,dataClient.Path.c_str(),X_OK) == true)
            return 4;;
        return 2;
    }
    return 3;
}

void getMethod(Data & dataClient)
{
    try
    {
        if(dataClient.modeAutoIndex == true)
        {
            if(dataClient.isReading == false)
                openFileAndSendHeader(dataClient);
            else
                serveFIle(dataClient);
        }
        else if(dataClient.isReading == false)
        {
            int i = checkFileOrDirectoryPermission(dataClient);
            if( i == 4)
                return;
            if(i == 2)
            {
                /* listing DIRECTORY */
                if(dataClient.autoIndex == false)
                {
                    std::string msg = " 403 Forbidden";
                    sendResponse(dataClient,msg);
                    return;
                }
                if(listingDirectory(dataClient) == 0)
                {
                    const std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + dataClient.listDirectory;
                    if(send(dataClient.fd, httpResponse.c_str(), httpResponse.size(),0) == -1)
                        dataClient.readyForClose = true;
                    dataClient.listDirectory.clear(); 
                    dataClient.readyForClose = true;
                    return ;
                }
            }
            else if(i == 0)
            {
                /* hundle Not Found case */
                std::string msg = " 404 NOT FOUND";
                sendResponse(dataClient,msg);
                dataClient.readyForClose = true;
            }
            else if(i == 1)
            {
                /* hundle files */
                openFileAndSendHeader(dataClient);
            }
        }
        else
            serveFIle(dataClient);
    }
    catch (const std::runtime_error &e)
    {
        // if(strcmp(e.what() ,"internal server error") == 0)
        // {
        //     std::string status = " 500 Internal Server Error";
        //     sendResponse(dataClient,status);
        // }
        // dataClient.readyForClose = true;
        std::cout<<e.what()<<"\n";
        return ;
    }
}
