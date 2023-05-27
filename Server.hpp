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

class Client;
class Channel;
class Server
{
	public:
		Server();
		~Server();
		Server(const char *portNumber, const char *domain = NULL);
		Server(const Server &source);
		Server& operator=(const Server &rhs);

		void	closeSocketFD();
		void	socketErrorHandler(int errorBitField) const;

		/* Getters */
		int		getSocketFD() const;
		std::map<int, Client> &getClients();

		/* Client handlers */
		void	addUser(int);
		void	disconnectUser(int);
		void	disconnectUser(std::map<int, Client>::iterator clientIterator);
		void	checkNewConnections();

		/* Channel handlers */
		void	createChannel(std::string, Client&);
		void	destroyChannel(const Channel&);
		void	destroyChannel(std::string);

		/* read/write loops and set handler */
		int		fillSets();
		void	readLoop();
		void	writeLoop();

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
	private:
	protected:
		int				_socketFD;
		int				_fdMax;
		std::map<int, Client> _clients;
		fd_set		_masterSet;
		fd_set		_readingSet;
		fd_set		_writingSet;
		struct sockaddr_storage	_pendingAddr;
		socklen_t				_pendingAddrSize;
		char	buffer[IRC_BUFFER_SIZE];
		std::list<Channel>	_channels;
};


#endif
