
#include "Topic.hpp"
#include <vector>

Topic::Topic() : ACommand()
{
	/*this is forbidden */
}

Topic::~Topic()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Topic destructor" << std::endl;
#endif
}

Topic::Topic(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Topic full constructor" << std::endl;
#endif
	if (this->_cmd.size() >= 3)
	{
		unsigned int offset = rawInput.find(":");
		if (offset < rawInput.size())
			offset++;
		this->_message = std::string(rawInput, offset);
	}
	if (this->_cmd.size() >= 2)
		this->_target = this->_server->getChannelIfExist(this->_cmd[1]);
	this->execute();
}

Topic::Topic(const Topic &source) : ACommand(source)
{
	*this = source;
}

Topic &Topic::operator=(const Topic &rhs)
{
	this->_cmd = rhs._cmd;
	this->_server = rhs._server;
	this->_author = rhs._author;
	return (*this);
}

void Topic::execute()
{
	if (!this->_author->isRegistered())
	{
		std::cout << "User is not registered" << std::endl;
		return ;
	}
	if (this->_cmd.size() < 2)
	{
		error(ERR_NEEDMOREPARAMS);
		return;
	}
	if (this->_target == NULL)
	{
		error(ERR_NOSUCHCHANNEL);
		return;
	}
	if (this->_target && !this->_target->isUserConnected(*this->_author))
	{
		error(ERR_NOTONCHANNEL);
		return;
	}
	if (this->_target->isTopicProtected() && !this->_target->isChannelOperator(*this->_author))
	{
		error(ERR_CHANOPRIVSNEEDED);
		return;
	}
	this->confirm();
}

void Topic::error(int errorCode) const
{
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_NEEDMOREPARAMS, this->_cmd[0], MSG_NEEDMOREPARAMS);
			return;
		}
		case ERR_NOSUCHCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NOSUCHCHANNEL,
					this->_cmd[1], MSG_NOSUCHCHANNEL);
			return;
		}
		case ERR_NOTONCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NOTONCHANNEL,
					this->_cmd[1], MSG_NOTONCHANNEL);
			return;
		}
		case ERR_CHANOPRIVSNEEDED:
		{
			this->_author->writeRPLToClient(this->_server, ERR_CHANOPRIVSNEEDED,
					this->_cmd[1], MSG_CHANOPRIVSNEEDED);
			return;
		}
		default:
			std::cerr << "Error: unrecognised error code in JOIN." << std::endl;
			break;
	}
}

void Topic::confirm() const
{
	if (this->_cmd.size() == 2)
	{
		this->_author->writeRPLToClient(this->_server, RPL_TOPIC,
			this->_cmd[1], this->_target->getTopic());
	}
	else if (this->_cmd[2] == ":")
	{
		this->_target->updateTopic(*this->_author, "");
	}
	else
	{
		this->_target->updateTopic(*this->_author, this->_message);

	}
}
