
# include "ACommand.hpp"
# include "Privmsg.hpp"
# include "Client.hpp"
# include "irc.hpp"

Privmsg::Privmsg() : ACommand()
{
	/*this is forbidden */
}

Privmsg::~Privmsg()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Privmsg destructor" << std::endl;
#endif
}

Privmsg::Privmsg(Server &server, Client &author, std::string rawInput) : ACommand(server, author, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Privmsg full constructor" << std::endl;
#endif
	if (this->_cmd.size() >= 3)
	{
		unsigned int offset = rawInput.find(":");
		if (offset < rawInput.size())
			offset++;
		this->_message = std::string(rawInput, offset);
	}
	this->execute();
}

Privmsg::Privmsg(const Privmsg &source) : ACommand(source)
{
	*this = source;
}

Privmsg &Privmsg::operator=(const Privmsg &rhs)
{
	this->_cmd = rhs._cmd;
	this->_server = rhs._server;
	this->_author = rhs._author;
	return (*this);
}

void Privmsg::execute()
{
	if (!this->_author->isRegistered())
		return;
	if (this->_cmd.size() < 2)
	{
		error(ERR_NORECIPIENT);
		return;
	}
	if (!this->_server->nicknameAlreadyInUse(*this->_author, this->_cmd[1]))
	{
		error(ERR_NOSUCHNICK);
		return;
	}
	if (0) //TODO replace by mod condition dest is a chan with denied access
	{
		error(ERR_CANNOTSENDTOCHAN);
		return;
	}
	if (this->_cmd.size() < 3)
	{
		error(ERR_NOTEXTTOSEND);
		return;
	}
	//NOSUCHSERVER not in subject
	this->confirm();
}

void Privmsg::error(int errorCode) const
{
	switch (errorCode)
	{
		case ERR_NORECIPIENT:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_NORECIPIENT, std::string(MSG_NORECIPIENT) + " (PRIVMSG)");
			break;
		}
		case ERR_NOSUCHNICK:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_NOSUCHNICK, MSG_NOSUCHNICK);
			break;
		}
		case ERR_CANNOTSENDTOCHAN:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_CANNOTSENDTOCHAN, this->_cmd[1], MSG_CANNOTSENDTOCHAN);
			break;
		}
		case ERR_NOTEXTTOSEND:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_NOTEXTTOSEND, MSG_NOTEXTTOSEND);
			break;
		}
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
			break;
	}
}

void	Privmsg::confirm() const
{
	Channel *chanTarget;
	Client	*clientTarget;

	if ((chanTarget = this->_server->getChannelIfExist(this->_cmd[1])) != NULL)
	{
		chanTarget->writePrivmsg(this->_author->getNickname(), this->_cmd[1], this->_message);
	}
	else if ((clientTarget = this->_server->getUserIfConnected(this->_cmd[1])) != NULL)
	{
		clientTarget->writePrivmsg(this->_author->getNickname(), this->_cmd[1], this->_message);
	}
}
