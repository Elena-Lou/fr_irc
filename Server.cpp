#include "irc.hpp"

Server::Server()
{
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

Server::Server(const char *portNumber, std::string password)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Server char* char* constructor" << std::endl;
#endif
	int	status = 0;
	unsigned int	errorCode = 0;
	struct addrinfo hints;
	struct addrinfo	*servinfo;

	if (password == "")
		this->_restricted = false;
	else
	{
		this->_restricted = true;
		this->_password = password;
	}
	this->_hostname = "mismatched_sock(et)s_irc";
	this->_fdMax = 0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, portNumber, &hints, &servinfo)) != 0)
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
			errorCode |= ERRSOCKFD;
			continue;
		}

		/* sets the descriptor as reusable */
		int opt = 1;
		if (setsockopt(this->_socketFD, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
		{
			errorCode |= ERRSOCKOPT;
			continue;
		}

		/* sets the socket as non blocking */
		int flag = fcntl(this->_socketFD, F_GETFL, 0);
		if (fcntl(this->_socketFD, F_SETFL, flag | O_NONBLOCK) < 0)
		{
			errorCode |= ERRSOCKNOBLOCK;
			continue;
		}

		/* binds the socket descriptor to the port passed to getaddrinfo() */
		if (bind(this->_socketFD, focus->ai_addr, focus->ai_addrlen) < 0)
		{
			errorCode |= ERRSOCKBIND;
			continue;
		}
		/* listens to the port bound to the socket descriptor for incoming connections */
		if (listen(this->_socketFD, MYIRC_ALLOWED_PENDING_CONNECTIONS) < 0)
		{
			errorCode |= ERRSOCKLISTEN;
			continue;
		}
		//this could have been what we would have used if getnameinfo was allowed in the subject
		//char			hostname[HOST_BUFFER_SIZE + 1];
		//char			servname[SERV_BUFFER_SIZE + 1];
		//if ((status = getnameinfo(focus->ai_addr, focus->ai_addrlen,
				//hostname, HOST_BUFFER_SIZE, servname, SERV_BUFFER_SIZE, NI_NAMEREQD)) == 0)
		//{
			//this->_hostname = hostname;
		//}
		break;
	}
	freeaddrinfo(servinfo);
	if (errorCode != 0 || focus == NULL)
	{
		this->socketErrorHandler(errorCode);
		throw Server::CannotRetrieveSocketException();
	}
	FD_ZERO(&(this->_masterSet));
	FD_ZERO(&(this->_readingSet));
	FD_ZERO(&(this->_writingSet));
	FD_SET(this->_socketFD, &(this->_masterSet)); //add the current socket descriptor to the set
	if (this->_socketFD > this->_fdMax)
		this->_fdMax = this->_socketFD;
	this->_pendingAddrSize = sizeof(this->_pendingAddr);
	time_t timeStamp = time(NULL);
	this->_startTime = localtime(&timeStamp);
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
	this->_restricted = rhs._restricted;
	this->_password = rhs._password;
	this->_socketFD = rhs._socketFD;
	this->_fdMax = rhs._fdMax;
	this->_hostname = rhs._hostname;
	this->_startTime = rhs._startTime;
	this->_masterSet = rhs._masterSet;
	this->_readingSet = rhs._readingSet;
	this->_writingSet = rhs._writingSet;
	this->_pendingAddr = rhs._pendingAddr;
	this->_pendingAddrSize = rhs._pendingAddrSize;
	for (unsigned int i = 0; i < IRC_BUFFER_SIZE; i++)
		this->buffer[i] = rhs.buffer[i];
	this->_clients = rhs._clients;
	this->_channels = rhs._channels;
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

void	Server::socketErrorHandler(unsigned int errorBitField) const
{
	std::cout << errorBitField << std::endl;
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
	if (errorBitField & ERRNAMERESOLVE)
		std::cerr << "could not resolve name." << std::endl;
}

int	Server::getSocketFD() const
{
	return (this->_socketFD);
}

const std::string &Server::getHostname() const
{
	return (this->_hostname);
}

std::map<int, Client> &Server::getClients()
{
	return (this->_clients);
}

std::deque<Channel> & Server::getChannels( void )
{
	return this->_channels;
}

static bool	isCaseInsensitiveEqual(std::string str1, std::string str2)
{
	if (str1.size() != str2.size())
		return (false);
	for (unsigned int i = 0; i < str1.size(); i++)
	{
		if (tolower(str1[i]) != tolower(str2[i]))
			return (false);
	}
	return (true);
}

bool	Server::isUserConnected(std::string nickname) const
{
	for (std::map<int, Client>::const_iterator it = this->_clients.begin();
		it != this->_clients.end(); it++)
	{
		if (isCaseInsensitiveEqual(nickname, it->second.getNickname()))
		{
			return (true);
		}
	}
	return (false);
}

Client	*Server::getUserIfConnected(std::string nickname)
{
	for (std::map<int, Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); it++)
	{
		if (nickname == it->second.getNickname())
		{
			return (&(it->second));
		}
	}
	return (NULL);
}

bool	Server::nicknameAlreadyInUse(const Client &user, std::string nickname) const
{
	for (std::map<int, Client>::const_iterator it = this->_clients.begin();
		it != this->_clients.end(); it++)
	{
		if (it->second.getSocketFD() == user.getSocketFD())
			continue;
		if (isCaseInsensitiveEqual(nickname, it->second.getNickname()))
		{
			return (true);
		}
	}
	for (std::deque<Channel>::const_iterator it = this->_channels.begin();
			it != this->_channels.end(); it++)
	{
		if (isCaseInsensitiveEqual(nickname, it->getName()))
			return (true);
	}
	return (false);

}

void	Server::connectUser(const int socketFD)
{
	this->_clients.insert(std::make_pair(socketFD, Client(socketFD)));
}

void	Server::disconnectUser(int socketFD)
{
	for (std::map<int, Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); it++)
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

std::string	Server::getStartTime() const
{
	return (asctime(this->_startTime));
}

std::string	Server::getPassword() const
{
	return (this->_password);
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
					std::cout << "entered condition !" << std::endl;
					for (;;)
					{
						std::cout << "full buffer before: ["
							<< it->second.readBuffer << "]" << std::endl;
						std::string cmd = extractCmd(it->second.readBuffer);
						if (cmd == "")
							break;
						else
						{
							std::cout << "buffer sent to parsing : " << cmd << std::endl;
							this->parsingCommand(cmd, it->second);
						}
						std::cout << "full buffer after: ["
							<< it->second.readBuffer << "]" << std::endl;
					}
				}
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
		/* Writing loop */
		if (FD_ISSET(it->first, &(this->_writingSet))
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

void	Server::destroyChannel(Channel& chanToDelete)
{
	for (std::map<int, Client>::iterator clientIterator = this->_clients.begin();
		clientIterator != this->_clients.end(); clientIterator++)
	{
			clientIterator->second.quitChannel(chanToDelete);
	}
	for (std::deque<Channel>::iterator channelIterator = this->_channels.begin();
		channelIterator != this->_channels.end(); channelIterator++)
	{
		if (*channelIterator == chanToDelete)
		{
			this->_channels.erase(channelIterator);
			break;
		}
	}
}

void	Server::destroyChannel(std::string channelName)
{
	Channel *foundChannel;
	foundChannel = this->getChannelIfExist(channelName);
	if (foundChannel == NULL)
		return ;
	this->destroyChannel(*foundChannel);
}

Channel	*Server::getChannelIfExist(std::string chanName)
{
	for (std::deque<Channel>::iterator it = this->_channels.begin();
		it != this->_channels.end(); it++)
	{
		if (chanName == it->getName())
			return (&(*it));
	}
	return (NULL);
}

void	Server::broadcastAllClients(std::string message)
{
	for (std::map<int, Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); it++)
	{
		it->second.writeToClient(message);
	}
}

void	Server::broadcastChannel(Channel& targetChannel, std::string message)
{
	targetChannel.broadcastToChannel(message);
}

bool Server::checkRawInput( std::string & rawInput ) const
{
	std::size_t pos = rawInput.find(CRLF);
	if (pos != std::string::npos)
	{
		return (true);
	}
	return (false);
}

std::string	Server::extractCmd(std::string &rawInput)
{
	std::size_t pos = rawInput.find(CRLF);
	if (pos != std::string::npos)
	{
		std::string cmd(rawInput, 0, pos);
		rawInput.erase(0, pos + 2);
		return (cmd);
	}
	return ("");
}

void Server::parsingCommand( std::string & rawInput, Client & user )
{
	std::cout << "parsingCommand - rawInput : [" << rawInput << "]" << std::endl;

	int index = -1;
	std::string rawCommand;

	for (size_t i = 0; i < rawInput.size(); i++)
	{
		if (isspace(rawInput[i]))
		{
			rawCommand = rawInput.substr(0, i);
			break;
		}
	}
	if (rawCommand == "")
		rawCommand = rawInput;

	std::string commands[] = { "KICK",
								"JOIN",
								"INVITE",
								"USER",
								"OPER",
								"NICK",
								"TOPIC",
								"MODE",
								"PRIVMSG",
								"CAP",
								"PASS"
							};

	for (int i = 0; i < 11; i++)
	{
		if (commands[i] == rawCommand)
		{
			index = i;
			break ;
		}
	}
	switch (index)
	{
		case 0 :
		{
			Kick(*this, user, rawInput);
			break;
		}
		case 1 :
		{
			Join(*this, user, rawInput);
			break;
		}
		case 2 :
		{
			std::cout << "need to create INVITE command" << std::endl;
			break;
		}
		case 3 :
		{
			User(*this, user, rawInput);
			break;
		}
		case 4 :
		{
			std::cout << "need to create OPER command" << std::endl;
			break;
		}
		case 5 :
		{
			Nick(*this, user, rawInput);
			break;
		}
		case 6 :
		{
			std::cout << "need to create TOPIC command" << std::endl;
			break;
		}
		case 7 :
		{
			std::cout << "need to create MODE command" << std::endl;
			break;
		}
		case 8 :
		{
			std::cout << "need to create PRIVMSG command" << std::endl;
			break;
		}
		case 9 :
		{
			std::cout << "CAP recognised, continuing" << std::endl;
			break;
		}
		case 10 :
		{
			Pass(*this, user, rawInput);
			break;
		}
		default:
		{
			std::cerr << "invalid command" << std::endl;
			break;
		}
	}
	rawInput.clear();
}
