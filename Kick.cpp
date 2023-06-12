#include "Kick.hpp"

Kick::Kick() : ACommand()
{
}

Kick::Kick( Kick const & src ) : ACommand(src)
{
#if SHOW_CONSTRUCTOR
	std::cout << "KICK copy constructor" << std::endl;
#endif
	*this = src;
}

Kick::Kick(Server &server, Client &author, std::string rawInput) : ACommand(server, author, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "KICK overloaded constructor" << std::endl;
#endif
	this->_foundChannel = NULL;
	this->execute();
}

Kick & Kick::operator=( Kick const & rhs )
{
	if (this != &rhs)
	{
		this->_cmd = rhs._cmd;
		this->_server = rhs._server;
		this->_author = rhs._author;
		this->_foundChannel = rhs._foundChannel;
	}
	return *this;
}

Kick::~Kick()
{
#if SHOW_CONSTRUCTOR
	std::cout << "KICK destructor" << std::endl;
#endif
}

void Kick::execute()
{
	if (!this->_author->isRegistered())
		return ;
	if (this->_cmd.size() < 3)
	{
		error(ERR_NEEDMOREPARAMS);
		return ;
	}

	/* Does the channel exist ? */
	if ((this->_foundChannel = this->_server->getChannelIfExist(this->_cmd[1])) == NULL)
	{
		error(ERR_NOSUCHCHANNEL);
		return ;
	}

	/* Is the user connected to that channel ? */
	if (!this->_foundChannel->isUserConnected(*(this->_author)))
	{
		error(ERR_NOTONCHANNEL);
		return ;
	}

	/* Is the user a Channel Operator ? */
	if (!this->_foundChannel->isChannelOperator(*(this->_author)))
	{
		error(ERR_CHANOPRIVSNEEDED);
		return ;
	}
	/* Is the target_user the author wants to kick conected to that channel ?*/
	Client *foundClient;
	if ((foundClient = this->_foundChannel->getUserIfConnected(this->_cmd[2])) == NULL)
	{
		error(ERR_NOTONCHANNEL);
		return ;
	}
	this->confirm();
	if (this->_foundChannel->removeUserFromChannel(*foundClient) == 0)
		this->_server->destroyChannel(*this->_foundChannel);
}

void	Kick::confirm() const
{
	std::stringstream msgBuilder;
	msgBuilder << ":" << this->_author->getFullName() << " KICK" << this->_cmd[1] << " " << this->_cmd[2] << " :" << this->_author->getNickname();
	this->_server->broadcastChannel(*this->_foundChannel, msgBuilder.str());
}

void Kick::error( int errorCode ) const
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
		case ERR_CHANOPRIVSNEEDED:
		{
			this->_author->writeRPLToClient(this->_server, ERR_CHANOPRIVSNEEDED,
					this->_cmd[1], MSG_CHANOPRIVSNEEDED);
			break;
		}
		case ERR_USERNOTINCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_USERNOTINCHANNEL,
					this->_cmd[2] + " " + this->_cmd[1], MSG_USERNOTINCHANNEL);
			break;
		}
		case ERR_NOTONCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NOTONCHANNEL,
					this->_cmd[1], MSG_NOTONCHANNEL);
			break;
		}
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
	}
}