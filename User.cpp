
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
#if SHOW_CONSTRUCTOR
	std::cout << "User destructor" << std::endl;
#endif
}

User::User(Server &server, Client &author, std::string rawInput) : ACommand(server, author, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "User constructor" << std::endl;
#endif
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
	this->_server = rhs._server;
	return (*this);
}

void User::execute()
{
	if (this->_server->isPasswordProtected())
	{
		if (!this->_author->isPasswordOk())
		{
			//this->_server->disconnectUser(this->_author->getSocketFD());
			return ;
		}
	}
	else
		this->_author->validatePassword();
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
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NEEDMOREPARAMS,
				this->_cmd[0],  MSG_NEEDMOREPARAMS);
			break;
		}
		case ERR_ALREADYREGISTERED:
		{
			this->_author->writeRPLToClient(this->_server, ERR_ALREADYREGISTERED,
				MSG_ALREADYREGISTERED);
			break;
		}
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
	}
}

void	User::confirm() const
{
	/* Setting Nickname */
	if (!this->_server->nicknameAlreadyInUse(*this->_author, this->_cmd[1]))
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
	this->sendRPLWELCOME();
	this->sendRPLYOURHOST();
	this->sendRPLCREATED();
	this->sendRPLISUPPORT();
	this->sendRPLUSERS();
}

void	User::sendRPLWELCOME() const
{
	std::stringstream rplMsgBuilder;
	rplMsgBuilder
		<< "Welcome to the " << this->_server->getHostname() << " Network, "
		<< this->_author->getFullName();
	this->_author->writeRPLToClient(this->_server, RPL_WELCOME, rplMsgBuilder.str());
}

void	User::sendRPLYOURHOST() const
{
	std::stringstream rplMsgBuilder;
	rplMsgBuilder
		<< RPL_YOURHOST << " " << this->_author->getNickname() << " :"
		<< "Your host is " << this->_server->getHostname()
		<< ", running version mismatched sock(et)s !" << CRLF;
	this->_author->writeBuffer += rplMsgBuilder.str();
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
	replyMessageBuilder1
		<< "AWAYLEN=255 "
		<< "CASEMAPPING=ascii "
		<< "CHANLIMIT=&:0,#:" << MAX_CHAN_PER_USER << " "
		<< "CHANMODES=k "
		<< "CHANNELLEN=64 "
		<< "CHANTYPES=# "
		<< "ELIST=M "
				<< ":are supported by this server";
	replyMessageBuilder2
		<< "HOSTLEN=64 "
		<< "KICKLEN=255 "
		<< "MAXLIST=k:50 "
		<< "NICKLEN=30 " //we need to update this depending on NICK implementation
		<< "STATUSMSG=@+ "
		<< "TOPICLEN=307 "
		<< "USERLEN=30 " //we need to update this depending on USER implementation
		<< ":are supported by this server";
	this->_author->writeRPLToClient(this->_server, RPL_ISUPPORT, replyMessageBuilder1.str());
	this->_author->writeRPLToClient(this->_server, RPL_ISUPPORT, replyMessageBuilder2.str());
}

void	User::sendRPLUSERS() const
{
	std::stringstream msgBuilder;
	/* LUSERCLIENT */
	msgBuilder << "There are " << this->_server->getNbOfClients()
		<< " users and 0 invisible on 0 servers";
	this->_author->writeRPLToClient(this->_server, RPL_LUSERCLIENT,
			msgBuilder.str());
	msgBuilder.ignore();
	/* LUSERME */
	msgBuilder << "I have " << this->_server->getNbOfClients()
		<< " clients and 0 servers";
	this->_author->writeRPLToClient(this->_server, RPL_LUSERME,
		msgBuilder.str());
	msgBuilder.ignore();
	/* LUSEROP */
	msgBuilder << this->_server->getNbOfOps();
	this->_author->writeRPLToClient(this->_server, RPL_LUSEROP,
			msgBuilder.str(), MSG_LUSEROP);
	msgBuilder.ignore();
	/* LUSERCHANNELS */
	msgBuilder << this->_server->getNbOfChannels();
	this->_author->writeRPLToClient(this->_server, RPL_LUSERCHANNELS,
			msgBuilder.str(), MSG_LUSERCHANNELS);
	msgBuilder.ignore();
}
