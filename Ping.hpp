#ifndef PING_HPP
# define PING_HPP

# include <list>
# include <string>

class Client;
class Server;
class Ping : public ACommand
{
	public:
		~Ping();
		Ping(Server &server, Client &author, std::string rawInput);
		Ping(const Ping &source);
		Ping &operator=(const Ping &rhs);

	protected:

	private:
		Ping();
		void	execute();
		void	confirm() const;
		void	error(int errorCode) const;

		std::string _message;
};

#endif
