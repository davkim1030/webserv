# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hyukim <hyukim@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/02/25 11:37:14 by sunpark           #+#    #+#              #
#    Updated: 2021/06/25 03:53:04 by hyukim           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS	= ft_memset.cpp ft_bzero.cpp ft_memcpy.cpp ft_memccpy.cpp ft_memmove.cpp \
		  ft_memchr.cpp ft_memcmp.cpp ft_strlen.cpp ft_isalpha.cpp ft_isdigit.cpp \
		  ft_isalnum.cpp ft_isascii.cpp ft_isprint.cpp ft_toupper.cpp ft_tolower.cpp \
		  ft_strchr.cpp ft_strrchr.cpp ft_strncmp.cpp ft_strlcpy.cpp ft_strlcat.cpp \
		  ft_strnstr.cpp ft_atoi.cpp ft_calloc.cpp ft_strdup.cpp ft_substr.cpp \
		  ft_strjoin.cpp ft_strtrim.cpp ft_split.cpp ft_itoa.cpp ft_strmapi.cpp \
		  ft_putchar_fd.cpp ft_putstr_fd.cpp ft_putendl_fd.cpp ft_putnbr_fd.cpp \
		  ft_get_time.cpp ft_split_space.cpp ftLog.cpp ftToupperStr.cpp
OBJS	= $(SRCS:.cpp=.o)

BSRCS	= ft_lstnew.cpp ft_lstadd_front.cpp ft_lstsize.cpp ft_lstlast.cpp \
		  ft_lstadd_back.cpp ft_lstdelone.cpp ft_lstclear.cpp ft_lstiter.cpp \
		  ft_lstmap.cpp \
		  ft_isupper_bonus.cpp ft_islower_bonus.cpp ft_isnumber_bonus.cpp \
		  ft_isblank_bonus.cpp ft_strndup_bonus.cpp ft_strnul_bonus.cpp \
		  ft_strpush_bonus.cpp get_next_line_bonus.cpp
BOBJS	= $(BSRCS:.cpp=.o)

NAME	= libft.a

GCC		= clang++
GCCFLAG	= -g -Wall -Wextra -Werror
RM		= rm -f

.cpp.o:
			@$(GCC) $(GCCFLAG) -c $< -o $(<:.cpp=.o)

$(NAME):	$(OBJS)
			ar rc $(NAME) $(OBJS)

all:		$(NAME)

bonus:		$(OBJS) $(BOBJS)
			@ar rc $(NAME) $(BOBJS) $(OBJS)

clean:
			@$(RM) $(OBJS) $(BOBJS) $(POBJS)

fclean:		clean
			@$(RM) $(NAME) $(TSTNAME)

re:			fclean all

reclean:	fclean all clean

/PHONY:		all clean fclean re reclean .cpp.o
