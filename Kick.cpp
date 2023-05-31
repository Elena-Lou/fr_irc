#include "Kick.hpp"

Kick::Kick( void ) : ACommand()
{
	std::cout << "KICK constructor" << std::endl;
}

Kick::Kick( Kick const & src )
{
	*this = src;
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
	std::cout << "Execute()" << std::endl;
}

void Kick::error( std::string ) const
{
	std::cout << "Error()" << std::endl;
}