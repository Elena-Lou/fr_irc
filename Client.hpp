
#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "irc.hpp"

class Channel;
class Client
{
	public:
		~Client();
		Client(const Client &source);
		Client& operator=(const Client &rhs);
		Client(int);

		void	setUsername(std::string name);

		/* Getters */
		int	getSocketFD() const;

		std::string	readBuffer;
		std::string writeBuffer;

		/* channel handlers */
		int		isInChannel(Channel&) const;
		void	joinChannel(Channel&);
		void	quitChannel(Channel&);
	private:
		Client();

		/* Attributes */
		int			_socketFD;
		std::string	_username;
		std::set<Channel*> _connectedChannels;
	protected:
};

#endif
