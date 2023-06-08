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
	return (*this);
}

void Nick::execute()
{
	if (this->_cmd.size() != 2)
	{
		error(ERR_NONICKNAMEGIVEN);
		return ;
	}
	this->newName = this->_cmd[1];
	for (unsigned int i = 0; i < this->newName.size(); i++)
	{
		if (!isalpha(newName[i]))
		{
			error(ERR_ERRONEUSNICKNAME);
			return ;
		}
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
	std::stringstream msgBuilder;
	msgBuilder << ":" << this->_server->getHostname() << " " << errorCode << " ";
	switch (errorCode)
	{
		case ERR_NONICKNAMEGIVEN:
			msgBuilder << ":" << ERR_NONICKNAMEGIVEN_MSG;
			break;
		case ERR_ERRONEUSNICKNAME:
			msgBuilder << this->_cmd[1] << " :" << ERR_ERRONEUSNICKNAME_MSG;
			break;
		case ERR_NICKNAMEINUSE:
			msgBuilder << this->_cmd[1] << " :" << ERR_NICKNAMEINUSE_MSG;
			break;
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
			break;
	}
	this->_author->writeToClient(msgBuilder.str());
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
