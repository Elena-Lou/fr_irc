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

Nick::Nick(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
	this->execute();
}

Nick::Nick(const Nick &source)
{
	*this = source;
}

Nick &Nick::operator=(const Nick &rhs)
{
	this->_cmd = rhs._cmd;
	this->_user = rhs._user;
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
	std::map<int, Client> clientsConnectedCopy = this->_server->getClients();
	for (std::map<int, Client>::iterator it = clientsConnectedCopy.begin();
		it != clientsConnectedCopy.end(); it++)
	{
		if (newName == it->second.getUsername())
		{
			error(ERR_NICKNAMEINUSE);
			return ;
		}
	}
	//No condition for ERR_NICKCOLLISION not required by subject
	std::string message;
	message += this->_user->getUsername() + " changed his nickname to " + newName + ".\r\n";
	this->_server->broadcastAllClients(message);
}

void Nick::error(int errorCode) const
{
	std::stringstream converter;
	std::string errorMessage;
	if (errorCode != ERR_NONICKNAMEGIVEN)
	{
		converter << errorCode << " :" << this->_user->getUsername() << " "
			<< this->_cmd[1];
		errorMessage = converter.str();
	}
	switch (errorCode)
	{
		case ERR_NONICKNAMEGIVEN:
			errorMessage += this->_user->getUsername() + ": No nickname given\r\n";
			break;
		case ERR_ERRONEUSNICKNAME:
			errorMessage += "Erroneus nickname\r\n";
			break;
		case ERR_NICKNAMEINUSE:
			errorMessage += "Nickname is already in use\r\n";
			break;
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
			break;
	}
	this->_user->writeBuffer += errorMessage;
}
