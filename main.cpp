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
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>

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

	// if (ac != 2)
	// {
	// 	std::cerr << "usage: ./ircserv hostname" << std::endl;
	// 	return 1;
	// }

	(void)ac;
	(void)av;

	int status;

	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct addrinfo	*tmp;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int opt = 1;
	int socketFD; //listening socket
	int newFD; // clone socket created after accept()

	if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0)
	{
		std::cerr << gai_strerror(status) << std::endl;
		exit(1);
	}

	/* select an address which is operational */
	for (tmp = servinfo; tmp != NULL; tmp = tmp->ai_next)
	{
		/* creates a socket descriptor with the info from getaddrinfo() */

		socketFD = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		std::cout << "socketFD : " << socketFD << std::endl;
		if (socketFD < 0)
		{
			std::cerr << "socket() failed : " << std::strerror(errno) << std::endl;
			continue ;
		}
	
		std::cout << "\n--- SETSOCKOPT ---\n" << std::endl;

		/* sets the descriptor as reusable */
		if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
			{
			std::cerr << "setsockopt() failed : " << std::strerror(errno) << std::endl;
			close(socketFD);
			continue ;	
		}

		/* sets the socket as non blocking */
		std::cout << "\n--- FCNTL---\n" << std::endl;

		int flag = fcntl(socketFD, F_GETFL, 0);
		std::cout << "flag : " << flag << std::endl;
		if (fcntl(socketFD, F_SETFL, flag | O_NONBLOCK) < 0)
		{
			std::cerr << "fcntl() failed : " << std::strerror(errno) << std::endl;
			close(socketFD);
			continue;	
		}

		std::cout << "\n--- BIND ---\n" << std::endl;
		/* binds the socket descriptor to the port passed to getaddrinfo() */
		if (bind(socketFD, tmp->ai_addr, tmp->ai_addrlen) < 0)
		{
			std::cerr << "bind() failed : " << std::strerror(errno) << std::endl;
			close(socketFD);
			continue;	
		}
		break ;
	}
	if (tmp == NULL)
	{
		std::cerr << "Didn't work out, sorry. It's not you, it's me..." << std::endl;
		exit(1);
	}

	std::cout << "\n--- LISTEN ---\n" << std::endl;

	/* listens to the port bound to the socket descriptor for incoming connections */
	if (listen(socketFD, 5) < 0)
	{
		std::cerr << "listen() failed : " << std::strerror(errno) << std::endl;
		close(socketFD);
		exit(1);	
	}
	std::cout << "freeaddrinfo(NULL)" << std::endl;
	freeaddrinfo(NULL);

	/* setting up structures needed for select */
	struct timeval	timeOut;
	fd_set			masterSet;
	fd_set			workingSet; // select erase the list given so we need to save it in a tmp set
	int				fdMax; //biggest  fd
	int				selectRet; //saves select() return value
	int				recvRet; // saves recv() return value
	char			buffer[80];
	int				i;

	FD_ZERO(&masterSet); // initialise the list of fd to 0
	FD_ZERO(&workingSet); // same for the working one

	FD_SET(socketFD, &masterSet); //add the current socket descriptor to the set

	fdMax = socketFD;

	/* initialises the timeval struct to shut down after 30s of inactivity */
	timeOut.tv_sec = 120;
	timeOut.tv_usec = 0;

	/* setting up sockaddr_storage structures for accept() : large struct which will hold IPv4 or IPV6. 
	This is where the info about the icoming connection will be stored. */

	struct sockaddr_storage pendingAddr;
	socklen_t				pendingAddrSize;

	/* WAITING FOR CONNECTIONS LOOP */
	while(1)
	{
		memcpy(&workingSet, &masterSet, sizeof(masterSet)); //

		// std::cout << "Waiting for connections..." << std::endl;
		selectRet = select(fdMax + 1, &workingSet, NULL, NULL, &timeOut);
		if (selectRet < 0)
		{
			std::cerr << "select() failed :  " << std::strerror(errno) << std::endl;
			break ;
		}
		else if (selectRet == 0)
		{
			std::cerr << "select() timed out. Bye Bye" << std::endl;
			break ;
		}
		/* there is one or more readable fd */
		for (i = 3; i <= fdMax; i++)
		{
			if (FD_ISSET(i, &workingSet))
			{
				if (i == socketFD)
				{
						pendingAddrSize = sizeof(pendingAddr);
						newFD = accept(socketFD, reinterpret_cast<struct sockaddr*>(&pendingAddr), &pendingAddrSize);
						std::cout << "newFD = " << newFD << std::endl;

						if (newFD == -1)
						{
							// std::cout << "NO WORRIES MAN" << std::endl;
						}
						else
						{
							FD_SET(newFD, &masterSet);
							if (newFD > fdMax)
								fdMax = newFD;
							std::cout << "Server : new incoming connection" << std::endl;
						}
				}

				else {

					bzero(buffer, sizeof(buffer));
					recvRet = recv(i, buffer, sizeof(buffer), 0);
					if (recvRet < 0)
					{
						std::cerr << "recv() failed : " << std::strerror(errno) << "i = " << i << std::endl;
						break; 
					}
					else if ( recvRet == 0)
					{
						std::cerr << "connection closed by the client. Bye Bye" << std::endl;
						close(i);
						FD_CLR(i, &masterSet);
						continue ;
					}
					else
					{
						std::cout << recvRet << " bytes received" << std::endl;
						std::cout << "received : " << buffer << std::endl;

					}
				}
			}
		}	
	}
	return 0;
}