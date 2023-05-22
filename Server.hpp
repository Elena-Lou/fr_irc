#ifndef SERVER_HPP
# define SERVER_HPP

#include "irc.hpp"

# define ERRSOCKFD 0b1
# define ERRSOCKOPT 0b10
# define ERRSOCKNOBLOCK 0b100
# define ERRSOCKBIND 0b1000
# define ERRSOCKLISTEN 0b10000

# define MYIRC_PORT "3490"
# define MYIRC_ALLOWED_PENDING_CONNECTIONS 5


# include <set>
# include "Client.hpp"

class Client;
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
		struct addrinfo *getServinfo() const;
		struct sockaddr *getSockaddr() const;

		void	startListening() const;

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
		struct addrinfo	hints;
		struct addrinfo	*_servinfo;
		int				_socketFD;
		std::set<Client> _clients;
};


#endif
