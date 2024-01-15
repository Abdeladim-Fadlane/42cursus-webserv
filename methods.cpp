#include"webserv.hpp"
#include <sys/stat.h>
#include<cstdio>
#include<cstdlib>

void   deleteMethod(const std::string &path1,const std::string &version)
{
    struct stat fileStat;
    std::ofstream testfile("tools/index.html");
    if(!testfile.is_open())
    {
        testfile << version+" 404 NOT FOUND\r\n" + "{message: can't opning file}";
        return;
    }
    std::string response;
    std::string dir = "www/wordpress";
    std::string fullPath = dir + "/" + path1;
    const char* path = fullPath.c_str();
    if(stat(path,&fileStat) == 0)
    {
        if(S_ISREG(fileStat.st_mode))
        {
            if(remove(path) == 0)
            {
                testfile <<version+" 200 OK\r\n" + "{message: Resource deleted successfully}";
                return; 
            }
            else
            {
                testfile<< version+" 404 NOT FOUND\r\n" + "{message: can't removing file}";
                return ;
            }   
        }
        else if(S_ISDIR(fileStat.st_mode))
        {
            if(rmdir(path) == 0)
            {

                testfile <<version+" 200 OK\r\n" + "{message: Resource deleted successfully}";
                return ;
            }
            else
            {
                testfile<< version+" 404 NOT FOUND\r\n" + "{message: can't removing directory}";
                return ;
            }
        }
        else
        {
            testfile << version+" 404 NOT FOUND\r\n" + "{not such file or directory}";
            return;
        }
    }
    else
        testfile << version+" 404 NOT FOUND\r\n" + "{message: can't removing file/diroctory}";
    testfile.close();
    return;
}

void sendChunk(int clientSocket, const char* data, ssize_t length)
{
    std::stringstream chunkHeader;
    chunkHeader << std::hex << length << "\r\n";
    std::string chunkHeaderStr = chunkHeader.str();
    if (write(clientSocket, chunkHeaderStr.c_str(), chunkHeaderStr.size()) < 0)
        throw std::runtime_error("Error writing chunk header");
    if (length > 0 && write(clientSocket, data, length) < 0)
        throw std::runtime_error("Error writing chunk data");
    if (write(clientSocket, "\r\n", sizeof("\r\n") - 1) < 0)
        throw std::runtime_error("Error writing chunk footer");
}


/* Be patient dahdouh */

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
    contentTypeMap[".php"] = "text/html";
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
    struct stat file;
    // std::cout<<"<<<"<<fullPath<<">>>\n";
    if (stat(fullPath.c_str(), &file) != 0)
        return(0);
    if (S_ISREG(file.st_mode))
        return 1;
    if (S_ISDIR(file.st_mode))
        return 2;
    return 0;
}
void serveFIle(Method &method, int cfd);
int    listingDirectory(Method &method,int cfd)
{
    const char *autoFile = "index.html";
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
            if(strcmp(it->d_name,autoFile) == 0)
            {
                std::cout<<"f = "<<it->d_name <<"\n";

                method.path = method.path + autoFile;
                serveFIle(method,cfd);
                return (1);
            }
            // if (!method.path.empty() && method.path.back() == '/')
            //     method.path.pop_back();
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
            // std::cout<<"--------------------------------------"<<"\n";
            
        }
        closedir(dir);
    }
    else
        list<< "<p>Directory not found </p>";
    list << "</table></body></html>";
    method.buff =  list.str();
    return(0);
}

void serveFIle(Method &method, int cfd)
{
    std::string contentType = getContentType(method);
    method.path = method.rootLocation + method.path;
    int fd = open(method.path.c_str(), O_RDONLY);
    if (fd == -1)
        throw std::runtime_error("Error opening file");
    char buffer[BUFFER_SIZE];
    memset(buffer,0,sizeof(buffer));
    ssize_t bytesRead;
    std::string httpResponse = method.version + " 200 OK\r\nContent-Type:" + contentType+" \r\nTransfer-Encoding: chunked\r\n\r\n";
    write(cfd, httpResponse.c_str(), httpResponse.size());  
    while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0)
    {
        try
        {
            sendChunk(cfd, buffer, bytesRead);

        } catch (const std::runtime_error& e)
        {
            std::cout<<e.what()<<std::endl;
            close(fd);
            return;
        }
    }
    close(fd);
    write(cfd, "0\r\n\r\n", sizeof("0\r\n\r\n") - 1);
}

void getMethod(Method &method, int cfd)
{
    method.rootLocation = "/home/afadlane/webserv/tools/BlastSphere";
    if(method.path == "/favicon.ico" )
        return;
    int i = isFileOrDirectory(method);
    if(i == 2)
    {
        /* hundle DIRECTORY */
        if(listingDirectory(method,cfd) == 0)
        {
            const std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + method.buff;
            write(cfd, httpResponse.c_str(), httpResponse.size());
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
        write(cfd, httpResponse.c_str(), httpResponse.size());
    }
    if(i == 1)
    {
        /* hundle file */
        serveFIle(method,cfd);
        /* here we go */
    }
}

int paceUrl(std::string line,Method &object)
{
    std::istringstream wiss(line);
    if (!(wiss >> object.method >> object.path >> object.version))
    {
        std::cerr << "Error parsing the request." << std::endl;
        return 0;
    }
    return(1);
}

int Methods(int fd,Method &object)
{
    std::string line;
    ssize_t bytesRead;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        line += buffer;
        if (line.find("\r\n\r\n") != std::string::npos) {
            break; 
        }
    }
    // std::cout << line << std::endl;
    std::string firstLine;
    std::istringstream wiss(line);
    std::getline(wiss, firstLine);
    paceUrl(firstLine, object);
    // std::cout << "Method: " << object.method << std::endl;
    // std::cout << "Version: " << object.version << std::endl;
    return 1;
}
