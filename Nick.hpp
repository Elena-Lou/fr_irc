#ifndef NICK_HPP
# define NICK_HPP

# include <list>
# include <string>

# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKNAMEINUSE 433
# define ERR_NICKCOLLISION 436

class Client;
class Server;
class Nick : public ACommand
{
	public:
		~Nick();

	protected:
		Nick(Server &server, Client &user, std::string rawInput);
		Nick(const Nick &source);
		Nick &operator=(const Nick &rhs);

		void	execute() const;
		void	error(int errorCode) const;

	private:
		Nick();
};

#endif
