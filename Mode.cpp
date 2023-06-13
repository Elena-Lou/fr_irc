#include "Mode.hpp"

Mode::Mode() : ACommand()
{
}

Mode::Mode( Mode const & src ) : ACommand(src)
{
#if SHOW_CONSTRUCTOR
	std::cout << "MODE copy constructor" << std::endl;
#endif
	*this = src;
}

Mode::Mode(Server &server, Client & author, std::string rawInput) : ACommand(server, author, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "MODE overloaded constructor" << std::endl;
#endif
	this->execute();
}

Mode & Mode::operator=( Mode const & rhs )
{
	if (this != &rhs)
	{
		this->_cmd = rhs._cmd;
		this->_server = rhs._server;
		this->_author = rhs._author;
	}
	return *this;
}

Mode::~Mode()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Mode destructor" << std::endl;
#endif
}

Mode::Mode(Server &server, Client & author, std::string rawInput) : ACommand(server, author, rawInput)
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

// void Mode::execute()
// {
// 	if (!this->_author->isRegistered())
// 		return ;
// 	if (this->_cmd.size() < 2)
// 	{
// 		error(ERR_NEEDMOREPARAMS);
// 		return;
// 	}
// 	if (this->isAPossibleChannelName(this->_cmd[1]) && this->_targetChannel == NULL)
// 	{
// 		error(ERR_NOSUCHCHANNEL);
// 		return;
// 	}
// 	if (0 && !this->_targetChannel->isChannelOperator(*this->_author)) //TODO replace 0 by is channel is invite only
// 	{
// 		error(ERR_CHANOPRIVSNEEDED);
// 		return;
// 	}
// 	if (!this->isAPossibleChannelName(this->_cmd[1]) && this->_server->getUserIfConnected(this->_cmd[1]) == NULL)
// 	{
// 		error(ERR_NOSUCHNICK);
// 		return;
// 	}
// 	if (!this->isAPossibleChannelName(this->_cmd[1]) && this->_cmd[1] != this->_author->getNickname())
// 	{
// 		error(ERR_USERSDONTMATCH);
// 		return;
// 	}
// 	this->confirm();
// }


void Mode::execute()
{
    if (!this->_author->isRegistered())
		return;
    if (this->_cmd.size() == 1)
    {
        error(ERR_NEEDMOREPARAMS);
        return ;
    }
    /* check channel exists */
	this->_targetChannel = this->_server->getChannelIfExist(this->_cmd[1]);
	if (this->isAPossibleChannelName(this->_cmd[1]) && this->_targetChannel == NULL)
	{
		error(ERR_NOSUCHCHANNEL);
		return ;
	}

	/* Is the user connected to that channel ? */
	if (!this->_targetChannel->isUserConnected(*(this->_author)))
	{
		error(ERR_NOTONCHANNEL);
		return ;
	}

    /* need to check if author has priviledges before checking the rest of the command */
    if (!this->_targetChannel->isChannelOperator(*(this->_author)))
	{
		error(ERR_CHANOPRIVSNEEDED);
		return ;
	}
	
	this->checkValidCmd();  

}

void	Mode::confirm() const
{

}

void Mode::error( int errorCode ) const
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
        case ERR_NOSUCHNICK:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_NOSUCHNICK, this->_cmd[3], MSG_NOSUCHNICK);
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

int Mode::checkValidCmd()
{
/* check the command is valid */
    if (this->_cmd.size() < 5)
    {
		if (this->_cmd[2][0] != '-' || this->_cmd[2][0] != '+')
		{
			std::cerr << "Invalid sign" << std::endl;
			return ;
		}

		switch (this->_cmd[2][1])
		{
		case ('i'):
			std::cout << "Invite must be set" << std::endl;
			this->invite();
			break;
		case ('t'):
			std::cout << "Topic must be set" << std::endl;
			return topic();
		case ('k'):
			std::cout << "Password must be set" << std::endl;
			this->channelKey();
			break;
		case ('o'):
			std::cout << "ChanOp must be set" << std::endl;
			this->channelOp();
			break;
		case ('l'):
			std::cout << "Chan Limit must be set" << std::endl;
			this->channelLimit();
			break;
		default:
			error(ERR_UMODEUNKNOWNFLAG);
			break;
		}
        
    }
}

int	Mode::topic( )
{
	/* check the topic command is valid */
	if (this->_cmd[2][0] == '+')
	{
		this->_targetChannel->_modeFlagsField |= TOPIC_MODE;
		this->_targetChannel->changeTopicProtection(true);
		return true;
	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->_modeFlagsField &= ~TOPIC_MODE;
		this->_targetChannel->changeTopicProtection(false);
		return true;
	}
	return false;
}

int	Mode::invite()
{
	/* check the invite command is valid */
	if (this->_cmd[2][0] == '+')
	{
		this->_targetChannel->_modeFlagsField |= INVITE_MODE;
		return true;
	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->_modeFlagsField &= ~INVITE_MODE;
		return true;
	}
	return false;
}

int	Mode::channelKey()
{
	/* check the channel password command is valid && has enough arguments */
	if (this->_cmd.size() != 4)
	{
		return ERR_NEEDMOREPARAMS;
	}
	if (this->_cmd[2][0] == '+')
	{
		this->_targetChannel->_modeFlagsField |= PASSWORD_MODE;
		return true;

	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->_modeFlagsField &= ~PASSWORD_MODE;
		return true;
	}

}

int	Mode::channelOp( )
{
	/* check the channel Ope command is valid && has enough arguments */
	if (this->_cmd.size() != 4)
	{
		return ERR_NEEDMOREPARAMS;
	}
	if (this->_cmd[2][0] == '+')
	{
		this->_targetChannel->_modeFlagsField |= CHANOP_MODE;
		this->_targetChannel->setOperator(*this->_author);
	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->_modeFlagsField &= ~CHANOP_MODE;
		this->_targetChannel->removeOperator(*this->_author);
	}

}

int	Mode::channelLimit( )
{
	/* check the channel limit command is valid && has enough arguments */
	if (this->_cmd.size() != 4)
	{
		return ERR_NEEDMOREPARAMS;
	}
	/* check that this->_cmd[3] is digit */
	int i = 0;
	while (this->_cmd[3][i])
	{
		if (!std::isdigit(this->_cmd[3][i]))
			return false;
	}
	
	if (this->_cmd[2][0] == '+')
	{
		this->_targetChannel->_modeFlagsField |= CHANLIMIT_MODE;
		this->_targetChannel->setMaxClients(std::atoi(this->_cmd[3].c_str()));
	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->_modeFlagsField &= ~CHANLIMIT_MODE;
	}

}