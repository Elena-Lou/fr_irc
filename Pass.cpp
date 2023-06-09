# include "ACommand.hpp"
# include "Pass.hpp"
# include "Client.hpp"
# include "irc.hpp"

Pass::Pass() : ACommand()
{
	/*this is forbidden */
}

Pass::~Pass()
{
}

Pass::Pass(Server &server, Client &author, std::string rawInput) : ACommand(server, author, rawInput)
{
	std::cout << "built  a Pass class" << std::endl;
	this->execute();
}

Pass::Pass(const Pass &source) : ACommand(source)
{
	*this = source;
}

Pass &Pass::operator=(const Pass &rhs)
{
	this->_cmd = rhs._cmd;
	this->_author = rhs._author;
	return (*this);
}

void Pass::execute()
{
	if (this->_author->getUsername() != "")
	{
		error(ERR_ALREADYREGISTERED);
		return ;
	}
	if (this->_cmd.size() < 2)
	{
		error(ERR_NEEDMOREPARAMS);
		return ;
	}
	if (this->_cmd[1] != this->_server->getPassword())
	{
		error(ERR_PASSWDMISMATCH);
	}
	this->confirm();
}

void Pass::error(int errorCode) const
{
	std::stringstream errorMessage;
	errorMessage << ":" << this->_server->getHostname() << " " << errorCode << " "
	<< this->_author->getNickname();
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS:
			this->_author->writeRPLToClient(this->_server,
					ERR_NEEDMOREPARAMS, MSG_NONICKNAMEGIVEN);
			errorMessage << this->_cmd[1] << " :" << MSG_NEEDMOREPARAMS << CRLF;
			break;
		case ERR_ALREADYREGISTERED:
			this->_author->writeRPLToClient(this->_server,
					ERR_ALREADYREGISTERED, MSG_ALREADYREGISTERED);
			break;
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
			break;
	}
	this->_author->writeBuffer += errorMessage.str();
}

void	Pass::confirm() const
{
}
