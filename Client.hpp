
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

		/* Setters */
		void	setNickname(std::string name);

		/* Getters */
		int	getSocketFD() const;
		std::string getNickname() const;
		std::string getFullName() const;
		const std::set<Channel*> &getConnectedChannels() const;

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
		std::string	_nickname;
		std::string _username;
		std::string	_hostname;
		std::set<Channel*> _connectedChannels;
	protected:
};

#endif
