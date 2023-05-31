#include "Kick.hpp"

Kick::Kick( void ) : ACommand()
{
}

Kick::Kick( Kick const & src )
{
	*this = src;
}

Kick::Kick(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
	std::cout << "KICK overloaded constructor" << std::endl;
	std::cout << "client socketFD : " << this->_user->getSocketFD() << std::endl;
	this->execute();
}

Kick & Kick::operator=( Kick const & rhs )
{
	if (this != &rhs)
	{
		this->_cmd = rhs._cmd;
		this->_server = rhs._server;
		this->_user = rhs._user;
	}
	return *this;
}

Kick::~Kick()
{
	std::cout << "KICK destructor" << std::endl;
}

void Kick::execute( void ) const
{
	if (this->_cmd.size() < 2)
	{
		error(ERR_NEEDMOREPARAMS);
		return ;
	}

	/* Does the channel exist ? */
	std::deque<Channel> serverChannelsCopy = this->_server->getChannels();
	std::deque<Channel>::iterator it;
	for (it = serverChannelsCopy.begin(); it != serverChannelsCopy.end(); it++)
	{
		if (this->_cmd[1] == it->getName())
		break ;
	}
	if (it == serverChannelsCopy.end())
	{
		error(ERR_NOSUCHCHANNEL);
		return ;
	}

	/* Is the user a Channel Operator ? */
	if (it->isChannelOperator(*(this->_user)))
	{
		
	}

	// /* Is the user connected to that channel ? */
	// 	-> map of connected users _connectedClients
	// 	=> method isUserConnected(Client & user)

	// /* Is the user the user wants to kick conected to that channel ?*/
	// 	=> method isUserConnected(Client & user)
	
	// if error
	// 	error(int)
	// */
	
}

void Kick::error( int errorCode ) const
{
	std::string errorMsg;
	errorMsg += errorCode;
	errorMsg += ":";
	errorMsg += this->_user->getUsername();
	errorMsg += this->_cmd[0];
	errorMsg += "\n";
// switch (errorCode)
// {
// case /* constant-expression :*/
// 	/* this->_user->write_buffer += errorMsg; */
// 	// break;

// // default:
// // 	break;
// }
	std::cout << "Error()" << std::endl;
}