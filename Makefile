# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hyukim <hyukim@student.42seoul.kr>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/11/22 22:02:20 by hyukim            #+#    #+#              #
#    Updated: 2021/05/25 15:12:54 by hyukim           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC		=	main.cpp

SRCDIR	=	./srcs/
SRCS	=	$(addprefix $(SRCDIR), $(SRC))
OBJS	=	$(SRCS:.cpp=.o)
NAME	=	webserv

LIBNAME	=	libft.a
LIBDIR	=	libft/
LIBH	=	libft.h

INCDIR	=	./includes/

GPP		=	clang++
GPPFLAG	=	-g #-Wall -Wextra -Werror
RM		=	rm -f

%.o:		%.cpp
			$(GPP) $(GPPFLAG) -I$(INCDIR) -c $< -o $@

$(NAME):	$(LIBNAME) $(OBJS)
			$(GPP) $(GPPFLAG) -I$(INCDIR) -o $(NAME) $(OBJS) $(LIBNAME)

$(LIBNAME):
			$(MAKE) -C $(LIBDIR) bonus
			@cp $(addprefix $(LIBDIR), $(LIBNAME)) $(LIBNAME)
			@cp $(addprefix $(LIBDIR), $(LIBH)) $(INCDIR)

all:		$(NAME)

clean:
			$(RM) $(OBJS)

fclean:		clean
			$(RM) $(NAME) $(LIBNAME)
			$(MAKE) -C $(LIBDIR) fclean

re:			fclean all

.PHONY:		all clean fclean re
