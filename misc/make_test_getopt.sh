#!/bin/sh

if [ "$1" = clean -o "$1" = fclean ]
then
	rm -f test_getopt
	exit
fi

make -C .. && gcc test_getopt.c -o test_getopt ../objs/ft_getopt.o -I../includes -L../libft -lft
