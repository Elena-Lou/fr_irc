#ifndef INVITE_HPP
# define INVITE_HPP

# include "irc.hpp"
# include "ACommand.hpp"

class Client;
class Server;
class Invite : public ACommand
{
	public:
		Invite(Server &server, Client &author, std::string rawInput);
		Invite(const Invite &source);
		Invite &operator=(const Invite &rhs);
		~Invite();

	protected:
	private:
		Invite();
		void	execute();
		void	error(int) const;
		void	confirm() const;

		Channel *_targetChannel;
		Client	*_targetClient;
};

#endif
