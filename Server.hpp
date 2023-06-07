#ifndef SERVER_HPP
# define SERVER_HPP

#include "irc.hpp"
# include "Client.hpp"
# include <exception>

# define ERRSOCKFD 0b1
# define ERRSOCKOPT 0b10
# define ERRSOCKNOBLOCK 0b100
# define ERRSOCKBIND 0b1000
# define ERRSOCKLISTEN 0b10000
# define ERRNAMERESOLVE 0b100000

# define HOST_BUFFER_SIZE 255
# define SERV_BUFFER_SIZE 255

class Client;
class Channel;
class ACommand;
class Server
{
	public:
		Server();
		~Server();
		Server(const char *portNumber);
		Server(const Server &source);
		Server& operator=(const Server &rhs);

		void	closeSocketFD();
		void	socketErrorHandler(unsigned int errorBitField) const;

		/* Getters */
		int						getSocketFD() const;
		const std::string		&getHostname() const;
		std::map<int, Client>	&getClients();
		std::deque<Channel>		&getChannels();

		/* Client handlers */
		void	connectUser(int);
		void	disconnectUser(int);
		void	disconnectUser(std::map<int, Client>::iterator clientIterator);
		void	checkNewConnections();
		bool	isUserConnected(std::string) const;
		Client	*getUserIfConnected(std::string nickName);

		/* Channel handlers */
		void	createChannel(std::string, Client&);
		void	destroyChannel(Channel&);
		void	destroyChannel(std::string);
		Channel	*getChannelIfExist(std::string chanName);

		/* read/write loops and set handler */
		int		fillSets();
		void	readLoop();
		void	writeLoop();

		/* messages */
		void	broadcastAllClients(std::string &msg);
		void	broadcastChannel(Channel& targetChan, std::string &msg);

		/* COMMANDS */
		void	JOIN(ACommand &placeholder);
		/* parsing commands */
		bool	checkRawInput(std::string & rawInput);
		void	parsingCommand(std::string & rawInput, Client & user);

		/* exceptions */
		class CannotRetrieveAddrinfoException : public std::exception
		{
			public:
				const char* what(void) const throw();
		};

		class CannotRetrieveSocketException : public std::exception
		{
			public:
				const char* what(void) const throw();
		};

		class InterruptionSignalException : public std::exception
		{
			public:
				const char* what(void) const throw();
		};

	private:
		int				_socketFD;
		int				_fdMax;
		std::string	_hostname;
		fd_set		_masterSet;
		fd_set		_readingSet;
		fd_set		_writingSet;
		struct sockaddr_storage	_pendingAddr;
		socklen_t				_pendingAddrSize;
		char	buffer[IRC_BUFFER_SIZE];

		std::map<int, Client> _clients;
		std::deque<Channel>	_channels;
};


#endif
