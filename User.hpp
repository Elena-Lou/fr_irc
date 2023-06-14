
#ifndef USER_HPP
# define USER_HPP

# include <list>
# include <string>

class Client;
class Server;
class User : public ACommand
{
	public:
		~User();
		User(Server &server, Client &author, std::string rawInput);
		User(const User &source);
		User &operator=(const User &rhs);

	protected:

	private:
		User();
		void	execute();
		void	confirm() const;
		void	error(int errorCode) const;


		void	sendRPLWELCOME() const;
		void	sendRPLYOURHOST() const;
		void	sendRPLCREATED() const;
		void	sendRPLISUPPORT() const;
		void	sendRPLUSERS() const;
};

#endif
