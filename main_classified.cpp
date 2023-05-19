#include "irc.hpp"

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

	int newFD; // clone socket created after accept()
	Server	myIrc("3490", NULL);
	/* listens to the port bound to the socket descriptor for incoming connections */
	if (listen(myIrc.getSocketFD(), 5) < 0)
	{
		std::cerr << "listen() failed : " << std::strerror(errno) << std::endl;
		exit(1);
	}

	/* setting up structures needed for select */
	struct timeval	timeOut;
	fd_set	masterSet;
	fd_set	workingSet; // select erase the list given so we need to save it in a tmp set
	int				fdMax; //biggest  fd
	int				selectRet; //saves select() return value
	int				recvRet; // saves recv() return value
	char			buffer[80];

	FD_ZERO(&masterSet); // initialise the list of fd to 0
	FD_ZERO(&workingSet); // same for the working one

	FD_SET(myIrc.getSocketFD(), &masterSet); //add the current socket descriptor to the set

	fdMax = myIrc.getSocketFD();

	/* initialises the timeval struct to 2 min to shut down after inactivity */
	timeOut.tv_sec = 120;
	timeOut.tv_usec = 0;

	/* WAITING FOR CONNECTIONS LOOP */
	while(1)
	{
		memcpy(&workingSet, &masterSet, sizeof(masterSet)); //

		std::cout << "Waiting for connections..." << std::endl;
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
		for (int i = 0; i <= fdMax && selectRet > 0; i++)
		{
			if (FD_ISSET(i, &workingSet))
			{
				selectRet--; //One less fd to investigate within the set
				if (i == myIrc.getSocketFD())
				{
					std::cout << "Listening socket is readable" << std::endl;
					do
					{
						newFD = accept(myIrc.getSocketFD(), NULL, NULL);
						if (newFD < 0)
						{
							std::cerr << "accept() failed : " << std::strerror(errno) << std::endl;
							break ;
						}
						else
						{
							FD_SET(newFD, &masterSet);
							if (newFD < fdMax)
								fdMax = newFD;
							std::cout << "Server : new incoming connection" << std::endl;
						}
					} while (newFD != -1);
				}
			}
			else //not the listening socket 
			{
				while (1)
				{
					recvRet = recv(i, buffer, sizeof(buffer), 0);
					if (recvRet < 0)
					{
						std::cerr << "recv() failed : " << std::strerror(errno) << std::endl;
						break ;
					}
					else if ( recvRet == 0)
					{
						std::cerr << "connection closed by the client. Bye Bye" << std::endl;
						break ;
					}
					else
					{
						std::cout << recvRet << " bytes received" << std::endl;
					}
				}
			}
		}
	}
}
