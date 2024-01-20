#include"webserv.hpp"
#include <sys/stat.h>
#include<cstdio>
#include<cstdlib>
void serveFIle(Data& datacleint, int cfd);
// void   deleteMethod(const std::string &path1,const std::string &version)
// {
//     struct stat fileStat;
//     std::ofstream testfile("tools/index.html");
//     if(!testfile.is_open())
//     {
//         testfile << version+" 404 NOT FOUND\r\n" + "{message: can't opning file}";
//         return;
//     }
//     std::string response;
//     std::string dir = "www/wordpress";
//     std::string fullPath = dir + "/" + path1;
//     const char* path = fullPath.c_str();
//     if(stat(path,&fileStat) == 0)
//     {
//         if(S_ISREG(fileStat.st_mode))
//         {
//             if(remove(path) == 0)
//             {
//                 testfile <<version+" 200 OK\r\n" + "{message: Resource deleted successfully}";
//                 return; 
//             }
//             else
//             {
//                 testfile<< version+" 404 NOT FOUND\r\n" + "{message: can't removing file}";
//                 return ;
//             }   
//         }
//         else if(S_ISDIR(fileStat.st_mode))
//         {
//             if(rmdir(path) == 0)
//             {

//                 testfile <<version+" 200 OK\r\n" + "{message: Resource deleted successfully}";
//                 return ;
//             }
//             else
//             {
//                 testfile<< version+" 404 NOT FOUND\r\n" + "{message: can't removing directory}";
//                 return ;
//             }
//         }
//         else
//         {
//             testfile << version+" 404 NOT FOUND\r\n" + "{not such file or directory}";
//             return;
//         }
//     }
//     else
//         testfile << version+" 404 NOT FOUND\r\n" + "{message: can't removing file/diroctory}";
//     testfile.close();
//     return;
// }


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
int isFileOrDirectory(Method &method)
{
    std::string fullPath = method.rootLocation + method.path;
    std::cout<<fullPath<<"\n";
    struct stat file;
    if (stat(fullPath.c_str(), &file) != 0)
        return(0);
    if (S_ISREG(file.st_mode))
        return 1;
    if (S_ISDIR(file.st_mode))
        return 2;
    return 0;
}

// void serveFIle(,Method &method, int cfd);

int    listingDirectory(Data data,Method &method,int cfd)
{
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
            if(strcmp(it->d_name , ".") == 0 || strcmp(it->d_name , "..") == 0)
                continue;
            if(strcmp(it->d_name,method.autoFile.c_str()) == 0)
            {
                method.path = method.path + method.autoFile;
                serveFIle(data,cfd);
                return (1);
            }
            std::string directoryChildPath = directoryPath  + it->d_name;
            struct stat file;
            if (stat(directoryChildPath .c_str(), &file) == 0)
            { 
                list << "<tr>";
                if (S_ISREG(file.st_mode))
                    list << "<td>"<< "<a href='" << it->d_name << "'>" << it->d_name << "</a></td>";
                if (S_ISDIR(file.st_mode))
                    list << "<td>"<< "<a href='" << it->d_name << "/" << "'>" << it->d_name << "</a></td>";
                list << "<td>"<< ctime(&file.st_mtime) <<"</td>";
                list << "<td>"<< file.st_size << " bytes</td>";
                list << "</tr>";
            }
            else
                list<< "<p>Error  opening directory</p>";
        }
        closedir(dir);
    }
    else
        list<< "<p>Directory not found </p>";
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
    // bool flag = 1;
    // ssize_t bytesRead;
    char buffer[BUFFER_SIZE];
    std::string contentType = getContentType(method);
    // std::cout<<"---contentType-"<< contentType <<"\n";

    
    // if(contentType == ".php")
    // {
    //     /* hundle CGI */
    //     contentType == "text/html";
    //     std::string tmp  = method.rootLocation + method.path;
    //     fastCGI(tmp);
    //     method.path = "/tmp/tmpFile";
    //     flag = 0;
    // }
    // else
   
    datacleint.isReading = 1;
    method.path = method.rootLocation + method.path;
    memset(buffer,0,sizeof(buffer));
    // std::cout<<"----"<< method.path <<"\n";
    datacleint.fd = open(method.path.c_str(), O_RDONLY);
    if (datacleint.fd == -1)
    {
        close(datacleint.fd);
        throw std::runtime_error("Error opening file");
    }
        
    std::string httpResponse = method.version + " 200 OK\r\nContent-Type:" +contentType+ " \r\nTransfer-Encoding: chunked\r\n\r\n";
    if(send(cfd, httpResponse.c_str(), httpResponse.size(),0) == -1)
    {
        close(datacleint.fd);
        datacleint.readyForClose = 1;
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
        datacleint.readyForClose = 1;
        throw std::runtime_error("EROOR !!!");
    }
        
    if(byteRead == 0)
    {
        close(datacleint.fd);
        datacleint.readyForClose = 1;
        if(send(cfd, "0\r\n\r\n", sizeof("0\r\n\r\n") - 1,0) == -1)
           throw std::runtime_error("An error aka client disconnect");
        return ;
    }
    sendChunk(cfd,buffer,byteRead,datacleint);  
    // while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0)
    // {
    //     try
    //     {
    //         std::string responseChunk(buffer, bytesRead);
    //         if (flag == 0)
    //         {  
    //             size_t headerEndPos = responseChunk.find("\r\n\r\n");
    //             if (headerEndPos != std::string::npos)
    //             {
    //                 flag = true;
    //                 std::string htmlContent = responseChunk.substr(headerEndPos + 4);
    //                 sendChunk(cfd, htmlContent.c_str(), htmlContent.size());
    //             }
    //         }
    //         else
    //             sendChunk(cfd, responseChunk.c_str(), responseChunk.size());
    //         memset(buffer, 0, sizeof(buffer));
    //     }
    //     catch (const std::runtime_error &e)
    //     {
    //         std::cout << e.what() << std::endl;
    //         close(fd);
    //         return;
    //     }
    // }
    // if(bytesRead == -1)
    // {
    //     close(fd);
    //     std::runtime_error("Falied read from file!!!");
    // }
    // remove("/tmp/tmpFile");
    // close(fd);
    // send(cfd, "0\r\n\r\n", sizeof("0\r\n\r\n") - 1,0);
}



void getMethod(Data & datacleint,Method &method, std::vector<std::pair<std::string,ServerConfig> >&Servers,int cfd)
{
    try
    {    
        if(datacleint.isReading == 0)
        {
            ServerConfig config =  getServer(Servers,method.host);
            method.autoFile = config.autoFile;
            method.rootLocation = config.root;
            if(method.path == "/favicon.ico" )
            {
                datacleint.readyForClose = 1;
                return;
            }
            int i = isFileOrDirectory(method);
            if(i == 2)
            {
                /* hundle DIRECTORY */
                if(listingDirectory(datacleint,method,cfd) == 0)
                {
                    const std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + method.buff;
                    if(send(cfd, httpResponse.c_str(), httpResponse.size(),0) == -1)
                    {
                        datacleint.readyForClose = 1;
                        throw std::runtime_error("An error aka client disconnect");
                    }
                    method.buff.clear(); 
                    datacleint.readyForClose = 1;
                }
            }
            if(i == 0)
            {
                /* hundle  not found*/
                std::string httpResponse = "HTTP/1.1 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n" ;
                httpResponse += "<html>\r\n<head>\r\n<title>404 - Page Not Found</title>\r\n</head>\r\n<body>\r\n";
                httpResponse += "<div style=\"text-align: center;\">\r\n"; 
                httpResponse += "<h1>404 - Page Not Found</h1>\r\n";
                httpResponse += "<p>The requested page could not be found.</p>\r\n";
                httpResponse += "</div>\r\n"; 
                httpResponse += "</body>\r\n</html>";
                if(send(cfd, httpResponse.c_str(), httpResponse.size(),0) == -1)
                {
                    datacleint.readyForClose = 1;
                    throw std::runtime_error("An error aka client disconnect");
                }
                datacleint.readyForClose = 1;
            }
            if(i == 1)
            {
                /* opning file and send hedear */
                openFileAndSendHeader(datacleint,method,cfd);
                /* here we go */
            }
        }
        else
        {
            serveFIle(datacleint,cfd);
        }

    }
    catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
}
