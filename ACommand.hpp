#ifndef ACOMMAND_HPP
# define ACOMMAND_HPP

# include <deque>
# include <string>
# include <sstream>

class Client;
class Server;
class ACommand
{
	public:
		virtual ~ACommand();

	protected:
		ACommand();
		ACommand(Server &server, Client &author, std::string cmd);
		ACommand(const ACommand &source);
		ACommand &operator=(const ACommand &rhs);


		virtual void	execute() = 0;
		virtual void	error(int errorCode) const = 0;
		virtual void	confirm() const = 0;

		//TODO: create a bool for the target to be used in derived classes
		//bool	targetIsUserOrChannel;
		//void	*_target;

		Server	*_server;
		Client	*_author;
		std::deque<std::string>	_cmd;
	private:
		void	tokenise(std::string rawInput);
};

#endif
