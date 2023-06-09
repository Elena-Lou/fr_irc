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

Join::Join(const Join &source) : ACommand(source)
{
	*this = source;
}

Join &Join::operator=(const Join &rhs)
{
	this->_cmd = rhs._cmd;
	this->_server = rhs._server;
	this->_author = rhs._author;
	return (*this);
}

void Join::execute()
{
}

void Join::error(int) const
{
}

void Join::confirm() const
{
}
