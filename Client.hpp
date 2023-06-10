
#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "irc.hpp"

class Server;
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
		void	setUsername(std::string name);
		void	setHostname(std::string name);
		void	setRealname(std::string name);
		void	confirmRegistration();
		void	validatePassword();

		/* Getters */
		int	getSocketFD() const;
		std::string getNickname() const;
		std::string	getUsername() const;
		std::string getFullName() const;
		const std::set<Channel*> &getConnectedChannels() const;
		bool	isPasswordOk() const;


		/* writing */
		void	writeToClient(std::string message);
		void	writeRPLToClient(Server *server, int RPL, std::string message);
		void	writeRPLToClient(Server *server, int RPL, std::string additionalSource, std::string message);

		/* channel handlers */
		int		isInChannel(Channel&) const;
		void	joinChannel(Channel&);
		void	quitChannel(Channel&);

		/* storage */
		std::string	readBuffer;
		std::string writeBuffer;
	private:
		Client();

		/* Attributes */
		int			_socketFD;
		std::string	_nickname;
		std::string _username;
		std::string	_hostname;
		std::string	_realname;

		bool	_registered;
		bool	_passwordOK;

		std::set<Channel*> _connectedChannels;
	protected:
};

#endif
