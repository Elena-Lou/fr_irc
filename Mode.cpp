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
	std::cout << "Mode full constructor" << std::endl;
#endif
	this->_targetChannel = NULL;
	if (this->_cmd.size() >= 2)
		this->_targetChannel = this->_server->getChannelIfExist(this->_cmd[1]);
	this->execute();
}

Mode & Mode::operator=( Mode const & rhs )
{
	if (this != &rhs)
	{
		this->_cmd = rhs._cmd;
		this->_server = rhs._server;
		this->_author = rhs._author;
		this->_targetChannel = rhs._targetChannel;
	}
	return *this;
}

Mode::~Mode()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Mode destructor" << std::endl;
#endif
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
	if (this->_targetChannel->isMode(INVITE_MODE) && !this->_targetChannel->isChannelOperator(*this->_author))
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
	if (!this->isAPossibleChannelName(this->_cmd[1]))
	{
		// This is not asked by the subject
		return;
	}

	int cmdReturn = this->checkValidCmd();
	if (cmdReturn != 0)
	{
		error(cmdReturn);
		return ;
	}
	this->confirm();
}

void	Mode::sendRPLCHANNELMODEIS() const
{
	std::stringstream modeString;

	modeString << ":" << this->_server->getHostname() << " " <<
	RPL_CHANNELMODEIS << " " << this->_author->getNickname() << " " << this->_cmd[1] <<
	" " << this->_targetChannel->getModes();
	this->_author->writeToClient(modeString.str());
}

void	Mode::confirm() const
{
	this->sendRPLCHANNELMODEIS();
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
		case ERR_KEYSET:
		{
			this->_author->writeRPLToClient(this->_server, ERR_KEYSET,
				this->_cmd[1], MSG_KEYSET);
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
	if (this->_cmd.size() == 2)
		return SUCCESS;

	if (this->_cmd.size() >= 3)
	{
		if (this->_cmd[2][0] == '-' || this->_cmd[2][0] == '+')
		{
			switch (this->_cmd[2][1])
			{
			case ('i'):
				std::cout << "Invite must be set" << std::endl;
				return invite();
			case ('t'):
				std::cout << "Topic must be set" << std::endl;
				return topic();
			case ('k'):
				std::cout << "Password must be set" << std::endl;
				return channelKey();
			case ('o'):
				std::cout << "ChanOp must be set" << std::endl;
				return channelOp();
			case ('l'):
				std::cout << "Chan Limit must be set" << std::endl;
				return channelLimit();
			default:
				error(ERR_UMODEUNKNOWNFLAG);
				break;
			}
		}
	}
	return ERROR;
}

int	Mode::topic( )
{
	/* check the topic command is valid */
	if (this->_cmd[2][0] == '+')
	{
		this->_targetChannel->setMode(TOPIC_MODE);
		this->_targetChannel->changeTopicProtection(true);
		return SUCCESS;
	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->unsetMode(TOPIC_MODE);
		this->_targetChannel->changeTopicProtection(false);
		return SUCCESS;
	}
	return ERR_UMODEUNKNOWNFLAG;
}

int	Mode::invite()
{
	/* check the invite command is valid */
	if (this->_cmd[2][0] == '+')
	{
		this->_targetChannel->setMode(INVITE_MODE);
		return SUCCESS;
	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->unsetMode(INVITE_MODE);
		return SUCCESS;
	}
	return ERR_UMODEUNKNOWNFLAG;
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
		if (this->_targetChannel->isProtected())
			return ERR_KEYSET;
		this->_targetChannel->setMode(PASSWORD_MODE);
		this->_targetChannel->changePassword(this->_cmd[3]);
		return SUCCESS;

	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->unsetMode(PASSWORD_MODE);
		this->_targetChannel->changeChannelProtection(false);
		return SUCCESS;
	}
	return ERR_UMODEUNKNOWNFLAG;
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
		this->_targetChannel->setMode(CHANOP_MODE);
		this->_targetChannel->setOperator(*this->_author);
		return SUCCESS;
	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->unsetMode(CHANOP_MODE);
		this->_targetChannel->removeOperator(*this->_author);
		return SUCCESS;
	}
	return ERR_UMODEUNKNOWNFLAG;
}

int	Mode::channelLimit( )
{
	/* check the channel limit command is valid && has enough arguments */
	if (this->_cmd.size() != 4)
		return ERR_NEEDMOREPARAMS;
	/* check that this->_cmd[3] is digit */
	int i = 0;
	while (this->_cmd[3][i])
	{
		if (!std::isdigit(this->_cmd[3][i]))
			return ERR_UMODEUNKNOWNFLAG;
	}
	if (this->_cmd[2][0] == '+')
	{
		this->_targetChannel->setMode(CHANLIMIT_MODE);
		this->_targetChannel->setMaxClients(std::atoi(this->_cmd[3].c_str()));
		return SUCCESS;
	}
	else if (this->_cmd[2][0] == '-')
	{
		this->_targetChannel->unsetMode(CHANLIMIT_MODE);
		return SUCCESS;
	}
	return ERR_UMODEUNKNOWNFLAG;
}