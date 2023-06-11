#include "Join.hpp"

Join::Join() : ACommand()
{
	/*this is forbidden */
}

Join::~Join()
{
#if SHOW_CONSTRUCTOR
	std::cout << "Join destructor" << std::endl;
#endif
}

Join::Join(Server &server, Client &user, std::string rawInput) : ACommand(server, user, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Join full constructor" << std::endl;
#endif
	this->execute();
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
	{
		std::cout << "User is not registered" << std::endl;
		return ;
	}
	if (this->_cmd.size() < 2)
	{
		error(ERR_NEEDMOREPARAMS);
		return;
	}
	if (this->_cmd[1] == "0" && this->_cmd.size() == 2)
	{
		/* quit all channels */
		std::cout << "quitting all channels" << std::endl;
		std::set<Channel*> connectedChans = this->_author->getConnectedChannels();
		for (std::set<Channel*>::iterator it = connectedChans.begin();
			it != connectedChans.end();)
		{
			if ((*it)->removeUserFromChannel(*this->_author) == 0)
			{
				std::set<Channel*>::iterator tmp = it;
				it++;
				this->_server->destroyChannel(**tmp);
				continue;
			}
			it++;
		}
		return ;
	}
	if ((this->_target = this->_server->getChannelIfExist(this->_cmd[1])) == NULL
			&& this->_server->isUserConnected(this->_cmd[1]))
	{
		error(ERR_NOSUCHCHANNEL);
		return;
	}
	if (this->_target && this->_target->isUserConnected(*this->_author))
	{
		std::cout << "Client is already connected to the channel" << std::endl;
		//client is already connected to the channel
		return;
	}
	if (0) //TODO maximum channel per user
	{
		error(ERR_TOOMANYCHANNELS);
		return;
	}
	if (!verifyChannelName())
	{
		error(ERR_BADCHANMASK);
		return;
	}
	if (this->_target && ((this->_target->isProtected())
		&& (this->_cmd.size() < 3 || !this->_target->tryPassword(this->_cmd[2]))))
	{
			error(ERR_BADCHANNELKEY);
			return ;
	}
	if (0) // TODO banned from chan (not asked by current subject)
	{
		error(ERR_BANNEDFROMCHAN);
		return;
	}
	if (0) //TODO channel is full (mode to do)
	{
		error(ERR_CHANNELISFULL);
		return;
	}
	if (0) //TODO invite only chan (mode to do)
	{
		error(ERR_INVITEONLYCHAN);
		return;
	}
	this->confirm();
}

void Join::error(int errorCode) const
{
	switch (errorCode)
	{
		case ERR_NEEDMOREPARAMS:
		{
			this->_author->writeRPLToClient(this->_server,
					ERR_NEEDMOREPARAMS, this->_cmd[0], MSG_NEEDMOREPARAMS);
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
	Channel *toJoin = this->_server->getChannelIfExist(this->_cmd[1]);
	std::string topic;
	if (toJoin)
	{
		toJoin->addUserToChannel(*this->_author);
		toJoin->getTopic();
	}
	else
	{
		this->_server->createChannel(this->_cmd[1], *this->_author);
		toJoin = this->_server->getChannelIfExist(this->_cmd[1]);
	}
	/* confirm JOIN message */
	toJoin->broadcastToChannel(this->_author->getNickname() + " JOIN " + this->_cmd[1]);
	/* Send topic if set */
	if (topic != "")
		this->_author->writeRPLToClient(this->_server, RPL_TOPIC,
			this->_cmd[1], topic);
	/* list of all users currently in the channel */
	toJoin->sendAllNamesToUser(*this->_server, *this->_author);
	/* END OF NAMES */
	this->_author->writeRPLToClient(this->_server, RPL_ENDOFNAMES,
			this->_cmd[1], MSG_ENDOFNAMES);
}

bool	Join::verifyChannelName()
{

	if (this->_cmd.size() < 2)
	{
		return (false);
	}
	if (this->_cmd[1][0] != '#' || this->_cmd[1].size() < 2)
		return (false);
	for (unsigned int i = 1; i < this->_cmd[1].size(); i++)
	{
		if (!std::isalnum(this->_cmd[1][i]))
			return (false);
	}
	return (true);

}
