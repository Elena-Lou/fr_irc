#ifndef JOIN_HPP
# define JOIN_HPP

# include <list>
# include <string>
# include "ACommand.hpp"

class Client;
class Join : public ACommand
{
	public:
		~Join();

	protected:
		Join(Client &user, std::string rawInput);
		Join(const Join &source);
		Join &operator=(const Join &rhs);

		virtual void	execute() const = 0;
		virtual void	error(std::string) const = 0;

	private:
		Join();
};

#endif
