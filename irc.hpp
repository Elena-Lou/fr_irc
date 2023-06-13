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
# include <iomanip>
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
# include <algorithm>
# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Kick.hpp"
# include "Join.hpp"
# include "User.hpp"
# include "Nick.hpp"
# include "Pass.hpp"
# include "Privmsg.hpp"
# include "Topic.hpp"
# include "Invite.hpp"
# include "Mode.hpp"
# include <ctime>

# define RPL_WELCOME 1
# define RPL_YOURHOST 2
# define RPL_CREATED 3
# define RPL_INFO 4
# define RPL_ISUPPORT 5
# define RPL_NOTOPIC 331
# define RPL_TOPIC 332
# define RPL_TOPICWHOTIME 333
# define RPL_NAMREPLY 353
# define RPL_ENDOFNAMES 366
# define RPL_YOUREOPER 381
# define ERR_NOSUCHNICK 401
# define ERR_NOSUCHCHANNEL 403
# define ERR_CANNOTSENDTOCHAN 404
# define ERR_TOOMANYCHANNELS 405
# define ERR_NORECIPIENT 411
# define ERR_NOTEXTTOSEND 412
# define ERR_NONICKNAMEGIVEN 431
# define ERR_ERRONEUSNICKNAME 432
# define ERR_NICKNAMEINUSE 433
# define ERR_NICKCOLLISION 436
# define ERR_USERNOTINCHANNEL 441
# define ERR_NOTONCHANNEL 442
# define ERR_USERONCHANNEL 443
# define ERR_NEEDMOREPARAMS 461
# define ERR_ALREADYREGISTERED 462
# define ERR_PASSWDMISMATCH 464
# define ERR_CHANNELISFULL 471
# define ERR_INVITEONLYCHAN 473
# define ERR_BANNEDFROMCHAN 474
# define ERR_BADCHANNELKEY 475
# define ERR_BADCHANMASK 476
# define ERR_CHANOPRIVSNEEDED 482
# define ERR_NOOPERHOST 491
# define ERR_UMODEUNKNOWNFLAG 501
# define ERR_USERSDONTMATCH 502
# define ERR_KEYSET 467
# define RPL_CHANNELMODEIS 324

# define MSG_NOTOPIC "No topic is set"
# define MSG_ENDOFNAMES "End of /NAMES list"
# define MSG_YOUREOPER "You are now an IRC operator"
# define MSG_NOSUCHNICK "No such nick/channel"
# define MSG_CANNOTSENDTOCHAN "Cannot send to channel"
# define MSG_NOSUCHCHANNEL "No such channel"
# define MSG_TOOMANYCHANNELS "You have joined too many channels"
# define MSG_NORECIPIENT "No recipient given"
# define MSG_NOTEXTTOSEND "No text to send"
# define MSG_NONICKNAMEGIVEN "No nickname given"
# define MSG_ERRONEUSNICKNAME "Erroneus nickname"
# define MSG_NICKNAMEINUSE "Nickname is already in use"
# define MSG_NICKCOLLISION "Nickname collision KILL from "
# define MSG_USERNOTINCHANNEL "They aren't on that channel"
# define MSG_NOTONCHANNEL "You're not on that channel"
# define MSG_USERONCHANNEL "is already on channel"
# define MSG_NEEDMOREPARAMS "Not enough parameters"
# define MSG_ALREADYREGISTERED "You may not reregister"
# define MSG_PASSWDMISMATCH "Password incorrect"
# define MSG_CHANNELISFULL "Cannot join channel (+l)"
# define MSG_INVITEONLYCHAN "Cannot join channel (+i)"
# define MSG_BANNEDFROMCHAN "Cannot join channel (+b)"
# define MSG_BADCHANNELKEY "Cannot join channel (+k)"
# define MSG_BADCHANMASK "Bad Channel Mask"
# define MSG_CHANOPRIVSNEEDED "You're not channel operator"
# define MSG_NOOPERHOST "No O-lines for your host"
# define MSG_UMODEUNKOWNFLAG "Unkown MODE flag"
# define MSG_USERSDONTMATCH "Cant change mode for other users"
# define MSG_KEYSET "Channel key already set"
# define CRLF "\r\n"



#endif
