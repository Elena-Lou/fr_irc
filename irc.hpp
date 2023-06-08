#ifndef IRC_HPP
# define IRC_HPP

# define IRC_BUFFER_SIZE 80
# define MYIRC_PORT "3490"
# define MYIRC_ALLOWED_PENDING_CONNECTIONS 5
# define MYIRC_TIMEOUT 30

# define CONNECTED 1
# define NOT_CONNECTED 0

# define SHOW_CONSTRUCTOR 1

# include <string>
# include <iostream>
# include <netinet/in.h>
# include <cstdlib>
# include <cstring>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <cerrno>
# include <unistd.h>
# include <fcntl.h>
# include <sys/time.h>
# include <sys/select.h>
# include <set>
# include <deque>
# include <map>
# include <exception>
# include <csignal>
#include <algorithm>
# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Kick.hpp"
# include "Join.hpp"
# include "User.hpp"
# include "Nick.hpp"
# include <ctime>

# define RPL_WELCOME "001"
# define RPL_YOURHOST "002"
# define RPL_CREATED "003"
# define RPL_INFO "004"
# define RPL_ISUPPORT "005"
# define RPL_TOPIC 332
# define ERR_NOSUCHCHANNEL 403
# define ERR_TOOMANYCHANNELS 405
# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKNAMEINUSE 433
# define ERR_NICKCOLLISION 436
# define ERR_USERNOTINCHANNEL 441
# define ERR_NOTONCHANNEL 442
# define ERR_NEEDMOREPARAMS 461
# define ERR_ALREADYREGISTERED 462
# define ERR_CHANNELISFULL 471
# define ERR_INVITEONLYCHAN 473
# define ERR_BANNEDFROMCHAN 474
# define ERR_BADCHANNELKEY 475
# define ERR_BADCHANMASK 476
# define ERR_CHANOPRIVSNEEDED 482

# define ERR_NOSUCHCHANNEL_S "403"
# define ERR_TOOMANYCHANNELS_S "405"
# define ERR_NONICKNAMEGIVEN_S "431"
# define ERR_ERRONEUSNICKNAME_S "432"
# define ERR_NICKNAMEINUSE_S "433"
# define ERR_NICKCOLLISION_S "436"
# define ERR_USERNOTINCHANNEL_S "441"
# define ERR_NOTONCHANNEL_S "442"
# define ERR_NEEDMOREPARAMS_S "461"
# define ERR_ALREADYREGISTERED_S "462"
# define ERR_CHANNELISFULL_S "471"
# define ERR_INVITEONLYCHAN_S "473"
# define ERR_BANNEDFROMCHAN_S "474"
# define ERR_BADCHANNELKEY_S "475"
# define ERR_BADCHANMASK_S "476"
# define ERR_CHANOPRIVSNEEDED_S "482"


# define ERR_NOSUCHCHANNEL_MSG "No such channel"
# define ERR_TOOMANYCHANNELS_MSG "You have joined too many channels"
# define ERR_NONICKNAMEGIVEN_MSG "No nickname given"
# define ERR_ERRONEUSNICKNAME_MSG "Erroneus nickname"
# define ERR_NICKNAMEINUSE_MSG "Nickname is already in use"
# define ERR_NICKCOLLISION_MSG "Nickname collision KILL from "
# define ERR_USERNOTINCHANNEL_MSG "They aren't on that channel"
# define ERR_NOTONCHANNEL_MSG "You're not on that channel"
# define ERR_NEEDMOREPARAMS_MSG "Not enough parameters"
# define ERR_ALREADYREGISTERED_MSG "You may not reregister"
# define ERR_CHANNELISFULL_MSG "Cannot join channel (+l)"
# define ERR_INVITEONLYCHAN_MSG "Cannot join channel (+i)"
# define ERR_BANNEDFROMCHAN_MSG "Cannot join channel (+b)"
# define ERR_BADCHANNELKEY_MSG "Cannot join channel (+k)"
# define ERR_BADCHANMASK_MSG "Bad Channel Mask"
# define ERR_CHANOPRIVSNEEDED_MSG "You're not channel operator"

# define CRLF "\r\n"



#endif
