#ifndef NICK_HPP
# define NICK_HPP

# include <list>
# include <string>

class Client;
class Server;
class Nick : public ACommand
{
	public:
		~Nick();
		Nick(Server &server, Client &author, std::string rawInput);
		Nick(const Nick &source);
		Nick &operator=(const Nick &rhs);

	protected:

	private:
		Nick();
		void	execute() const;
		void	confirm() const;
		void	error(int errorCode) const;
};

#endif
