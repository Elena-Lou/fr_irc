#include "ACommand.hpp"


ACommand::ACommand()
{
}

ACommand::~ACommand()
{
}

ACommand::ACommand(Server &server, Client &author, std::string rawInput)
{
	this->_server = &server;
	this->_author = &author;
	this->tokenise(rawInput);
}

ACommand::ACommand(const ACommand &source)
{
	*this = source;
}

ACommand &ACommand::operator=(const ACommand &rhs)
{
	this->_cmd = rhs._cmd;
	this->_author = rhs._author;
	return (*this);
}

void	ACommand::tokenise(std::string rawInput)
{
	std::string::iterator start = rawInput.begin();
	std::string::iterator it = start;
	while (it != rawInput.end())//raw input not over
	{
		if (isspace(*it))
		{
			this->_cmd.push_back(std::string(start, it));
			start = it;
		}
		it++;
	}
}
