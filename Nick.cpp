
#include "Nick.hpp"

Nick::Nick() : ACommand()
{
	/*this is forbidden */
}

Nick::~Nick()
{
}

Nick::Nick(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
}

Nick::Nick(const Nick &source)
{
	*this = source;
}

Nick &Nick::operator=(const Nick &rhs)
{
	this->_cmd = rhs._cmd;
	this->_user = rhs._user;
	return (*this);
}

