#include "Join.hpp"

Join::Join() : ACommand()
{
	/*this is forbidden */
}

Join::~Join()
{
}

Join::Join(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
}

Join::Join(const Join &source)
{
	*this = source;
}

Join &Join::operator=(const Join &rhs)
{
	this->_cmd = rhs._cmd;
	this->_user = rhs._user;
	return (*this);
}

