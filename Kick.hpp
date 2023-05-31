#ifndef KICK_HPP
# define KICK_HPP

# include "ACommand.hpp"
# include "irc.hpp"

class Client;
class Server;
class Kick : public ACommand
{
	private:
	protected:
	public:
		
		Kick();
		Kick( Kick const & src );
		Kick & operator=( Kick const & rhs );
		~Kick();

		void execute( void ) const;
		void error( std::string ) const;
};

#endif