#ifndef JOIN_HPP
# define JOIN_HPP

# include "irc.hpp"
# include "ACommand.hpp"

class Client;
class Server;
class Join : public ACommand
{
	public:
		Join(Server &server, Client &user, std::string rawInput);
		Join(const Join &source);
		Join &operator=(const Join &rhs);
		~Join();

	protected:
	private:
		Join();
		void	execute();
		void	error(int) const;
		void	confirm() const;

};

#endif
