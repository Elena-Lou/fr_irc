
#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "irc.hpp"

class Channel;
class Client
{
	public:
		~Client();
		Client(int);

		void	setUsername(std::string name);

		/* Getters */
		int	getSocketFD() const;

		std::string	readBuffer;
		std::string writeBuffer;

		/* channel handlers */
		int		isInChannel(Channel&) const;
		void	joinChannel(Channel&);
		std::set<Channel*> _connectedChannels;
	private:
		Client();
		Client(const Client &source);
		Client& operator=(const Client &rhs);

		/* Attributes */
		int			_socketFD;
		std::string	_username;
	protected:
};

#endif
