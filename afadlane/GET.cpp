#include"webserv.hpp"

bool checkIsCgi(std::string &contentType)
{
    if((contentType == ".pl" || contentType == ".php" || contentType == ".py") )
        return true;
    return false;
}

bool checkCgiExtention(Data &dataClient ,std::string &contentType)
{
    if(dataClient.requeste->Location_Server.cgi.find(contentType.c_str()) != dataClient.requeste->Location_Server.cgi.end())
        return true;
    return false;
}

bool GETMETHOD::getAutoFile(Data & dataClient,char * path)
{
    if(strcmp(path,dataClient.requeste->Location_Server.index.c_str()) == 0)
    {
        dataClient.Path = dataClient.Path + "/" + dataClient.requeste->Location_Server.index;
        modeAutoIndex = true;
        return true ;
    }
    return false;
}

GETMETHOD::GETMETHOD()
{
    fdFile         =   new std::ifstream();
    isReading = false;
    Alreadyopen = false;
    modeAutoIndex = false;
}

GETMETHOD::~GETMETHOD()
{
    if(fdFile->is_open())
        fdFile->close();
    delete fdFile;
}

std::string    GETMETHOD::getContentType(Data &dataClient)
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
    contentTypeMap[".pl"] = ".pl";
    contentTypeMap[".css"] = "text/css";
    contentTypeMap[".pdf"] = " application/pdf";
    contentTypeMap[".js"] = "application/javascript";
    size_t pos = dataClient.Path.find_last_of(".");
    if(pos != std::string::npos)
    {
        std::string extension = dataClient.Path.substr(pos);
        std::map<std::string,std::string>::iterator it = contentTypeMap.find(extension);
        if(it != contentTypeMap.end())
            return it->second;
    }
    return "application/json";
}

void  makeTableStyle(std::stringstream &list)
{
    list << "<html><head><title>Directory Listing</title>";
    list << "<style>";
    list << "table {";
    list << "    font-family: Arial, sans-serif;";
    list << "    border-collapse: collapse;";
    list << "    width: 100%;";
    list << "}";
    list << "td, th {";
    list << "    border: 1px solid #dddddd;";
    list << "    text-align: left;";
    list << "    padding: 8px;";
    list << "}";
    list << "tr:nth-child(even) {";
    list << "    background-color: #f2f2f2;";
    list << "}";
    list << "h1 {";
    list << "    font-family: Arial, sans-serif;";
    list << "}";
    list << "</style>";
    list << "</head><body>";
}

int GETMETHOD::listingDirectory(Data &dataClient)
{
    std::stringstream list;
    makeTableStyle(list);
    list << "<h1>Index of: " << dataClient.requeste->path << "</h1><table><tr>";
    std::string directoryPath = dataClient.Path ;
    DIR *dir =  opendir(directoryPath.c_str());
    if(!dir)
        throw std::runtime_error("error");
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
            list << "<td>"<< statInfo.st_size << " bytes</td></tr>";
        }
    }
    closedir(dir);
    list << "</table></body></html>";
    listDirectory =  list.str();
    return(0);
}


void    GETMETHOD::openFileAndSendHeader(Data& dataClient)
{
    std::string contentType = getContentType(dataClient);
    if(checkIsCgi(contentType) == true && dataClient.requeste->Location_Server.cgi_allowed == "ON")
    {
        if(checkCgiExtention(dataClient,contentType) == true)
        {
            dataClient.OBJCGI.fastCGI(dataClient,contentType);
            return ;
        }
    }
    if(checkPermission(dataClient,R_OK) == true)
        return;
    isReading = true;
    fdFile->open(dataClient.Path.c_str());
    if(!fdFile->is_open())
        throw std::runtime_error("error");
    std::string httpResponse;
    struct stat fileInfo;
    if(stat(dataClient.Path.c_str(),&fileInfo))
        throw std::runtime_error("error");
    std::stringstream ss;
    ss <<fileInfo.st_size ;
    httpResponse = dataClient.requeste->http_v.append(" 200 OK\r\nContent-Type: ");
    httpResponse.append(contentType).append("; charset=UTF-8\r\nContent-Lenght: ");
    httpResponse.append(ss.str()).append("\r\n\r\n");
    sendResponce(dataClient,httpResponse);
}

void GETMETHOD::serveFIle(Data& dataClient)
{
    char buffer[BUFFER_SIZE];
    fdFile->read(buffer,BUFFER_SIZE);
    std::streamsize bytesRead = fdFile->gcount();
    std::string httpresponse(buffer,bytesRead);
    if(bytesRead == 0 || fdFile->bad())
        dataClient.readyForClose = true;
    else
        sendResponce(dataClient,httpresponse);
}

int GETMETHOD::checkFileDirPermission(Data &dataClient)
{
    if(access(dataClient.Path.c_str(),F_OK) != 0)
        return 0;
    if(checkPermission(dataClient,R_OK) == true)
        return 4;
    struct stat file;
    stat(dataClient.Path.c_str(), &file);
    if (S_ISREG(file.st_mode))
        return 1;
    if (S_ISDIR(file.st_mode))
    {
        if(checkPermission(dataClient,X_OK) == true)
            return 4;
        return 2;
    }
    return 3;
}

void GETMETHOD::sendListDir(Data & dataClient)
{
    std::string httpResponse;
    std::ostringstream wiss;
    wiss << listDirectory.size();
    httpResponse = dataClient.requeste->http_v;
    httpResponse.append(" 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Lenght: ");
    httpResponse.append(wiss.str()).append("\r\n\r\n").append(listDirectory);
    sendResponce(dataClient,httpResponse);
    listDirectory.clear(); 
    dataClient.readyForClose = true;
}

void    GETMETHOD::openDirFIle(Data & dataClient)
{
    int i = checkFileDirPermission(dataClient);
    if( i == 4)
        return;
    if(i == 2)
    {
        if(listingDirectory(dataClient) == 0)
        {
            if(dataClient.autoIndex == false)
            {
                dataClient.statusCode = " 403 Forbidden";
                dataClient.code = 403;
            }
            else
                sendListDir(dataClient);
        }
    }
    else if(i == 0)
    {
        dataClient.statusCode = " 404 NOT FOUND";
        dataClient.code = 404;
    }
    else if(i == 1)
        openFileAndSendHeader(dataClient);
}

void GETMETHOD::getMethod(Data & dataClient)
{
    try
    {
        if(modeAutoIndex == true)
        {
            if(isReading == false)
                openFileAndSendHeader(dataClient);
            else
                serveFIle(dataClient);
        }
        else if(isReading == false)
            openDirFIle(dataClient);
        else
            serveFIle(dataClient);
    }
    catch (const std::exception &e)
    {
        if(strcmp(e.what() ,"error send") == 0)
            dataClient.readyForClose = true;
        else
        {
            dataClient.statusCode = " 500 Internal Server Error";
            dataClient.code = 500;
        }
    }
}