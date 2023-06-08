#include "Client.hpp"
#include "Server.hpp"

Client::Client()
{
	this->_registered = false;
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
	this->_registered = 0;
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
	this->_nickname = rhs._nickname;
	return (*this);
}

void	Client::setNickname(std::string name)
{
	this->_nickname = name;
}

void	Client::setUsername(std::string name)
{
	this->_username = name;
}

void	Client::setHostname(std::string name)
{
	this->_hostname = name;
}

void	Client::setRealname(std::string name)
{
	this->_realname = name;
}

void	Client::confirmRegistration()
{
	this->_registered = true;
}

int	Client::getSocketFD() const
{
	return (this->_socketFD);
}

std::string Client::getNickname() const
{
	return (this->_nickname);
}

std::string Client::getFullName() const
{
	return (this->_nickname + "!" + this->_username + "@" + this->_hostname);
}

std::string	Client::getUsername() const
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

void	Client::writeToClient(std::string message)
{
	this->writeBuffer += message + CRLF;
}

void	Client::writeRPLToClient(Server *server, std::string RPL, std::string message)
{
	std::stringstream replyMessageBuilder;
	replyMessageBuilder << ":" << server->getHostname() << " "
		<< RPL << " " << this->_nickname << " :" << message << CRLF;
	this->writeBuffer += replyMessageBuilder.str();
}
