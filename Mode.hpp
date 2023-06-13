#ifndef MODE_HPP
# define MODE_HPP

# include "ACommand.hpp"
# include "irc.hpp"

# define SUCCESS 0
# define ERROR -1

class Client;
class Server;
class Mode : public ACommand
{
	public:
		Mode(Server &server, Client & author, std::string rawInput);
		Mode(const Mode &source);
		Mode &operator=(const Mode &rhs);
		~Mode();

	protected:
	private:
		Mode();
		void	execute();
		void	error(int) const;
		void	confirm() const;

		bool	isAPossibleChannelName(std::string name);
		int		checkValidCmd();

		int		invite();
		int		topic();
		int		channelKey();
		int		channelOp();
		int		channelLimit();

		Channel *_targetChannel;
};

#endif
