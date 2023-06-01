#include "Client.hpp"

Client::Client()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Client default constructor" << std::endl;
#endif
	//this is prohibited
}

Client::~Client()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Client destructor" << std::endl;
#endif
}

Client::Client(int socketFD) : _socketFD(socketFD)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Client socketFD constructor" << std::endl;
#endif
}

Client::Client(const Client &source)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Client copy constructor" << std::endl;
#endif
	*this = source;
}

Client& Client::operator=(const Client &rhs)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Client = overload" << std::endl;
#endif
	this->_socketFD = rhs._socketFD;
	this->_username = rhs._username;
	return (*this);
}

void	Client::setUsername(std::string name)
{
	this->_username = name;
}

int	Client::getSocketFD() const
{
	return (this->_socketFD);
}

std::string Client::getUsername() const
{
	return (this->_username);
}

const std::set<Channel*> &Client::getConnectedChannels() const
{
	return (this->_connectedChannels);
}
int	Client::isInChannel(Channel& toFind) const
{
	if (this->_connectedChannels.find(&toFind) != this->_connectedChannels.end())
		return (CONNECTED);
	return (NOT_CONNECTED);
}

void	Client::joinChannel(Channel& channelToJoin)
{
	this->_connectedChannels.insert(&channelToJoin);
}

void	Client::quitChannel(Channel& channelToQuit)
{
	this->_connectedChannels.erase(&channelToQuit);
}
