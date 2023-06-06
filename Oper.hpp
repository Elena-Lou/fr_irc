#ifndef OPER_HPP
# define OPER_HPP

# include "ACommand.hpp"
# include "irc.hpp"

class Client;
class Server;
class Oper : public ACommand
{
	public:
		~Oper();

	protected:
		Oper(Server & server, Client & user, std::string rawInput);
		Oper(Oper const & src);
		Oper &operator=(Oper const & rhs);

		void	execute() const;
		void	error(int) const;

	private:
		Oper();
};

#endif