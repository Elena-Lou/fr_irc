NAME = ircserv

SRCS =	main.cpp\
		Server.cpp\
		Client.cpp\
		Channel.cpp

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

CPPFLAGS = -MMD -I ./include/

OBJS_DIR = ./objs/

OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.cpp=.o))

DEPS = $(addprefix $(OBJS_DIR), $(SRCS:.cpp=.d))

all: $(NAME)
	@make $(NAME) -q && echo "All good here !"

$(OBJS_DIR)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(NAME)

re: clean
	@make all

test: $(NAME)
		./$(NAME)

vtest:	$(NAME)
		valgrind --leak-check=full --track-fds=yes ./$(NAME)

-include $(DEPS)

.PHONY: all clean flean re test vtest