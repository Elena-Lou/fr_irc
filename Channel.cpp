#include "Channel.hpp"
#include "irc.hpp"

Channel::Channel()
{
	this->_nbOfClients = 0;
}

Channel::~Channel()
{
}

Channel::Channel(std::string name) :  _nbOfClients(0), _name(name)
{
}

Channel::Channel(const Channel &source)
{
	*this = source;
}

Channel &Channel::operator=(const Channel &rhs)
{
	this->_nbOfClients = rhs._nbOfClients;
	this->_name = rhs._name;
	return (*this);
}

bool Channel::operator<(const Channel &rhs) const
{
	return (this->_name < rhs._name);
}

std::string	Channel::getName() const
{
	return (this->_name);
}

void	Channel::updateChannelName(std::string name)
{
	this->_name = name;
}

int		Channel::isUserConnected(Client& user)
{
	std::set<Client*>::iterator it = this->_connectedClients.find(&user);
	if (it != this->_connectedClients.end())
		return (CONNECTED);
	return (NOT_CONNECTED);
}

void	Channel::connectUser(Client& user)
{
	if (this->isUserConnected(user) == NOT_CONNECTED)
	{
		this->_connectedClients.insert(&user);
		this->_nbOfClients++;
		user.joinChannel(*this);
	}
}

/* returns the number of connected clients after the operation to delete chan if empty*/
int		Channel::disconnectUser(Client& user)
{
	if (this->isUserConnected(user) == CONNECTED)
	{
		this->_nbOfClients--;
		std::set<Channel*>::iterator channelIterator = user._connectedChannels.find(this);
		if (channelIterator != user._connectedChannels.end())
			user._connectedChannels.erase(channelIterator);

		std::set<Client*>::iterator clientIterator = this->_connectedClients.find(&user);
		if (clientIterator != this->_connectedClients.end())
			this->_connectedClients.erase(&user);
	}
	return (this->_nbOfClients);
}
