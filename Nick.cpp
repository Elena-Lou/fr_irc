# include "ACommand.hpp"
# include "Nick.hpp"
# include "Client.hpp"

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

void Nick::execute() const
{
	if (this->_cmd.size() != 2)
	{
		error(ERR_NONICKNAMEGIVEN);
		return ;
	}
	std::string newName = this->_cmd[1];
	for (unsigned int i = 0; i < newName.size(); i++)
	{
		if (!isalpha(newName[i]))
		{
			error(ERR_ERRONEUSNICKNAME);
			return ;
		}
	}
	if (this->_author->getNickname() == newName)
		return ;
	if (this->_server->isUserConnected(newName) && this->_author->getNickname() != "")
	{
		error(ERR_NICKNAMEINUSE);
		return ;
	}

	//No condition for ERR_NICKCOLLISION not required by subject
	if (this->_author->getNickname() != "")
		this->confirm();
	this->_author->setNickname(newName);
}

void Nick::error(int errorCode) const
{
	std::stringstream prefix;
	prefix << ":" << this->_server->getHostname() << " " << errorCode << " ";
	std::stringstream suffix;
	switch (errorCode)
	{
		case ERR_NONICKNAMEGIVEN:
			suffix << " :" << ERR_NONICKNAMEGIVEN_MSG << CRLF;
			break;
		case ERR_ERRONEUSNICKNAME:
			suffix << this->_cmd[1] << " :" << ERR_ERRONEUSNICKNAME_MSG << CRLF;
			break;
		case ERR_NICKNAMEINUSE:
			suffix << " " << this->_cmd[1] << " :" << ERR_NICKNAMEINUSE_MSG << CRLF;
			break;
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
			break;
	}
	this->_author->writeToClient(prefix.str(), suffix.str());
}

void	Nick::confirm() const
{
	std::stringstream prefix;
	std::stringstream suffix;
	prefix << ":" << this->_author->getFullName() << " " << this->_cmd[0] << " ";
	suffix << " :"  << this->_cmd[1] << CRLF;
	this->_author->setNickname(this->_cmd[1]);
	this->_server->broadcastAllClients(prefix.str(), suffix.str());
}
