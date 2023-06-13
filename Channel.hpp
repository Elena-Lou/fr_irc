#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <map>

# define INVITE_MODE 0b1
# define TOPIC_MODE 0b10
# define PASSWORD_MODE 0b100
# define CHANOP_MODE 0b1000
# define CHANLIMIT_MODE 0b10000

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
		void	changeTopicProtection(bool);

		/* users handler */
		bool	isUserConnected(Client& user);
		bool	isUserConnected(std::string userName);
		Client	*getUserIfConnected(std::string userName);
		int		removeUserFromChannel(Client& user);
		void	addUserToChannel(Client&);
		void	setMaxClients(int);

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
		void	sendTOPICWHOTIME(Server &server, Client &author);

		/* mods */
		void	changePassword(std::string password);
		bool	tryPassword(std::string password);
		void	setMode(char flag);
		void	unsetMode(char flag);
		bool	isMode(char flag);
	protected:
	private:
		Channel();
		int					_maxClients;
		int					_nbOfClients;
		std::string			_name;
		std::map<int, Client*>	_chanOps;
		std::map<int, Client*>	_connectedClients;
		bool		_protected;
		bool		_topicProtected;
		time_t _topicUpdateTimestamp;
		std::string	_topicUpdater;

		std::string _password;
		std::string _topic;
		char		_modeFlagsField;
};

#endif
