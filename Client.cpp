#include "Client.hpp"

Client::Client()
{
	//this is prohibited
}

Client::~Client()
{
	std::cout << "Channel Client start" << std::endl;
	//close(this->_socketFD);
	std::cout << "Channel Client end" << std::endl;
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
