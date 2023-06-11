#include "Channel.hpp"
#include "irc.hpp"
#include "Server.hpp"

Channel::Channel()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Channel default constructor" << std::endl;
#endif
	//this is prohibited
}

Channel::~Channel()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Channel default destructor" << std::endl;
#endif
}

Channel::Channel(std::string name, Client& owner) :  _nbOfClients(0), _name(name)
{
	this->setOperator(owner);
	this->_protected = false;
	this->_topicProtected = false;
#if SHOW_CONSTRUCTOR
	std::cout << "Channel string Client constructor" << std::endl;
#endif
}

Channel::Channel(const Channel &source)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Channel copy constructor" << std::endl;
#endif
	*this = source;
}

Channel &Channel::operator=(const Channel &rhs)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Channel = overload" << std::endl;
#endif
	this->_chanOps = rhs._chanOps;
	this->_nbOfClients = rhs._nbOfClients;
	this->_name = rhs._name;
	this->_protected = rhs._protected;
	this->_password = rhs._password;
	this->_topic = rhs._topic;
	this->_topicProtected = rhs._topicProtected;
	return (*this);
}

bool Channel::operator<(const Channel &rhs) const
{
	return (this->_name < rhs._name);
}

bool Channel::operator==(const Channel &rhs) const
{
	return (this->_name == rhs._name);
}

std::string	Channel::getName() const
{
	return (this->_name);
}

std::string Channel::getTopic() const
{
	return (this->_topic);
}

void	Channel::updateChannelName(std::string name)
{
	this->_name = name;
}

bool		Channel::isUserConnected(Client& user)
{
	std::map<int, Client*>::iterator it = this->_connectedClients.find(user.getSocketFD());
	if (it != this->_connectedClients.end())
		return (CONNECTED);
	return (NOT_CONNECTED);
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


bool Channel::isUserConnected(std::string nickName)
{
	std::map<int, Client*>::iterator it;
	for (it = this->_connectedClients.begin();
		it != this->_connectedClients.end(); it++)
	{
		if (isCaseInsensitiveEqual(it->second->getNickname(), nickName))
			return true;
	}
	return false;
}

bool	Channel::isProtected() const
{
	return (this->_protected);
}

Client	*Channel::getUserIfConnected(std::string nickname)
{
	for (std::map<int, Client*>::iterator it = this->_connectedClients.begin();
		it != this->_connectedClients.end(); it++)
	{
		if (nickname == it->second->getNickname())
			return (it->second);
	}
	return (NULL);
}

void	Channel::addUserToChannel(Client& user)
{
	if (this->isUserConnected(user) == NOT_CONNECTED)
	{
		this->_connectedClients.insert(std::make_pair(user.getSocketFD(), &user));
		this->_nbOfClients++;
		user.joinChannel(*this);
	}
}

/* returns the number of connected clients after the operation to delete chan if empty*/
int		Channel::removeUserFromChannel(Client& user)
{
	if (this->isUserConnected(user) == CONNECTED)
	{
		this->_nbOfClients--;
		user.quitChannel(*this);

		std::map<int, Client*>::iterator clientIterator = this->_connectedClients.find(user.getSocketFD());
		if (clientIterator != this->_connectedClients.end())
			this->_connectedClients.erase(clientIterator);
	}
	return (this->_nbOfClients);
}

bool Channel::isChannelOperator(Client & user)
{
	std::map<int, Client*>::iterator it = this->_chanOps.find(user.getSocketFD());
	if (it != this->_chanOps.end())
		return (true);
	else
		return (false);
}

void	Channel::setOperator(Client &chanOp)
{
	this->_chanOps.insert(std::make_pair(chanOp.getSocketFD(), &chanOp));
}

void	Channel::removeOperator(Client &chanOp)
{
	this->_chanOps.erase(chanOp.getSocketFD());
}


void	Channel::broadcastToChannel(std::string message)
{
	for (std::map<int, Client*>::iterator it = this->_connectedClients.begin();
		it != this->_connectedClients.end(); it++)
	{
		it->second->writeToClient(message);
	}
}

void	Channel::writePrivmsg(std::string source, std::string dest, std::string msg)
{
	for (std::map<int, Client*>::iterator it = this->_connectedClients.begin();
		it != this->_connectedClients.end(); it++)
	{
		if (source != dest && it->second->getNickname() == source)
			continue;
		it->second->writePrivmsg(source, dest, msg);
	}
}

void	Channel::changePassword(std::string password)
{
	this->_protected = true;
	this->_password = password;
}

bool	Channel::tryPassword(std::string password)
{
	return (this->_password == password);
}

bool	Channel::isTopicProtected()
{
	return (this->_topicProtected);
}

void	Channel::sendAllNamesToUser(Server &server, Client &user)
{
	std::string symbol("= ");
	//symbol can be:
	//	"= " for public
	//	"@ " for secret
	//	"* " for private

	for (std::map<int, Client*>::iterator it = this->_connectedClients.begin();
		it != this->_connectedClients.end(); it++)
	{
		std::string prefix;
		if (this->_chanOps.find(it->first) != this->_chanOps.end())
			prefix = "@ ";
		std::string name = prefix + it->second->getNickname();
		user.writeRPLToClient(&server, RPL_NAMREPLY,
			symbol + this->_name, prefix + it->second->getNickname());
	}
}

void	Channel::updateTopic(Client &author, std::string topic)
{
	this->_topic = topic;
	std::stringstream fullMessage;
	fullMessage << ":" << author.getFullName() << " TOPIC " << this->_name
		<< " :" << topic;
	this->broadcastToChannel(fullMessage.str());
}
