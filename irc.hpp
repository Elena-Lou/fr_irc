#ifndef IRC_HPP
# define IRC_HPP

# define IRC_BUFFER_SIZE 80
# define MYIRC_PORT "3490"
# define MYIRC_ALLOWED_PENDING_CONNECTIONS 5
# define MYIRC_TIMEOUT 30

# define CONNECTED 1
# define NOT_CONNECTED 0

# define SHOW_CONSTRUCTOR 1

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
# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"


#endif
