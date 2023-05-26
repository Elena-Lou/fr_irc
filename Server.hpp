#ifndef SERVER_HPP
# define SERVER_HPP

#include "irc.hpp"
# include <set>
# include "Client.hpp"
# include <exception>

# define ERRSOCKFD 0b1
# define ERRSOCKOPT 0b10
# define ERRSOCKNOBLOCK 0b100
# define ERRSOCKBIND 0b1000
# define ERRSOCKLISTEN 0b10000

# define MYIRC_PORT "3490"
# define MYIRC_ALLOWED_PENDING_CONNECTIONS 5

# define MYIRC_TIMEOUT 30

class Client;
class Channel;
class Server
{
	public:
		Server();
		~Server();
		Server(const char *portNumber, const char *domain = NULL);

		void	closeSocketFD();
		void	socketErrorHandler(int errorBitField) const;

		/* Getters */
		int		getSocketFD() const;
		std::map<int, Client> &getClients();

		/* Client handlers */
		void	addUser(int);
		void	deleteUser(int);
		void	checkNewConnections();

		/* Channel handlers */
		void	createChannel(std::string);
		void	destroyChannel(Channel&);
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
		Server(const Server &source);
		Server& operator=(const Server &rhs);
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
		std::set<Channel>	_channels;
};


#endif
