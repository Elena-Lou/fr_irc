#ifndef KICK_HPP
# define KICK_HPP

# include "ACommand.hpp"
# include "irc.hpp"

class Client;
class Server;
class Kick : public ACommand
{
	private:
		Kick();
		Channel* _foundChannel;

	protected:
	public:

		Kick(Server &server, Client &author, std::string rawInput);
		Kick( Kick const & src );
		Kick & operator=( Kick const & rhs );
		~Kick();

		void	execute( void );
		void	error( int errorCode ) const;
		void	confirm() const;
};

#endif