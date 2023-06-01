#ifndef JOIN_HPP
# define JOIN_HPP

# include <list>
# include <string>
# include "ACommand.hpp"

# define ERR_NEEDMOREPARAMS 461
# define ERR_BANNEDFROMCHAN 474
# define ERR_INVITEONLYCHAN 473
# define ERR_BADCHANNELKEY 475
# define ERR_CHANNELISFULL 471
# define ERR_BADCHANMASK 476
# define ERR_NOSUCHCHANNEL 403
# define ERR_TOOMANYCHANNELS 405
# define RPL_TOPIC 332

class Client;
class Server;
class Join : public ACommand
{
	public:
		~Join();

	protected:
		Join(Server &server, Client &user, std::string rawInput);
		Join(const Join &source);
		Join &operator=(const Join &rhs);

		void	execute() const;
		void	error(std::string) const;

	private:
		Join();
};

#endif
