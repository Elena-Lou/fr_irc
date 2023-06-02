#ifndef KICK_HPP
# define KICK_HPP

# include "ACommand.hpp"
# include "irc.hpp"

# define ERR_NEEDMOREPARAMS 461
# define ERR_NOSUCHCHANNEL 403
# define ERR_NOTONCHANNEL 442
# define ERR_BADCHANMASK 476
# define ERR_CHANOPRIVSNEEDED 482
# define ERR_USERNOTINCHANNEL 441

class Client;
class Server;
class Kick : public ACommand
{
	private:
		Kick();

	protected:
	public:

		Kick(Server &server, Client &user, std::string rawInput);
		Kick( Kick const & src );
		Kick & operator=( Kick const & rhs );
		~Kick();

		void execute( void ) const;
		void error( int errorCode ) const;
};

#endif