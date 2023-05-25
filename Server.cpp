#include "Server.hpp"

Server::Server()
{
	int				status = 0;
	struct addrinfo hints;
	struct addrinfo	*servinfo;

	this->_fdMax = 0;
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
	FD_ZERO(&(this->_masterSet));
	FD_ZERO(&(this->_readingSet));
	FD_ZERO(&(this->_writingSet));
	FD_SET(this->_socketFD, &(this->_masterSet)); //add the current socket descriptor to the set
	if (this->_socketFD > this->_fdMax)
		this->_fdMax = this->_socketFD;
	this->_pendingAddrSize = sizeof(this->_pendingAddr);
}

Server::~Server()
{
	for (std::set<Client*>::iterator it = this->_clients.begin();
			it != this->_clients.end();)
	{
		std::set<Client*>::iterator tmp = it;
		it++;
		delete *tmp;
	}
	close(this->_socketFD);
	freeaddrinfo(this->_servinfo);
}

Server::Server(const char *portNumber, const char *domain)
{
	int				status = 0;
	struct addrinfo hints;
	struct addrinfo	*servinfo;

	this->_fdMax = 0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(domain, portNumber, &hints, &servinfo)) != 0)
	{
		std::cerr << gai_strerror(status) << std::endl;
		throw Server::CannotRetrieveAddrinfoException();
	}
	struct addrinfo *focus;
	for (focus = servinfo; focus != NULL; focus = focus->ai_next)
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
		break;
	}
	if (status != 0 || focus == NULL)
	{
		this->socketErrorHandler(status);
		throw Server::CannotRetrieveSocketException();
	}
	FD_ZERO(&(this->_masterSet));
	FD_ZERO(&(this->_readingSet));
	FD_ZERO(&(this->_writingSet));
	FD_SET(this->_socketFD, &(this->_masterSet)); //add the current socket descriptor to the set
	if (this->_socketFD > this->_fdMax)
		this->_fdMax = this->_socketFD;
	this->_pendingAddrSize = sizeof(this->_pendingAddr);
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
		std::cerr << "multiple error encountered:" << std::endl;
	if (errorBitField & ERRSOCKFD)
		std::cerr << "socket() failed." << std::endl;
	if (errorBitField & ERRSOCKOPT)
		std::cerr << "setsockopt() failed." << std::endl;
	if (errorBitField & ERRSOCKNOBLOCK)
		std::cerr << "fcntl() failed." << std::endl;
	if (errorBitField & ERRSOCKBIND)
		std::cerr << "bind() failed." << std::endl;
	if (errorBitField & ERRSOCKLISTEN)
		std::cerr << "listen() failed." << std::endl;
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

std::set<Client*> &Server::getClients()
{
	return (this->_clients);
}

void	Server::addUser(int socketFD)
{
	this->_clients.insert(new Client(socketFD));
}

void	Server::deleteUser(int socketFD)
{
	for (std::set<Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if ((*it)->getSocketFD() == socketFD)
		{
			this->_clients.erase(it);
			break;
		}
	}
}

int		Server::fillSets()
{
	/* inits timeval struct to shut down after MYIRC_TIMEOUT seconds of inactivity */
	struct timeval	timeOut;
	timeOut.tv_sec = MYIRC_TIMEOUT;
	timeOut.tv_usec = 0;

	int selectRet = 0;
	memcpy(&(this->_readingSet), &(this->_masterSet), sizeof(this->_masterSet));
	memcpy(&(this->_writingSet), &(this->_masterSet), sizeof(this->_masterSet));

	selectRet = select(this->_fdMax + 1, &(this->_readingSet), &(this->_writingSet), NULL, &timeOut);
	if (selectRet < 0)
	{
		std::cerr << "select() failed :  " << std::strerror(errno) << std::endl;
		return (1);
	}
	else if (selectRet == 0)
	{
		std::cerr << "select() timed out. Bye Bye" << std::endl;
		return (1);
	}
	return (0);
}

void	Server::checkNewConnections()
{
	int newFD = accept(this->_socketFD,
			reinterpret_cast<struct sockaddr*>(&(this->_pendingAddr)),
			&(this->_pendingAddrSize));
	if (newFD == -1) {
		// std::cout << "NO WORRIES MAN" << std::endl;
	}
	else
	{
		this->addUser(newFD);
		FD_SET(newFD, &(this->_masterSet));
		if (newFD > this->_fdMax)
			this->_fdMax = newFD;
	}
}

void	Server::readLoop()
{
	for (std::set<Client*>::iterator it = this->_clients.begin();
					it != this->_clients.end();)
	{
		/* Reading loop */
		if (FD_ISSET((*it)->getSocketFD(), &(this->_readingSet)))
		{
			bzero(this->buffer, sizeof(char) * IRC_BUFFER_SIZE - 1);
			int recvRet = recv((*it)->getSocketFD(), this->buffer, sizeof(char) * (IRC_BUFFER_SIZE - 1), 0);
			if (recvRet < 0)
			{
				std::cerr << "recv() failed : " << std::strerror(errno) << "fd = " << (*it)->getSocketFD() << std::endl;
				std::set<Client*>::iterator tmp = it;
				it++;
				this->_clients.erase(tmp);
				break;
			}
			else if ( recvRet == 0)
			{
				std::set<Client*>::iterator tmp = it;
				std::cerr << "connection closed by the client. Bye Bye" << std::endl;
				FD_CLR((*it)->getSocketFD(), &(this->_masterSet));
				it++;
				this->_clients.erase(tmp);
				continue ;
			}
			else
			{
				std::cout << recvRet << " bytes received" << std::endl;
				std::cout << "received : " << this->buffer << std::endl;
				(*it)->readBuffer.append(this->buffer);
			}
		}
		it++;
	}
}

void	Server::writeLoop()
{
	for (std::set<Client*>::iterator it = this->_clients.begin();
			it != this->_clients.end();)
	{
		/* Reading loop */
		if (FD_ISSET((*it)->getSocketFD(), &(this->_readingSet))
			&& (*it)->writeBuffer.size())
		{
			//bzero((*it)->buffer, sizeof(char) * IRC_BUFFER_SIZE);
			int sendRet = send((*it)->getSocketFD(), (*it)->writeBuffer.c_str(),
				sizeof(char) * (*it)->writeBuffer.size(), MSG_NOSIGNAL);
			if (sendRet < 0)
			{
				std::set<Client*>::iterator tmp = it;
				std::cerr << "send() failed : " << std::strerror(errno) << "fd = " << (*it)->getSocketFD() << std::endl;
				FD_CLR((*it)->getSocketFD(), &(this->_masterSet));
				this->_clients.erase(tmp);
				break;
			}
			else
			{
				if (static_cast<size_t>(sendRet) == (*it)->writeBuffer.size())
					(*it)->writeBuffer.erase();
				else
					(*it)->writeBuffer.erase((*it)->writeBuffer.begin() + sendRet);
				std::cout << sendRet << " bytes sent" << std::endl;
				std::cout << "sent : " << (*it)->writeBuffer << std::endl;
			}
		}
		it++;
	}
}
