
#ifndef TOPIC_HPP
# define TOPIC_HPP

# include "irc.hpp"
# include "ACommand.hpp"

class Client;
class Server;
class Topic : public ACommand
{
	public:
		Topic(Server &server, Client &user, std::string rawInput);
		Topic(const Topic &source);
		Topic &operator=(const Topic &rhs);
		~Topic();

	protected:
	private:
		Topic();
		void	execute();
		void	error(int) const;
		void	confirm() const;

		Channel *_target;
		std::string _chanName;
		std::string _message;
};

#endif
