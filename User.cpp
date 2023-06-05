
# include "ACommand.hpp"
# include "User.hpp"
# include "Client.hpp"

User::User() : ACommand()
{
	/*this is forbidden */
}

User::~User()
{
}

User::User(Server &server, Client &author, std::string rawInput) : ACommand(server, author, rawInput)
{
	this->execute();
}

User::User(const User &source)
{
	*this = source;
}

User &User::operator=(const User &rhs)
{
	this->_cmd = rhs._cmd;
	this->_author = rhs._author;
	return (*this);
}

void User::execute() const
{
	if (this->_author.username)
	{
		error(ERR_ALREADYREGISTERED);
		return ;
	}
	if (this->_cmd.size() < 2)
	{
		error(ERR_NEEDMOREPARAMS);
		return ;
	}
	this->confirm();
}

void User::error(int errorCode) const
{
	std::stringstream errorMessage;
	errorMessage << ":" << this->_server->getHostname() << " " << errorCode << " "
	<< this->_author->getNickname();
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS
			errorMessage << this->_cmd[1] << " :" << ERR_NEEDMOREPARAMS_MSG << CRLF;
			break;
		case ERR_ALREADYREGISTERED:
			errorMessage << " :" << ERR_ERRONEUSNICKNAME_MSG << CRLF;
			break;
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
			break;
	}
	this->_author->writeBuffer += errorMessage.str();
}

void	User::confirm() const
{
	/* Setting Nickname */
	this->_author.setNickname(this->_cmd[1]);
	/* Setting Username if given or default to nickname */
	if (this->_cmd.size() >= 3)
		this->_author.setUsername(this->_cmd[2]);
	else
		this->_author.setUsername(this->_cmd[1]);

	/* Setting Hostname if given or default to nickname */
	if (this->_cmd.size() >= 4)
		this->_author.setHostname(this->_cmd[3]);
	else
		this->_author.setHostname(this->_cmd[1]);

	/* Setting Realname if given or default to nickname */
	if (this->_cmd.size() >= 5)
		this->_author.setRealname(this->_cmd[4]);
	else
		this->_author.setRealname(this->_cmd[1]);

	this->_author.confirmRegistration();

	/* build message */
	std::stringstream replyMessageBuilder;

	//broadcast all
	// RPL_WELCOME
	this->sendRPLWELCOME();
	this->sendRPLYOURHOST();
	this->sendRPLCREATED();
	this->_server->broadcastAllClients(replyMessage);
}

void	User::sendRPLWELCOME()
{
	replyMessageBuilder << ":" << this->_server.getHostname() << " "
		<< RPL_WELCOME << " " << this->_author.getNickname() << " :" <<
		<< "Welcome to the " << this->_server.getHostname() << " Network, "
		<< this->_author.nick() << "[!" << this->_author.user() << "@"
		<< this->_author.hostname() << "]" << CRLF;
	std::string replyMessage = replyMessageBuilder.str();
	this->_author.writeBuffer += replyMessage;
}

void	User::sendRPLYOURHOST()
{
	replyMessageBuilder << ":" << this->_server.getHostname() << " "
		<< RPL_YOURHOST << " " << this->_author.getNickname() << " :" <<
		<< "Your host is " << this->server.getHostname()
		<< ", running version mismatched sock(et)s !" << CRLF;
}

void	User::sendRPLCREATED()
{
	replyMessageBuilder << ":" << this->_server.getHostname() << " "
		<< RPL_YOURHOST << " " << this->_author.getNickname() << " :" <<
		<< "This server was created" << this->getStartTime() << CRLF;

}
