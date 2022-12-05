# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vismaily <nenie_iri@mail.ru>               +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/28 12:59:24 by vismaily          #+#    #+#              #
#    Updated: 2022/12/03 14:57:43 by vismaily         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv

TMP			= Tmp

SRCS_COMM	= $(wildcard ./Srcs/*.cpp)
SRCS_PARS	= $(wildcard ./Srcs/Parsing/*.cpp)
SRCS_CLAS	= $(wildcard ./Includes/*.cpp)
SRCS		= $(SRCS_COMM) $(SRCS_PARS) $(SRCS_CLAS)

OBJS		= $(patsubst ./Srcs/%.cpp, ./$(TMP)/%.o, $(SRCS_COMM)) \
			  $(patsubst ./Srcs/Parsing/%.cpp, ./$(TMP)/%.o, $(SRCS_PARS)) \
			  $(patsubst ./Includes/%.cpp, ./$(TMP)/%.o, $(SRCS_CLAS)) \

CPP			= c++

CPP_FLAGS	= -Wall -Wextra -Werror

BONUS_PART	= 0

DEFINES		= -D BONUS=$(BONUS_PART)

INCLUDES	= -I ./Includes

RM			= rm -rf

all:		bonus

$(NAME):	$(TMP) $(OBJS)
			@$(CPP) -o $(NAME) $(OBJS)

bonus:		bonus_on $(NAME)

bonus_on:
			$(eval BONUS_PART = 1)

$(TMP):
			@$(shell mkdir $(TMP))

$(TMP)/%.o:	Srcs/%.cpp
			@$(CPP) $(CPP_CFLAGS) $(DEFINES) $(INCLUDES) -o $@ -c $<

$(TMP)/%.o:	Srcs/Parsing/%.cpp
			@$(CPP) $(CPP_CFLAGS) $(DEFINES) $(INCLUDES) -o $@ -c $<

$(TMP)/%.o:	Includes/%.cpp
			@$(CPP) $(CPP_CFLAGS) $(DEFINES) $(INCLUDES) -o $@ -c $<

clean:
			@$(RM) $(TMP)

fclean:		clean
			@$(RM) $(NAME) temp

re:			fclean all

.PHONY:		all clean fclean re bonus
