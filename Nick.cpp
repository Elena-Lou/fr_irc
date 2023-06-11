# include "ACommand.hpp"
# include "Nick.hpp"
# include "Client.hpp"
# include "Server.hpp"

Nick::Nick() : ACommand()
{
	/*this is forbidden */
}

Nick::~Nick()
{
}

Nick::Nick(Server &server, Client &author, std::string rawInput) : ACommand(server, author, rawInput)
{
	this->execute();
}

Nick::Nick(const Nick &source) : ACommand(source)
{
	*this = source;
}

Nick &Nick::operator=(const Nick &rhs)
{
	this->_cmd = rhs._cmd;
	this->_server = rhs._server;
	this->_author = rhs._author;
	this->newName = rhs.newName;
	return (*this);
}

void Nick::execute()
{
	if (this->_server->isPasswordProtected())
	{
		if (!this->_author->isPasswordOk())
		{
			//this->_server->disconnectUser(this->_author->getSocketFD());
			return;
		}
	}
	else
		this->_author->validatePassword();
	if (this->_cmd.size() != 2)
	{
		error(ERR_NONICKNAMEGIVEN);
		return ;
	}
	this->newName = this->_cmd[1];
	if (!this->isValidNickname(newName))
	{
		error(ERR_ERRONEUSNICKNAME);
		return ;
	}
	if (this->_author->getNickname() == this->newName)
		return ;
	while (this->_server->nicknameAlreadyInUse(*this->_author, this->newName))
	{
		if (this->_author->getUsername() == "")
		{
			this->newName += '_';
			continue;
		}
		error(ERR_NICKNAMEINUSE);
		return ;
	}
	//No condition for ERR_NICKCOLLISION not required by subject
	this->confirm();
}

void Nick::error(int errorCode) const
{
	switch (errorCode)
	{
		case ERR_NONICKNAMEGIVEN:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_NONICKNAMEGIVEN, MSG_NONICKNAMEGIVEN);
			return;
		}
		case ERR_ERRONEUSNICKNAME:
		{
			this->_author->writeRPLToClient(this->_server, ERR_ERRONEUSNICKNAME,
					this->_cmd[1], MSG_ERRONEUSNICKNAME);
			return;
		}
		case ERR_NICKNAMEINUSE:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NICKNAMEINUSE,
					this->_cmd[1], MSG_NICKNAMEINUSE);
			break;
		}
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
			break;
	}
}

void	Nick::confirm() const
{
	std::stringstream msgBuilder;
	if (this->_author->getNickname() == "")
		this->_author->setNickname("*");
	msgBuilder << ":" << this->_author->getFullName() << " NICK " << this->newName;
	this->_author->setNickname(this->newName);
	this->_server->broadcastAllClients(msgBuilder.str());
}

bool	Nick::isValidNickname(std::string name)
{
	for (unsigned int i = 0; i < name.size(); i++)
	{
		if (!isalnum(name[i]) && name[i] != '[' && name[i] != ']'
			&& name[i] != '{' && name[i] != '}'
			&& name[i] != '\\' && name[i] != '|')
		{
			return (false);
		}
	}
	return (true);
}
