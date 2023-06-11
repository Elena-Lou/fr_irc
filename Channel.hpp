#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <map>

class Server;
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
		std::string getTopic() const;
		bool	isProtected() const;
		bool	isTopicProtected();

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
		void	broadcastToChannel(std::string message);
		void	writePrivmsg(std::string source, std::string dest, std::string msg);
		void	sendAllNamesToUser(Server &server, Client &user);
		void	sendTopicToUser(Server &server, Client &user);
		void	updateTopic(Client &author, std::string message);

		/* mods */
		void	changePassword(std::string password);
		bool	tryPassword(std::string password);
	protected:
	private:
		Channel();
		int					_nbOfClients;
		std::string			_name;
		std::map<int, Client*>	_chanOps;
		std::map<int, Client*>	_connectedClients;
		bool		_protected;
		bool		_topicProtected;
		std::string _password;
		std::string _topic;
};

#endif
