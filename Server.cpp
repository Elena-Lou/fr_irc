#include "Server.hpp"

Server::Server()
{
	int				status = 0;
	struct addrinfo hints;
	struct addrinfo	*servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, MYIRC_PORT,  &hints, &servinfo)) != 0)
	{
		std::cerr << gai_strerror(status) << std::endl;
		throw Server::CannotRetrieveAddrinfoException();
	}
	for (struct addrinfo *focus = servinfo; focus != NULL; focus = focus->ai_next)
	{
		/* retrieving a socket file descriptor */
		this->_socketFD = socket(focus->ai_family, focus->ai_socktype,
				focus->ai_protocol);
		if (this->_socketFD < 0)
		{
			status |= 1;
			continue;
		}

		/* sets the descriptor as reusable */
		int opt = 1;
		if (setsockopt(this->_socketFD, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
		{
			status |= 0b10;
			close(this->_socketFD);
			continue;
		}

		/* sets the socket as non blocking */
		int flag = fcntl(this->_socketFD, F_GETFL, 0);
		if (fcntl(this->_socketFD, F_SETFL, flag | O_NONBLOCK) < 0)
		{
			status |= 0b100;
			close(this->_socketFD);
			continue;
		}

		/* binds the socket descriptor to the port passed to getaddrinfo() */
		if (bind(this->_socketFD, focus->ai_addr, focus->ai_addrlen) < 0)
		{
			status |= 0b1000;
			close(this->_socketFD);
			continue;
		}
		/* listens to the port bound to the socket descriptor for incoming connections */
		if (listen(this->_socketFD, MYIRC_ALLOWED_PENDING_CONNECTIONS) < 0)
		{
			status |= 0b10000;
			close(this->_socketFD);
			continue;
		}
	}
	if (status != 0)
	{
		this->socketErrorHandler(status);
		throw Server::CannotRetrieveSocketException();
	}
}

Server::~Server()
{
	close(this->_socketFD);
	freeaddrinfo(this->_servinfo);
}

Server::Server(const char *portNumber, const char *domain)
{
	int				status = 0;
	struct addrinfo hints;
	struct addrinfo	*servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(domain, portNumber, &hints, &servinfo)) != 0)
	{
		std::cerr << gai_strerror(status) << std::endl;
		throw Server::CannotRetrieveAddrinfoException();
	}
	for (struct addrinfo *focus = servinfo; focus != NULL; focus = focus->ai_next)
	{
		/* retrieving a socket file descriptor */
		this->_socketFD = socket(focus->ai_family, focus->ai_socktype,
				focus->ai_protocol);
		if (this->_socketFD < 0)
		{
			status |= 1;
			continue;
		}

		/* sets the descriptor as reusable */
		int opt = 1;
		if (setsockopt(this->_socketFD, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
		{
			status |= 0b10;
			close(this->_socketFD);
			continue;
		}

		/* sets the socket as non blocking */
		int flag = fcntl(this->_socketFD, F_GETFL, 0);
		if (fcntl(this->_socketFD, F_SETFL, flag | O_NONBLOCK) < 0)
		{
			status |= 0b100;
			close(this->_socketFD);
			continue;
		}

		/* binds the socket descriptor to the port passed to getaddrinfo() */
		if (bind(this->_socketFD, focus->ai_addr, focus->ai_addrlen) < 0)
		{
			status |= 0b1000;
			close(this->_socketFD);
			continue;
		}
		/* listens to the port bound to the socket descriptor for incoming connections */
		if (listen(this->_socketFD, MYIRC_ALLOWED_PENDING_CONNECTIONS) < 0)
		{
			status |= 0b10000;
			close(this->_socketFD);
			continue;
		}
	}
	if (status != 0)
	{
		this->socketErrorHandler(status);
		throw Server::CannotRetrieveSocketException();
	}
}

Server::Server(const Server &source)
{
	*this = source;
}

Server& Server::operator=(const Server &rhs)
{
	this->_socketFD = rhs._socketFD;
	this->_servinfo = rhs._servinfo;
	return (*this);
}

const char *Server::CannotRetrieveSocketException::what(void) const throw()
{
	return ("Aborting\n");
}

const char *Server::CannotRetrieveAddrinfoException::what(void) const throw()
{
	return ("Aborting\n");
}

void	Server::socketErrorHandler(int errorBitField) const
{
	if (errorBitField & (errorBitField - 1))
		std::cout << "multiple error encountered:" << std::endl;
	if (errorBitField & ERRSOCKFD)
		std::cout << "socket() failed." << std::endl;
	if (errorBitField & ERRSOCKOPT)
		std::cout << "setsockopt() failed." << std::endl;
	if (errorBitField & ERRSOCKNOBLOCK)
		std::cout << "fcntl() failed." << std::endl;
	if (errorBitField & ERRSOCKBIND)
		std::cout << "bind() failed." << std::endl;
	if (errorBitField & ERRSOCKLISTEN)
		std::cout << "listen() failed." << std::endl;
}

int	Server::getSocketFD() const
{
	return (this->_socketFD);
}


struct addrinfo *Server::getServinfo() const
{
	return (this->_servinfo);
}

struct sockaddr *Server::getSockaddr() const
{
	return (this->_servinfo->ai_addr);
}

void	Server::startListening() const
{
	if (listen(this->_socketFD, 5) < 0)
	{
		std::cerr << "listen() failed : " << std::strerror(errno) << std::endl;
		exit(1);
	}
}

