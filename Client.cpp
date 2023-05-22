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

void	Client::closeSocketFD()
{
}

void	Client::setUsername(std::string name)
{
	this->_username = name;
}
