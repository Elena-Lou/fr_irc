#include "Oper.hpp"

Oper::Oper() : ACommand()
{
	/*this is forbidden */
}

Oper::~Oper()
{
#if SHOW_CONSTRUCTOR
	std::cout << "OPER destructor" << std::endl;
#endif
}

Oper::Oper(Server & server, Client & user, std::string rawInput) : ACommand(server, user, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "OPER overloaded constructor" << std::endl;
#endif
}

Oper::Oper(Oper const & src)
{
#if SHOW_CONSTRUCTOR
	std::cout << "OPER copy constructor" << std::endl;
#endif
	*this = src;
}

Oper & Oper::operator=(Oper const & rhs)
{
    if (this != &rhs)
    {
		this->_cmd = rhs._cmd;
		this->_author = rhs._author;
    	this->_server = rhs._server;
    }
	return *this;
}

void Oper::execute()
{
	std::cout << "execute()" << std::endl;
}

void Oper::error( int errorCode ) const
{
	std::cout << "error()" << std::endl;
}

void Oper::confirm() const
{

}