
# include "ACommand.hpp"
# include "User.hpp"
# include "Client.hpp"
# include "irc.hpp"

User::User() : ACommand()
{
	/*this is forbidden */
}

User::~User()
{
}

User::User(Server &server, Client &author, std::string rawInput) : ACommand(server, author, rawInput)
{
	std::cout << "built  a User class" << std::endl;
	this->execute();
}

User::User(const User &source) : ACommand(source)
{
	*this = source;
}

User &User::operator=(const User &rhs)
{
	this->_cmd = rhs._cmd;
	this->_author = rhs._author;
	return (*this);
}

void User::execute()
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
	this->confirm();
}

void User::error(int errorCode) const
{
	std::stringstream errorMessage;
	errorMessage << ":" << this->_server->getHostname() << " " << errorCode << " "
	<< this->_author->getNickname();
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS:
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
	this->_author->setNickname(this->_cmd[1]);
	/* Setting Username if given or default to nickname */
	if (this->_cmd.size() >= 3)
		this->_author->setUsername(this->_cmd[2]);
	else
		this->_author->setUsername(this->_cmd[1]);

	/* Setting Hostname if given or default to nickname */
	if (this->_cmd.size() >= 4)
		this->_author->setHostname(this->_cmd[3]);
	else
		this->_author->setHostname(this->_cmd[1]);

	/* Setting Realname if given or default to nickname */
	if (this->_cmd.size() >= 5)
		this->_author->setRealname(this->_cmd[4]);
	else
		this->_author->setRealname(this->_cmd[1]);

	this->_author->confirmRegistration();


	//broadcast all
	// RPL_WELCOME
	this->sendRPLWELCOME();
	this->sendRPLYOURHOST();
	this->sendRPLCREATED();
	this->sendRPLISUPPORT();
	//this->_server->broadcastAllClients(replyMessage);
}

void	User::sendRPLWELCOME() const
{
	std::stringstream replyMessageBuilder;
	replyMessageBuilder << ":" << this->_server->getHostname() << " "
		<< RPL_WELCOME << " " << this->_author->getNickname() << " :"
		<< "Welcome to the " << this->_server->getHostname() << " Network, "
		<< this->_author->getFullName() << CRLF;
	this->_author->writeBuffer += replyMessageBuilder.str();
}

void	User::sendRPLYOURHOST() const
{
	std::stringstream replyMessageBuilder;
	replyMessageBuilder << ":" << this->_server->getHostname() << " "
		<< RPL_YOURHOST << " " << this->_author->getNickname() << " :"
		<< "Your host is " << this->_server->getHostname()
		<< ", running version mismatched sock(et)s !" << CRLF;
	this->_author->writeBuffer += replyMessageBuilder.str();
}

void	User::sendRPLCREATED() const
{
	std::stringstream replyMessageBuilder;
	replyMessageBuilder << ":" << this->_server->getHostname() << " "
		<< RPL_YOURHOST << " " << this->_author->getNickname() << " :"
		<< "This server was created " << this->_server->getStartTime() << CRLF;
	this->_author->writeBuffer += replyMessageBuilder.str();
}

void	User::sendRPLISUPPORT() const
{
	//the CHANMODE and MAXLIST need to be reviewed
	std::stringstream replyMessageBuilder1;
	std::stringstream replyMessageBuilder2;
	replyMessageBuilder1 << ":" << this->_server->getHostname() << " "
		<< RPL_ISUPPORT << " " << this->_author->getNickname() << " "
		<< "AWAYLEN=255 "
		<< "CASEMAPPING=ascii "
		<< "CHANLIMIT=&:0,#: "
		<< "CHANMODES=k "
		<< "CHANNELLEN=64 "
		<< "CHANTYPES=# "
		<< "ELIST=M "
				<< ":are supported by this server" << CRLF;
	replyMessageBuilder2 << ":" << this->_server->getHostname() << " "
		<< RPL_ISUPPORT << " " << this->_author->getNickname() << " "
		<< "HOSTLEN=64 "
		<< "KICKLEN=255 "
		<< "MAXLIST=k:50 "
		<< "NICKLEN=30 " //we need to update this depending on NICK implementation
		<< "STATUSMSG=@+ "
		<< "TOPICLEN=307 "
		<< "USERLEN=30 " //we need to update this depending on USER implementation
		<< ":are supported by this server" << CRLF;
	this->_author->writeBuffer += replyMessageBuilder1.str();
	this->_author->writeBuffer += replyMessageBuilder2.str();
}
