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
		bool operator<(const Channel &rhs) const;
		Channel(std::string name, Client& owner);

		void	updateChannelName(std::string);

		/* getters */
		std::string getName() const;

		/* users handler */
		int		isUserConnected(Client& user);
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
