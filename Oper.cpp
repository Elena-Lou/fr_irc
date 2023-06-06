#include "Oper.hpp"

Oper::Oper() : ACommand()
{
	/*this is forbidden */
}

Oper::~Oper()
{
}

Oper::Oper(Server & server, Client & user, std::string rawInput) : ACommand(server, user, rawInput)
{
}

Oper::Oper(Oper const & src)
{
	*this = src;
}

Oper & Oper::operator=(Oper const & rhs)
{
    if (this != &rhs)
    {
		this->_cmd = rhs._cmd;
		this->_user = rhs._user;
    	this->_server = rhs._server;
    }
	return *this;
}

void Oper::execute() const
{
	std::cout << "execute()" << std::endl;
}

void Oper::error( int errorCode) const
{
	std::cout << "error()" << std::endl;
}