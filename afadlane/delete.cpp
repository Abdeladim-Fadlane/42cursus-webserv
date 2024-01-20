

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

