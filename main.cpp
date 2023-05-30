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

void sigIntHandler(int signal)
{
	(void)signal;
	throw Server::InterruptionSignalException();
}

int main( int ac, char** av ) {

	// if (ac != 2)
	// {
	// 	std::cerr << "usage: ./ircserv hostname" << std::endl;
	// 	return 1;
	// }


	(void)ac;
	(void)av;

	try
	{
		std::signal(SIGINT, sigIntHandler);

		Server	myIrc("3490", NULL);
		std::cout << "entering the while loop" << std::endl;
		/* WAITING FOR CONNECTIONS LOOP */
		while(1)
		{
			if (myIrc.fillSets())
				break;
			/* Check socket for new connections */
			// std::cout << "Waiting for connections..." << std::endl;
			myIrc.checkNewConnections();
			myIrc.readLoop();
			myIrc.writeLoop();
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "Abort: " << e.what() << std::endl;
	}
	return 0;
}