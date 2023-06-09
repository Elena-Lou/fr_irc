#ifndef PASS_HPP
# define PASS_HPP

# include <list>
# include <string>

class Client;
class Server;
class Pass : public ACommand
{
	public:
		~Pass();
		Pass(Server &server, Client &author, std::string rawInput);
		Pass(const Pass &source);
		Pass &operator=(const Pass &rhs);

	protected:

	private:
		Pass();
		void	execute();
		void	confirm() const;
		void	error(int errorCode) const;
};

#endif
