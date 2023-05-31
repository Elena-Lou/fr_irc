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
	
}

void Kick::error( std::string ) const
{
	std::cout << "Error()" << std::endl;
}