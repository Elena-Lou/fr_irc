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
	std::cout << "MODE destructor" << std::endl;
#endif
}

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

    /* need to check if author has priviledges before checking the rest of the command */
    if (!this->_foundChannel->isChannelOperator(*(this->_author)))
	{
		error(ERR_CHANOPRIVSNEEDED);
		return ;
	}

    /* check the command is valid */
    if (this->_cmd.size() < 5)
    {
        if (this->_cmd[2].size() != 2 || this->_cmd[2][0] != '-' || this->_cmd[2][0] != '+')
        {
            std::cout << "this->_cmd[2] : " << this->_cmd[2] << std::endl; 
            std::cout << "this->_cmd[2].size() : " << this->_cmd[2].size() << std::endl; 
            std::cerr << "Invalid input" << std::endl;
            return ;
        }
        if (this->_cmd.size() == 3)
        {
            if (this->_cmd[2][1] == 't')
            {
                std::cout << "Topic must be set" << std::endl;
            }
            else if (this->_cmd[2][1] == 'i')
            {
                std::cout << "Invite must be set" << std::endl;
            }
            else
            {
                std::cout << "Invalid mode" << std::endl;
                return ;
            }
        }
        else if (this->_cmd.size() == 4)
        {
            if (this->_cmd[2][1] == 'o')
            {
                std::cout << "OP must be set" << std::endl;
            }
            else if (this->_cmd[2][1] == 'l')
            {
                std::cout << "user limit must be set" << std::endl;
            }
            else if (this->_cmd[2][1] == 'k')
            {
                std::cout << "channel password must be set" << std::endl;
            }
            else
            {
                std::cout << "invalid mode" << std::endl;
            }
        }
    }

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
					ERR_NOSUCHNICK, this->_cmd[4], MSG_NOSUCHNICK);
			break;
		}
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
	}
}
