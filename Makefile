SRC		=	main.cpp \
			IoObject.cpp \
			Socket.cpp \
			Client.cpp \
			ServerConfig.cpp \
			Server.cpp \
			Location.cpp \
			Exception.cpp \
			Response.cpp \
			Request.cpp \
			Resource.cpp \
			ResponseMaker.cpp \
			ResourceHandler.cpp \
			ResponseHandler.cpp \
			CgiResponse.cpp \
			CgiWriter.cpp \
			CgiResource.cpp

SRCDIR	=	./srcs/
SRCS	=	$(addprefix $(SRCDIR), $(SRC))
OBJS	=	$(SRCS:.cpp=.o)
NAME	=	webserv

LIBNAME	=	libft.a
LIBDIR	=	libft/
LIBH	=	libft.h

INCDIR	=	./includes/

GPP		=	clang++
GPPFLAG	=	-g -Wall -Wextra -Werror
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
			$(RM) $(OBJS) $(LIBNAME) $(addprefix $(INCDIR), $(LIBH))
			$(MAKE) -C $(LIBDIR) fclean

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
