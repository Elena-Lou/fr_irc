#include <iostream>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include<unistd.h>

// int setsockopt(int sockfd, int level, int optname,  const void *optval, socklen_t optlen);
// int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// int listen(int sockfd, int backlog);
// int new_socket= accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main( int ac, char** av ) {

	if (ac != 2)
	{
		std::cerr << "usage: ./ircserv hostname" << std::endl;
		return 1;
	}

	int status;

	struct addrinfo hints;
	struct addrinfo *servinfo;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int opt = 1;

	if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0)
	{
		std::cerr << gai_strerror(status) << std::endl;
		exit(1);
	}

	int socketFD;
	socketFD = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (socketFD < 0)
	{
		std::cerr << "socket() failed : " << std::strerror(errno) << std::endl;
		exit(1);
	}
	std::cout << "socketFD : " << socketFD << std::endl;

	if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
		{
		std::cerr << "setsockopt() failed : " << std::strerror(errno) << std::endl;
		exit(1);	
	}

	if (bind(socketFD, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
	{
		std::cerr << "bind() failed : " << std::strerror(errno) << std::endl;
		exit(1);	
	}
	
	if (listen(socketFD, 5) < 0)
	{
		std::cerr << "listen() failed : " << std::strerror(errno) << std::endl;
		exit(1);	
	}

	struct sockaddr_storage pendingAddr;
	socklen_t				pendingAddrSize;
	int						newFD;

	while (1)
	{
		pendingAddrSize = sizeof(pendingAddr);
		newFD = accept(socketFD, reinterpret_cast<struct sockaddr *>(&pendingAddr), &pendingAddrSize);
		if (newFD < 0)
		{
			std::cerr << "accept() failed : " << std::strerror(errno) << std::endl;
			exit(1);	
		}
		if (!fork())
		{
			close(socketFD);
			if (send(newFD, av[1], strlen(av[1]), 0) == -1)
			{
				std::cerr << "send() failed : " << std::strerror(errno) << std::endl;
				exit(1);	
			}
			close(newFD);
			exit(0);
		}
		close(newFD);
	}


	freeaddrinfo(servinfo);
}