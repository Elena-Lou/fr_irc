

# include "ACommand.hpp"
# include "Ping.hpp"
# include "Client.hpp"
# include "irc.hpp"

Ping::Ping() : ACommand()
{
	/*this is forbidden */
}

Ping::~Ping()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Ping destructor" << std::endl;
#endif
}

Ping::Ping(Server &server, Client &author, std::string rawInput) : ACommand(server, author, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Ping full constructor" << std::endl;
#endif
	{
		if (this->_cmd.size() >= 2)
		{
			unsigned int offset = rawInput.find(this->_cmd[1]);
			this->_message = std::string(rawInput, offset);
		}
	}
	this->execute();
}

Ping::Ping(const Ping &source) : ACommand(source)
{
	*this = source;
}

Ping &Ping::operator=(const Ping &rhs)
{
	this->_cmd = rhs._cmd;
	this->_server = rhs._server;
	this->_author = rhs._author;
	return (*this);
}

void Ping::execute()
{
	if (!this->_author->isRegistered())
		return;
	if (this->_cmd.size() < 2)
	{
		error(ERR_NEEDMOREPARAMS);
		return;
	}
	//NOSUCHSERVER not in subject
	this->confirm();
}

void Ping::error(int errorCode) const
{
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NEEDMOREPARAMS,
				this->_cmd[1], MSG_NEEDMOREPARAMS);
			break;
		}
		default:
			std::cerr << "Error: Unrecognised error code." << std::endl;
			break;
	}
}

void	Ping::confirm() const
{
	std::stringstream msgBuilder;

	msgBuilder << ":" << this->_server->getHostname() << " PONG :"
		<< this->_message;
	this->_author->writeToClient(msgBuilder.str());
}
