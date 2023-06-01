#include "Kick.hpp"

Kick::Kick() : ACommand()
{
}

Kick::Kick( Kick const & src )
{
	*this = src;
}

Kick::Kick(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
	std::cout << "KICK overloaded constructor" << std::endl;
	std::cout << "client socketFD : " << this->_author->getSocketFD() << std::endl;
	this->execute();
}

Kick & Kick::operator=( Kick const & rhs )
{
	if (this != &rhs)
	{
		this->_cmd = rhs._cmd;
		this->_server = rhs._server;
		this->_author = rhs._author;
	}
	return *this;
}

Kick::~Kick()
{
	std::cout << "KICK destructor" << std::endl;
}

void Kick::execute() const
{
	if (this->_cmd.size() < 3)
	{
		error(ERR_NEEDMOREPARAMS);
		return ;
	}

	/* Does the channel exist ? */
	Channel *foundChannel;
	if ((foundChannel = this->_server->getChannelIfExist(this->_cmd[1])) == NULL)
	{
		error(ERR_NOSUCHCHANNEL);
		return ;
	}

	/* Is the user connected to that channel ? */
	if (!foundChannel->isUserConnected(*(this->_author)))
	{
		error(ERR_NOTONCHANNEL);
		return ;
	}

	/* Is the user a Channel Operator ? */
	if (!foundChannel->isChannelOperator(*(this->_author)))
	{
		error(ERR_CHANOPRIVSNEEDED);
		return ;
	}

	/* Is the user the user wants to kick conected to that channel ?*/
	std::string targetName = this->_cmd[2];
		if (!foundChannel->isUserConnected(*(this->_author)))
	{
		error(ERR_NOTONCHANNEL);
		return ;
	}

}

void Kick::error( int errorCode ) const
{
	std::string errorMsg;
	errorMsg += errorCode;
	errorMsg += ":";
	errorMsg += this->_author->getNickname();
	errorMsg += this->_cmd[0];
	errorMsg += "\n";
// switch (errorCode)
// {
// case /* constant-expression :*/
// 	/* this->_author->write_buffer += errorMsg; */
// 	// break;

// // default:
// // 	break;
// }
	std::cout << "Error()" << std::endl;
}