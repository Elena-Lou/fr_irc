#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <map>

class Client;
class Channel
{
	public:
		~Channel();
		Channel(const Channel &source);
		Channel &operator=(const Channel &rhs);
		Channel(std::string name, Client& owner);

		void	updateChannelName(std::string);

		/* comparison operators */
		bool operator<(const Channel &rhs) const;
		bool operator==(const Channel &rhs) const;

		/* getters */
		std::string getName() const;

		/* users handler */
		bool	isUserConnected(Client& user);
		bool	isUserConnected(std::string userName);
		Client	*getUserIfConnected(std::string userName);
		int		removeUserFromChannel(Client& user);
		void	addUserToChannel(Client&);

		/* Operator handler */
		bool	isChannelOperator(Client & user);
		void	setOperator(Client &chanOp);
		void	removeOperator(Client &chanOp);

		/* message */
		void	broadcastAllClients(std::string &msg);

	private:
		Channel();
		int					_nbOfClients;
		std::string			_name;
		std::map<int, Client*>	_chanOps;
		std::map<int, Client*>	_connectedClients;
	protected:
};

#endif
