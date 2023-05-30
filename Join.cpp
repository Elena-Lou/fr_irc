#include "Join.hpp"

Join::Join() : ACommand()
{
	/*this is forbidden */
}

Join::~Join()
{
}

Join::Join(Client &user, std::string rawInput) : ACommand(user, rawInput)
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

