#ifndef JOIN_HPP
# define JOIN_HPP

# include <list>
# include <string>
# include "ACommand.hpp"

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
