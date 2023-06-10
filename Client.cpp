#include "Client.hpp"
#include "Server.hpp"
#include <iomanip>

Client::Client()
{
	this->_registered = false;
	this->_passwordOK = false;
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
	this->_passwordOK = false;
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
	this->_registered = rhs._registered;
	this->_passwordOK = rhs._passwordOK;
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

void	Client::validatePassword()
{
	this->_passwordOK = true;
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

bool	Client::isPasswordOk() const
{
	return (this->_passwordOK);
}

bool	Client::isRegistered() const
{
	return (this->_registered);
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

void	Client::writeRPLToClient(Server *server, int RPL, std::string message)
{
	std::stringstream replyMessageBuilder;
	replyMessageBuilder << ":" << server->getHostname() << " " <<
		std::setw(3) << std::setfill('0') << RPL;
	if (this->_nickname != "")
		replyMessageBuilder << " " << this->_nickname;
	replyMessageBuilder << " :" << message << CRLF;
	this->writeBuffer += replyMessageBuilder.str();
}

void	Client::writeRPLToClient(Server *server, int RPL, std::string additionalSource, std::string message)
{
	std::stringstream replyMessageBuilder;
	replyMessageBuilder << ":" << server->getHostname() << " " <<
		std::setw(3) << std::setfill('0') << RPL;
	if (this->_nickname != "")
		replyMessageBuilder << " " << this->_nickname;
	replyMessageBuilder << " " << additionalSource << " :" << message << CRLF;
	this->writeBuffer += replyMessageBuilder.str();
}

void	Client::writePrivmsg(std::string source, std::string dest, std::string msg)
{
	this->writeBuffer += ":" + source + " PRIVMSG " + dest + ":" + msg + CRLF;
}
