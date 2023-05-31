#ifndef ACOMMAND_HPP
# define ACOMMAND_HPP

# include <list>
# include <string>

class Client;
class Server;
class ACommand
{
	public:
		virtual ~ACommand();

	protected:
		ACommand();
		ACommand(Server &server, Client &user, std::string cmd);
		ACommand(const ACommand &source);
		ACommand &operator=(const ACommand &rhs);


		virtual void	execute() const = 0;
		virtual void	error(std::string) const = 0;

		//TODO: create a bool for the target to be used in derived classes
		//bool	targetIsUserOrChannel;
		//void	*_target;

		Server	*_server;
		Client	*_user;
		std::list<std::string>	_cmd;
	private:
		void	tokenise(std::string rawInput);
};

#endif
