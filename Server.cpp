#include "irc.hpp"

Server::Server()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Server Default constructor" << std::endl;
#endif
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
			continue;
		}

		/* sets the socket as non blocking */
		int flag = fcntl(this->_socketFD, F_GETFL, 0);
		if (fcntl(this->_socketFD, F_SETFL, flag | O_NONBLOCK) < 0)
		{
			status |= 0b100;
			continue;
		}

		/* binds the socket descriptor to the port passed to getaddrinfo() */
		if (bind(this->_socketFD, focus->ai_addr, focus->ai_addrlen) < 0)
		{
			status |= 0b1000;
			continue;
		}
		/* listens to the port bound to the socket descriptor for incoming connections */
		if (listen(this->_socketFD, MYIRC_ALLOWED_PENDING_CONNECTIONS) < 0)
		{
			status |= 0b10000;
			continue;
		}
	}
	freeaddrinfo(servinfo);
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
#if SHOW_CONSTRUCTOR
	std::cout << "Server destructor" << std::endl;
#endif
	for (std::map<int, Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); it++)
		close(it->second.getSocketFD());
	close(this->_socketFD);
}

Server::Server(const char *portNumber, const char *domain)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Server char* char* constructor" << std::endl;
#endif
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
			continue;
		}

		/* sets the socket as non blocking */
		int flag = fcntl(this->_socketFD, F_GETFL, 0);
		if (fcntl(this->_socketFD, F_SETFL, flag | O_NONBLOCK) < 0)
		{
			status |= 0b100;
			continue;
		}

		/* binds the socket descriptor to the port passed to getaddrinfo() */
		if (bind(this->_socketFD, focus->ai_addr, focus->ai_addrlen) < 0)
		{
			status |= 0b1000;
			continue;
		}
		/* listens to the port bound to the socket descriptor for incoming connections */
		if (listen(this->_socketFD, MYIRC_ALLOWED_PENDING_CONNECTIONS) < 0)
		{
			status |= 0b10000;
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);
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
#if SHOW_CONSTRUCTOR
	std::cout << "Server copy constructor" << std::endl;
#endif
	*this = source;
}

Server& Server::operator=(const Server &rhs)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Server = overload" << std::endl;
#endif
	this->_socketFD = rhs._socketFD;
	this->_fdMax = rhs._fdMax;
	return (*this);
}

const char *Server::CannotRetrieveSocketException::what(void) const throw()
{
	return ("cannot retrieve socket");
}

const char *Server::CannotRetrieveAddrinfoException::what(void) const throw()
{
	return ("cannot retrieve AddrInfo");
}

const char *Server::InterruptionSignalException::what(void) const throw()
{
	return ("Program Interrupted");
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

std::map<int, Client> &Server::getClients()
{
	return (this->_clients);
}

void	Server::connectUser(const int socketFD)
{
	this->_clients.insert(std::make_pair(socketFD, Client(socketFD)));
}

void	Server::disconnectUser(int socketFD)
{
	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (it->first == socketFD)
		{
			this->disconnectUser(it);
			break;
		}
	}
}

void	Server::disconnectUser(std::map<int, Client>::iterator clientIterator)
{
	for (std::deque<Channel>::iterator channelIterator = this->_channels.begin();
		channelIterator != this->_channels.end(); channelIterator++)
	{
		if (channelIterator->removeUserFromChannel(clientIterator->second) == 0)
			this->_channels.erase(channelIterator);
	}
	close(clientIterator->second.getSocketFD());
	this->_clients.erase(clientIterator);
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
		this->connectUser(newFD);
		FD_SET(newFD, &(this->_masterSet));
		if (newFD > this->_fdMax)
			this->_fdMax = newFD;
	}
}

void	Server::readLoop()
{
	for (std::map<int, Client>::iterator it = this->_clients.begin();
					it != this->_clients.end();)
	{
		/* Reading loop */
		if (FD_ISSET(it->second.getSocketFD(), &(this->_readingSet)))
		{
			bzero(this->buffer, sizeof(char) * IRC_BUFFER_SIZE - 1);
			int recvRet = recv(it->second.getSocketFD(), this->buffer, sizeof(char) * (IRC_BUFFER_SIZE - 1), 0);
			if (recvRet < 0)
			{
				std::cerr << "recv() failed : " << std::strerror(errno) << "fd = " << it->first << std::endl;
				std::map<int, Client>::iterator disconnectedClient = it;
				it++;
				this->disconnectUser(disconnectedClient);
				break;
			}
			else if ( recvRet == 0)
			{
				std::map<int, Client>::iterator disconnectedClient = it;
				std::cerr << "connection closed by the client. Bye Bye" << std::endl;
				FD_CLR(it->second.getSocketFD(), &(this->_masterSet));
				it++;
				this->disconnectUser(disconnectedClient);
				continue ;
			}
			else
			{
				std::cout << recvRet << " bytes received" << std::endl;
				std::cout << "received : " << this->buffer << std::endl;
				it->second.readBuffer.append(this->buffer);
				if (this->checkRawInput(it->second.readBuffer))
				{
					std::cout << "buffer sent to parsing : " << it->second.readBuffer << std::endl;
					this->parsingCommand(it->second.readBuffer);
				}
				std::cout << "buffer after parsing : " << it->second.readBuffer << std::endl;

			}
		}
		it++;
	}
}

void	Server::writeLoop()
{
	for (std::map<int, Client>::iterator it = this->_clients.begin();
			it != this->_clients.end();)
	{
		/* Reading loop */
		if (FD_ISSET(it->first, &(this->_readingSet))
			&& it->second.writeBuffer.size())
		{
			//bzero((*it)->buffer, sizeof(char) * IRC_BUFFER_SIZE);
			int sendRet = send(it->first, it->second.writeBuffer.c_str(),
				sizeof(char) * it->second.writeBuffer.size(), MSG_NOSIGNAL);
			if (sendRet < 0)
			{
				std::map<int, Client>::iterator tmp = it;
				std::cerr << "send() failed : " << std::strerror(errno) << "fd = "
					<< it->first << std::endl;
				FD_CLR(it->first, &(this->_masterSet));
				this->_clients.erase(tmp);
				break;
			}
			else
			{
				if (static_cast<size_t>(sendRet) == it->second.writeBuffer.size())
					it->second.writeBuffer.erase();
				else
					it->second.writeBuffer.erase(it->second.writeBuffer.begin() + sendRet);
				std::cout << sendRet << " bytes sent" << std::endl;
				std::cout << "sent : " << it->second.writeBuffer << std::endl;
			}
		}
		it++;
	}
}

void	Server::createChannel(std::string newChannelName, Client& owner)
{
	int	channelAlreadyExists = 0;
	for (std::deque<Channel>::iterator it = this->_channels.begin();
			it != this->_channels.end(); it++)
	{
		if (it->getName() == newChannelName)
			channelAlreadyExists = 1;
	}
	if (channelAlreadyExists == 0)
	{
		this->_channels.push_front(Channel(newChannelName, owner));
	}
}

void	Server::destroyChannel(const Channel& chanToDelete)
{
	(void)chanToDelete;
	//supprimer le chan dans la liste de chaque personne connectée
	//supprimer le chan dans Server
}

void	Server::destroyChannel(std::string channelName)
{
	for (std::deque<Channel>::iterator it = this->_channels.begin();
		it != this->_channels.end(); it++)
	{
		if (it->getName() == channelName)
		{
			this->destroyChannel(*it);
			break;
		}
	}
	//trouver le channel dans la liste de Server
	//appeler destroychannel(Channel&)
}

void	Server::broadcastAllClients(std::string &message)
{
	for (std::map<int, Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); it++)
	{
		it->second.writeBuffer += message;
	}
}

void	Server::broadcastChannel(Channel& targetChannel, std::string &message)
{
	targetChannel.broadcastAllClients(message);
}

bool Server::checkRawInput( std::string & rawInput )
{
	std::size_t it = rawInput.find("\r\n");
	if (it != std::string::npos)
	{
		rawInput.erase(rawInput.begin() + it, rawInput.end());
		return true;
	}
	return false;
}

void Server::parsingCommand( std::string & rawInput )
{
	int index = 3;
	ACommand* newCmd;

	std::cout << "parsingCommand - rawInput : [" << rawInput << "]" << std::endl;
	std::string commands[] = { "KICK", "JOIN" };

	for (int i = 0; i < 2; i++)
	{
		std::cout << "commands[i] : " << commands[i] << std::endl;
		if (commands[i] == rawInput)
		{
			std::cout << "index = " << index << std::endl;
			index = i;
			std::cout << "index = " << index << std::endl;
			break ;
		}
	}
	switch (index)
	{
	case 0 :
		newCmd = new Kick();
		rawInput.clear();
		delete newCmd;
		break;
	
	case 1 :
		std::cout << "need to create a JOIN command" << std::endl;
		rawInput.clear();
		break;
	
	default:
		std::cout << "not a known command" << std::endl;
		rawInput.clear();
		break;
	}


}
