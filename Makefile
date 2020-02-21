# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kdumarai <kdumarai@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/08/13 20:41:05 by kdumarai          #+#    #+#              #
#    Updated: 2020/02/21 08:22:38 by kdumarai         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_script

LIBFT = libft/libft.a
LIBFTDIR = $(dir $(LIBFT))

CC_FLAGS = -Wall -Werror -Wextra
CC_LIB = -Iincludes -I$(LIBFTDIR)/includes
LD_FLAGS = -L$(LIBFTDIR) -lft

INCDIR = includes
INCFILES = ft_script.h
INCLUDES = $(addprefix $(INCDIR)/, $(INCFILES))

SRCDIR = srcs
SRCFILES = ft_script.c \
			live.c \
			play.c \
			args.c \
			log.c \
			pty.c \
			term.c \
			cmd.c \
			timer.c \
			utils/utils.c \
			utils/play_utils.c \
			utils/stkhp_buff.c \
			utils/ft_getopt.c
SRCS = $(addprefix $(SRCDIR)/, $(SRCFILES))
NSRC = $(shell echo "$(SRCFILES)" | awk '{print NF}')
ifeq ($(shell [ ! -z "`which bc`" ] && [ ! -z "`which awk`" ] && echo true),true)
	CSRC = 1
else
	CSRC = 0
endif

OBJDIR = objs
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

PROJTEXT = \033[1;35m$(NAME): \033[0;39m

all: $(NAME)

$(LIBFTDIR):
	@ if [ -h "$(LIBFTDIR)" ]; then rm "$(LIBFTDIR)"; fi
	@ echo "$(PROJTEXT)Missing libft... Cloning..."
	@ git clone https://github.com/MLforAll/42Libft $(LIBFTDIR)

$(LIBFT): $(LIBFTDIR) force
	@ make -C $(LIBFTDIR)

$(NAME): $(LIBFT) $(LIBFTPRINTF) $(LIBMACHO) $(OBJS) $(INCLUDES)
	@ printf "\033[K$(PROJTEXT)Compiling\n"
	@ printf "$(PROJTEXT)Linking\n"
	@ gcc -o $(NAME) $(LD_FLAGS) $(OBJS)
	@ printf "$(PROJTEXT)\033[1;32mProgram built at $(NAME)\033[0;39m\n"

noflags:
	@ printf "\033[1;31m----------------------\n"
	@ printf "| CC_FLAGS DISABLED! |\n"
	@ printf "\r----------------------\033[0;39m\n"
	@ make all CC_FLAGS=""

fsanitize:
	@ printf "\033[1;31m------------------------------\n"
	@ printf "\r| ADDRESS SANITIZER ENABLED! |\n"
	@ printf "\r------------------------------\033[0;39m\n"
	@ make rep LD_FLAGS="$(LD_FLAGS) -fsanitize=address"

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCLUDES)
	@ if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	@ printf "\033[K$(PROJTEXT)Compiling \033[1;33m$<"
ifneq ($(CSRC),0)
	@ printf " %.0s" {1..$(shell expr 44 - $(shell printf "$<" | wc -m))}
	@ export LC_ALL=C; printf "\033[1;34m[%3.0f%%]" "$(shell bc <<< "scale=1; $(CSRC) / $(NSRC) * 100")"
	@ $(eval CSRC = $(shell expr $(CSRC) + 1))
endif
	@ printf "\033[0;39m\r"
	@ gcc $(CC_FLAGS) $(CC_LIB) -c $< -o $@

cleanp:
	@ rm -rf $(OBJDIR)
	@ printf "$(PROJTEXT)Removed objects\n"

fcleanp: cleanp
	@ rm -f $(NAME)
	@ printf "$(PROJTEXT)Removed $(NAME)\n"

rep: fcleanp all

clean: cleanp
	@ make clean -C $(LIBFTDIR)

fclean: fcleanp
	@ make fclean -C $(LIBFTDIR)

re: fclean all

force:
	@ true

.PHONY: all noflags fsanitize clean fclean re cleanp fcleanp rep
