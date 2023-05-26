#ifndef IRC_HPP
# define IRC_HPP
# define IRC_BUFFER_SIZE 80
# define CONNECTED 1
# define NOT_CONNECTED 0

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
# include "Server.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include <set>
# include <exception>


#endif
