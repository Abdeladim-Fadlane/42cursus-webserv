## 1. Introduction

In the Webserv project at 42, you're tasked with creating your own web server in C++. This involves understanding the HTTP protocol, socket programming, request handling, response generation, configuration file parsing, multiplexing, error handling, testing, and debugging.

### Key Aspects

- **HTTP Protocol**: Understanding HTTP methods, headers, status codes, and the request-response cycle is crucial.
- **Socket Programming**: Implementing web server functionality involves working with sockets to establish connections and handle requests.
- **Request Handling**: Your server must receive and parse HTTP requests from clients, extracting important information.
- **Response Generation**: After processing requests, your server must generate appropriate HTTP responses.
- **Configuration Files**: Implement parsing logic to read and apply configuration settings from files.
- **Multiplexing**: Implement concurrency mechanisms such as asynchronous I/O to handle multiple client connections simultaneously.
- **Error Handling**: Gracefully handle errors including invalid requests and network errors.
- **Testing and Debugging**: Thoroughly test and debug your server to ensure proper functionality.

## 2 . Resources

[Webserv raod map](https://www.figma.com/file/tQJOYxzFdxYkPYrPIv18kg/webserv-Flow?type=design&node-id=0-1&mode=design)

[Websocket](https://ipwithease.com/what-is-a-websocket/)

### Server setup

[How to build a simple HTTP server](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)

[Simple server with C++](https://ncona.com/2019/04/building-a-simple-server-with-cpp/)

[C++ Web Programming](https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm)

### HTTP Header Syntax

[HTTP Request Methods](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol#Request_methods)

[HTTP Status Codes](https://en.wikipedia.org/wiki/List_of_HTTP_status_codes)

[HTTP Header Break Style](https://stackoverflow.com/questions/5757290/http-header-line-break-style)

### Epoll and non-blocking

[Epoll](https://medium.com/@avocadi/what-is-epoll-9bbc74272f7c)

[How epoll work](TOOLS/UTILS/lrYLy.png)

[Non Blocking Server](https://www.geeksforgeeks.org/non-blocking-server-in-java-nio/?ref=header_search)

### CGI
[CGI Programming](https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db)

## 3. Installation & Usage

   1. Clone the repository:
      ```bash
      git clone https://github.com/Abdeladim-Fadlane/webserv.git
      
   2. Navigate to the cloned directory:
      ```bash 
      cd webserv
   
   3. Build the project
      ```bash 
      make
   
   4. run the server 
       ```bash
      ./webserv
   5. Try on browser
      ```bash
      http://localhost:8082/
### Contributors
- [Abdeladim Fadlane](https://github.com/Abdeladim-Fadlane)
- [Ayoub Katfi](https://github.com/ayyoubkatfi)

