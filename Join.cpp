#include "Join.hpp"
#include <vector>

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

Join::Join(Server &server, Client &author, std::string rawInput) : ACommand(server, author, rawInput)
{
#if SHOW_CONSTRUCTOR
	std::cout << "Join full constructor" << std::endl;
#endif
	this->inputToList();
	for (std::map<std::string, std::string>::iterator it = this->_chanList.begin();
		it != this->_chanList.end(); it++)
	{
		this->_chanName = it->first;
		this->_password = it->second;
		this->execute();
	}
	if (!this->_chanList.size())
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
		return ;
	if (this->_cmd.size() < 2)
	{
		error(ERR_NEEDMOREPARAMS);
		return;
	}
	if (this->_chanName == "0" && this->_cmd.size() == 2)
	{
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
	if ((this->_target = this->_server->getChannelIfExist(this->_chanName)) == NULL
			&& this->_server->isUserConnected(this->_chanName))
	{
		error(ERR_NOSUCHCHANNEL);
		return;
	}
	if (this->_target && this->_target->isUserConnected(*this->_author))
		return;
	if (this->_author->getConnectedChannels().size() >= MAX_CHAN_PER_USER)
	{
		error(ERR_TOOMANYCHANNELS);
		return;
	}
	if (!verifyChannelName(this->_chanName))
	{
		error(ERR_BADCHANMASK);
		return;
	}
	if (this->_target && ((this->_target->isProtected())
		&& (this->_cmd.size() < 3 || !this->_target->tryPassword(this->_password))))
	{
		error(ERR_BADCHANNELKEY);
		return ;
	}
	if (0) // TODO banned from chan (not asked by current subject)
	{
		error(ERR_BANNEDFROMCHAN);
		return;
	}
	if (this->_target && this->_target->isMode(CHANLIMIT_MODE)
		&& this->_target->getNbClients() >= this->_target->getMaxClients()) //channel is full
	{
		error(ERR_CHANNELISFULL);
		return;
	}
	if (this->_target && this->_target->isMode(INVITE_MODE)) // invite only chan )
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
			break;
		}
		case ERR_NOSUCHCHANNEL:
		{
			this->_author->writeRPLToClient(this->_server, ERR_NOSUCHCHANNEL,
					this->_cmd[1], MSG_NOSUCHCHANNEL);
			break;
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
	}
}

void Join::confirm() const
{
	Channel *toJoin = this->_server->getChannelIfExist(this->_chanName);
	std::string topic;
	if (toJoin)
	{
		toJoin->addUserToChannel(*this->_author);
		toJoin->getTopic();
	}
	else
	{
		this->_server->createChannel(this->_chanName, *this->_author);
		toJoin = this->_server->getChannelIfExist(this->_chanName);
	}
	/* confirm JOIN message */
	toJoin->broadcastToChannel(this->_author->getNickname() + " JOIN " + this->_chanName);
	/* Send topic if set */
	if (topic != "")
	{
		this->_author->writeRPLToClient(this->_server, RPL_TOPIC,
			this->_chanName, topic);
		toJoin->sendTOPICWHOTIME(*this->_server, *this->_author);
	}
	/* list of all users currently in the channel */
	toJoin->sendAllNamesToUser(*this->_server, *this->_author);
	/* END OF NAMES */
	this->_author->writeRPLToClient(this->_server, RPL_ENDOFNAMES,
		this->_chanName, MSG_ENDOFNAMES);
}

bool	Join::verifyChannelName(std::string name)
{

	if (this->_cmd.size() < 2)
		return (false);
	if (name[0] != '#' || name.size() < 2)
		return (false);
	for (unsigned int i = 1; i < name.size(); i++)
	{
		if (!std::isalnum(name[i]))
			return (false);
	}
	return (true);

}

void	Join::inputToList()
{
	std::vector<std::string> names;
	std::vector<std::string> passwords;

	std::string::iterator start = this->_cmd[1].begin();
	std::string::iterator it = start;
	while (start != this->_cmd[1].end()) //raw input not over
	{
		if (*it == ',' || it == this->_cmd[1].end())
		{
			names.push_back(std::string(start, it));
			start = it + 1;
		}
		if (it == this->_cmd[1].end())
			start = this->_cmd[1].end();
		it++;
	}
	if (this->_cmd.size() >= 3)
	{
		start = this->_cmd[2].begin();
		std::string::iterator it = start;
		while (start != this->_cmd[2].end()) //raw input not over
		{
			if (*it == ',' || it == this->_cmd[2].end())
			{
				if (it == start)
					passwords.push_back("");
				else
					passwords.push_back(std::string(start, it));
				start = it + 1;
			}
			if (it == this->_cmd[2].end())
				start = this->_cmd[2].end();
			it++;
		}
	}
	for (unsigned int i = 0; i < names.size(); i++)
	{
		if (i < passwords.size())
			this->_chanList.insert(std::make_pair(names[i], passwords[i]));
		else
			this->_chanList.insert(std::make_pair(names[i], ""));
	}
}
