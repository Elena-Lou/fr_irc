#include "Invite.hpp"
#include <vector>

Invite::Invite() : ACommand()
{
	/*this is forbidden */
}

Invite::~Invite()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Invite destructor" << std::endl;
#endif
}

Invite::Invite(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Invite full constructor" << std::endl;
#endif
	this->_targetClient = NULL;
	this->_targetChannel = NULL;
	if (this->_cmd.size() >= 2)
		this->_targetClient = this->_server->getUserIfConnected(this->_cmd[1]);
	if (this->_cmd.size() >= 3)
		this->_targetChannel = this->_server->getChannelIfExist(this->_cmd[2]);
	this->execute();
}

Invite::Invite(const Invite &source) : ACommand(source)
{
	*this = source;
}

Invite &Invite::operator=(const Invite &rhs)
{
	this->_cmd = rhs._cmd;
	this->_server = rhs._server;
	this->_author = rhs._author;
	return (*this);
}

void Invite::execute()
{
	if (!this->_author->isRegistered())
		return ;
	if (this->_cmd.size() < 3)
	{
		error(ERR_NEEDMOREPARAMS);
		return;
	}
	if (this->_targetChannel == NULL)
	{
		error(ERR_NOSUCHCHANNEL);
		return;
	}
	if (!this->_targetChannel->isUserConnected(*this->_author))
	{
		error(ERR_NOTONCHANNEL);
		return;
	}
	if (0 && !this->_targetChannel->isChannelOperator(*this->_author)) //TODO replace 0 by is channel is invite only
	{
		error(ERR_CHANOPRIVSNEEDED);
		return;
	}
	if (this->_targetClient == NULL)
		return; /* no spec in RC silently returning */
	if (this->_targetChannel->isUserConnected(*this->_targetClient))
	{
		error(ERR_USERONCHANNEL);
		return;
	}
	this->confirm();
}

void Invite::error(int errorCode) const
{
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS:
		{
			this->_author->writeRPLToClient(this->_server,
				ERR_NEEDMOREPARAMS, this->_cmd[0], MSG_NEEDMOREPARAMS);
			break;
		}
		case ERR_NOSUCHCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NOSUCHCHANNEL,
				this->_cmd[1], MSG_NOSUCHCHANNEL);
			break;
		}
		case ERR_NOTONCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NOTONCHANNEL,
				this->_cmd[1], MSG_NOTONCHANNEL);
			break;
		}
		case ERR_CHANOPRIVSNEEDED:
		{
			this->_author->writeRPLToClient(this->_server, ERR_CHANOPRIVSNEEDED,
				this->_cmd[1], MSG_CHANOPRIVSNEEDED);
			break;
		}
		case ERR_USERONCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_USERONCHANNEL,
				this->_cmd[2] + " " + this->_cmd[1], MSG_USERONCHANNEL);
			break;
		}
		default:
			std::cerr << "Error: unrecognised error code in JOIN." << std::endl;
	}
}

void Invite::confirm() const
{
	//send invite confirmation to author
	//send invite to targetClient
	//add client to channel invitelist (set ?);
	std::stringstream messageBuilderAuthor;
	messageBuilderAuthor << ":" << this->_server->getHostname() << " INVITE :"
		<< this->_cmd[1] << " " << this->_cmd[2];
	std::stringstream messageBuilderTarget;
	messageBuilderTarget << ":" << this->_author->getFullName() << " INVITE :"
		<< this->_cmd[1] << " " << this->_cmd[2];
	this->_author->writeToClient(messageBuilderAuthor.str());
	this->_targetClient->writeToClient(messageBuilderTarget.str());
}
