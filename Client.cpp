#include "Client.hpp"

Client::Client()
{
	//this is prohibited
}

Client::~Client()
{
	close(this->_socketFD);
}

Client::Client(int socketFD) : _socketFD(socketFD)
{
}

Client::Client(const Client &source)
{
	*this = source;
}

Client& Client::operator=(const Client &rhs)
{
	this->_socketFD = rhs._socketFD;
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

int	Client::isInChannel(Channel& toFind) const
{
	if (this->_connectedChannels.find(&toFind) != this->_connectedChannels.end())
		return (CONNECTED);
	return (NOT_CONNECTED);
}

void	Client::joinChannel(Channel& toJoin)
{
	this->_connectedChannels.insert(&toJoin);
}
