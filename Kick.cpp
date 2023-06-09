#include "Kick.hpp"

Kick::Kick() : ACommand()
{
}

Kick::Kick( Kick const & src ) : ACommand(src)
{
	*this = src;
}

Kick::Kick(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
	std::cout << "KICK overloaded constructor" << std::endl;
	std::cout << "client socketFD : " << this->_author->getSocketFD() << std::endl;
	this->execute();
}

Kick & Kick::operator=( Kick const & rhs )
{
	if (this != &rhs)
	{
		this->_cmd = rhs._cmd;
		this->_server = rhs._server;
		this->_author = rhs._author;
	}
	return *this;
}

Kick::~Kick()
{
	std::cout << "KICK destructor" << std::endl;
}

void Kick::execute()
{
	if (this->_cmd.size() < 3)
	{
		error(ERR_NEEDMOREPARAMS);
		return ;
	}

	/* Does the channel exist ? */
	Channel *foundChannel;
	if ((foundChannel = this->_server->getChannelIfExist(this->_cmd[1])) == NULL)
	{
		error(ERR_NOSUCHCHANNEL);
		return ;
	}

	/* Is the user connected to that channel ? */
	if (!foundChannel->isUserConnected(*(this->_author)))
	{
		error(ERR_NOTONCHANNEL);
		return ;
	}

	/* Is the user a Channel Operator ? */
	if (!foundChannel->isChannelOperator(*(this->_author)))
	{
		error(ERR_CHANOPRIVSNEEDED);
		return ;
	}
	/* Is the target_user the author wants to kick conected to that channel ?*/
	Client *foundClient;
	if ((foundClient = foundChannel->getUserIfConnected(this->_cmd[2])) == NULL)
	{
		error(ERR_NOTONCHANNEL);
		return ;
	}
	if (foundChannel->removeUserFromChannel(*foundClient) == 0)
		this->_server->destroyChannel(*foundChannel);
}

void	Kick::confirm() const
{
}

void Kick::error( int errorCode ) const
{
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS);
			return;
		}
		case ERR_NOSUCHCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NOSUCHCHANNEL,
					this->_cmd[1], MSG_NOSUCHCHANNEL);
			return;
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
			break;
	}
}