#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

static char *
ft_ptsname(int fd)
{
	static char buff[512];

	if (ioctl(fd, TIOCPTYGNAME, buff) != 0)
		return NULL;
	return buff;
}

static int
ft_tcgetattr(int fd, struct termios *t)
{
	return ioctl(fd, TIOCGETA, t);
}

static void
ft_pause(void)
{
	char	b;

	(void)write(1, "paused...\n", 10 * sizeof(char));
	(void)read(0, &b, sizeof(char));
}

int
main(void)
{
	setbuf(stdout, NULL);

	int fd = open("/dev/ptmx", O_RDWR);
	const char *ptsn = ft_ptsname(fd);

	(void)printf("fd: %i; errno: %i; %s\n", fd, errno, ptsn);

	(void)ioctl(fd, TIOCPTYGRANT);
	(void)ioctl(fd, TIOCPTYUNLK);
	int pts = open(ptsn, O_RDWR);
	(void)printf("pts: %i\n", pts);

	struct termios saved;
	struct termios t;
	(void)ft_tcgetattr(0, &saved);
	t = saved;
	t.c_lflag &= (unsigned long)~(ICANON | ECHO | ISIG);
	(void)tcsetattr(0, TCSANOW, &t);

	int pid = fork();
	if (pid == 0)
	{
		(void)close(fd);
	
		(void)dup2(pts, 0);
		(void)dup2(pts, 1);
		(void)dup2(pts, 2);
	
		(void)setsid();
		(void)ioctl(0, TIOCSCTTY, 1);
		(void)close(pts);

		(void)execl("/bin/bash", "bash", "-l", NULL);
		_exit(127);
	}
	(void)close(pts);

	int tsfd = open("typescript", O_WRONLY | O_CREAT, 0644);
	char tsb[512];
	ssize_t rb;
	fd_set ss;
	while (42)
	{
		FD_ZERO(&ss);
		FD_SET(fd, &ss);
		FD_SET(0, &ss);
		if (select(fd + 1, &ss, NULL, NULL, NULL) < 1)
			break ;
		if (FD_ISSET(0, &ss))
		{
			rb = read(0, tsb, sizeof(tsb));
			(void)write(fd, tsb, rb);
		}
		else if (FD_ISSET(fd, &ss))
		{
			if ((rb = read(fd, tsb, sizeof(tsb))) < 1)
				break ;
			(void)write(1, tsb, rb);
			(void)write(tsfd, tsb, rb);
		}
	}
	(void)close(tsfd);

	(void)waitpid(pid, NULL, 0);
	(void)close(fd);

	ft_pause();
	(void)tcsetattr(0, TCSANOW, &saved);

	return 0;
}
