#include "Mode.hpp"

Mode::Mode() : ACommand()
{
	/*this is forbidden */
}

Mode::~Mode()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Mode destructor" << std::endl;
#endif
}

Mode::Mode(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Mode full constructor" << std::endl;
#endif
	this->_targetChannel = NULL;
	if (this->_cmd.size() >= 2)
		this->_targetChannel = this->_server->getChannelIfExist(this->_cmd[1]);
	this->execute();
}

Mode::Mode(const Mode &source) : ACommand(source)
{
	*this = source;
}

Mode &Mode::operator=(const Mode &rhs)
{
	this->_cmd = rhs._cmd;
	this->_server = rhs._server;
	this->_author = rhs._author;
	return (*this);
}

void Mode::execute()
{
	if (!this->_author->isRegistered())
		return ;
	if (this->_cmd.size() < 2)
	{
		error(ERR_NEEDMOREPARAMS);
		return;
	}
	if (this->isAPossibleChannelName(this->_cmd[1]) && this->_targetChannel == NULL)
	{
		error(ERR_NOSUCHCHANNEL);
		return;
	}
	if (0 && !this->_targetChannel->isChannelOperator(*this->_author)) //TODO replace 0 by is channel is invite only
	{
		error(ERR_CHANOPRIVSNEEDED);
		return;
	}
	if (!this->isAPossibleChannelName(this->_cmd[1]) && this->_server->getUserIfConnected(this->_cmd[1]) == NULL)
	{
		error(ERR_NOSUCHNICK);
		return;
	}
	if (!this->isAPossibleChannelName(this->_cmd[1]) && this->_cmd[1] != this->_author->getNickname())
	{
		error(ERR_USERSDONTMATCH);
		return;
	}
	this->confirm();
}

void Mode::error(int errorCode) const
{
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS: /* not in the rfc but why not ? */
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
		case ERR_CHANOPRIVSNEEDED:
		{
			this->_author->writeRPLToClient(this->_server, ERR_CHANOPRIVSNEEDED,
				this->_cmd[1], MSG_CHANOPRIVSNEEDED);
			break;
		}
		case ERR_NOSUCHNICK:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NOSUCHNICK,
				this->_cmd[1], MSG_NOSUCHNICK);
			break;
		}
		case ERR_USERSDONTMATCH:
		{
			this->_author->writeRPLToClient(this->_server, ERR_USERSDONTMATCH,
				MSG_USERSDONTMATCH);
			break;
		}
		case ERR_UMODEUNKNOWNFLAG:
		{
			this->_author->writeRPLToClient(this->_server, ERR_UMODEUNKNOWNFLAG,
					MSG_UMODEUNKOWNFLAG);
			break;
		}
		default:
			std::cerr << "Error: unrecognised error code in Mode." << std::endl;
	}
}

void Mode::confirm() const
{
}

bool	Mode::isAPossibleChannelName(std::string name)
{

	if (this->_cmd.size() < 2)
		return (false);
	if (name[0] != '#' || name.size() < 2)
		return (false);
	for (unsigned int i = 1; i < name.size(); i++)
	{
		if (!std::isalnum(name[i]))
			return (false);
	}
	return (true);
}
