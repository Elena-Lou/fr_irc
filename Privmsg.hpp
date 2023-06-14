#ifndef PRIVMSG_HPP
# define PRIVMSG_HPP

# include <list>
# include <string>

class Client;
class Server;
class Privmsg : public ACommand
{
	public:
		~Privmsg();
		Privmsg(Server &server, Client &author, std::string rawInput);
		Privmsg(const Privmsg &source);
		Privmsg &operator=(const Privmsg &rhs);

	protected:

	private:
		Privmsg();
		void	execute();
		void	confirm() const;
		void	error(int errorCode) const;

		bool	verifyChannelName(std::string name);
		std::string _message;
};

#endif
