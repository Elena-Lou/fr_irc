#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <set>

class Client;
class Channel
{
	public:
		Channel();
		~Channel();
		Channel(const Channel &source);
		Channel &operator=(const Channel &rhs);
		bool operator<(const Channel &rhs) const;
		Channel(std::string name);

		void	updateChannelName(std::string);

		/* getters */
		std::string getName() const;

		/* users handler */
		int		isUserConnected(Client& user);
		int		disconnectUser(Client& user);
		void	connectUser(Client&);

	private:
		int					_nbOfClients;
		std::string			_name;
		std::set<Client*>	_connectedClients;
	protected:
};

#endif
