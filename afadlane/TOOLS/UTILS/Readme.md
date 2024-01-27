# Multiplexing I/O

### Non-Blocking Servers

***
In the Non-Blocking approach, one thread can handle multiple queries at a time. A server using a Non-Blocking socket works in an asynchronous manner means once the request is received it lets the system perform the task on it, and continues to take other requests and once the task is over it responds according to each request. As shown in the picture it only has one thread and the whole process is carried out using a concept called “Event Loop”. Here the system is not idle and never waits for each and every task to get completed before taking other requests.
***
![Sample Image](/tools/Nio.png)

***
For example, take the read() system call that reads data from a file into the buffer allocated by the user program. The call returns into userspace only after the user buffer has been filled with the desired data. The thread is essentially put to sleep only to be later awakened when disk control has completed the I/O from the storage device and put the data into the kernel space buffer. 
***


# HOW EPOLL WORK 

![Sample Image](/tools/lrYLy.png)