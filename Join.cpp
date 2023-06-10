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
	if (!this->_author->isRegistered())
		return ;
}

void Join::error(int errorCode) const
{
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS);
			return;
		}
		case ERR_NOSUCHCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NOSUCHCHANNEL,
					this->_cmd[1], MSG_NOSUCHCHANNEL);
			return;
		}
		case ERR_TOOMANYCHANNELS:
		{
			this->_author->writeRPLToClient(this->_server, ERR_TOOMANYCHANNELS,
					this->_cmd[1], MSG_TOOMANYCHANNELS);
			break;
		}
		case ERR_BADCHANNELKEY:
		{
			this->_author->writeRPLToClient(this->_server, ERR_BADCHANNELKEY,
					this->_cmd[1], MSG_BADCHANNELKEY);
			break;
		}
		case ERR_BANNEDFROMCHAN:
		{
			this->_author->writeRPLToClient(this->_server, ERR_BANNEDFROMCHAN,
					this->_cmd[1], MSG_BANNEDFROMCHAN);
			break;
		}
		case ERR_CHANNELISFULL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_CHANNELISFULL,
					this->_cmd[1], MSG_CHANNELISFULL);
			break;
		}
		case ERR_INVITEONLYCHAN:
		{
			this->_author->writeRPLToClient(this->_server, ERR_INVITEONLYCHAN,
					this->_cmd[1], MSG_INVITEONLYCHAN);
			break;
		}
		case ERR_BADCHANMASK:
		{
			this->_author->writeRPLToClient(this->_server, ERR_BADCHANMASK,
					MSG_BADCHANMASK);
			break;
		}
		default:
			std::cerr << "Error: unrecognised error code in JOIN." << std::endl;
			break;
	}
}

void Join::confirm() const
{
}
