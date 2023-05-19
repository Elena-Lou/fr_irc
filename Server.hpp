#ifndef SERVER_HPP
# define SERVER_HPP

#include "irc.hpp"

# define ERRSOCKFD 0b1
# define ERRSOCKOPT 0b10
# define ERRSOCKNOBLOCK 0b100
# define ERRSOCKBIND 0b1000

class Server
{
	public:
		Server();
		~Server();
		Server(const char *portNumber, const char *domain = NULL);
		Server(const Server &source);
		Server& operator=(const Server &rhs);

		int		getSocketFD();
		void	closeSocketFD();
		void	socketErrorHandler(int errorBitField) const;

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

};


#endif
