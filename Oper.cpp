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

Oper::Oper(Server & server, Client & author, std::string rawInput) : ACommand(server, author, rawInput)
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
	/* is the author registered ?*/
	if (!this->_author->isRegistered())
		return ;
	
	/* are there three params : command, user, pwd ? */
	if (this->_cmd.size() != 3)
	{
		error(ERR_NEEDMOREPARAMS);
		return ;
	}

	/* is the provided password correct ? */
	if (this->_cmd[2] != this->_server->getPassword())
	{
		error(ERR_PASSWDMISMATCH);
		return ;
	}

	/* is there something preventing the author from becoming an operator ? */
	// to be determined when MODE is coded
	this->confirm();
}

void Oper::error( int errorCode ) const
{
	switch (errorCode)
	{
	case ERR_NEEDMOREPARAMS:
	{
		this->_author->writeRPLToClient(this->_server,
			ERR_NEEDMOREPARAMS, this->_cmd[0], MSG_NEEDMOREPARAMS);
		break ;
	}
	case ERR_PASSWDMISMATCH:
	{
		this->_author->writeRPLToClient(this->_server,
			ERR_PASSWDMISMATCH, MSG_PASSWDMISMATCH);
		break ;
	}
	case ERR_NOOPERHOST:
	{
		this->_author->writeRPLToClient(this->_server,
			ERR_NOOPERHOST, MSG_NOOPERHOST);
		break;
	}
	default:
		std::cerr << "Error: Unrecognised error code." << std::endl;
		break;
	}
}

void Oper::confirm() const
{
	this->_author->writeRPLToClient(this->_server,
		RPL_YOUROPER, MSG_YOUROPER);
}
