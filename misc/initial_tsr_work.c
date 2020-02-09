#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <time.h>

struct rtsrecord
{
	uint32_t	pad;
	uint32_t	timestamp;
	uint32_t	tspad;
	uint32_t	ntimestamp;
	uint32_t	direction; // 1 byte used
	char		start; // end of string = ascii 0x1 (soh - start of heading)
};

struct rtshdr
{
	uint32_t		pad;
	struct rtsrecord	hdr;
	// series of struct rtsrecord
};

static size_t
read_file(const char *path, char **out)
{
	int			fd;
	struct stat st;
	void		*map;
	size_t		sz;

	if ((fd = open(path, O_RDONLY)) == -1)
		err(1, "open()");
	if (fstat(fd, &st) != 0)
		err(1, "fstat()");
	sz = st.st_size;
	map = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_FILE | MAP_PRIVATE, fd, 0);
	if (out)
		*out = (char *)map;
	return sz;
}

static void
print_recorded_string(struct rtsrecord *r, void *endptr)
{
	char	*s;
	size_t	len;

	s = &r->start;
	len = 0;
	while (s[len] != 0x1 && s[len] != 0x2)
	{
		if ((void *)(s + len) > endptr)
			return ;
		len++;
	}
	(void) write(1, s, len);
}

inline static void
print_rtsrecord(struct rtsrecord *r, void *endptr, uint8_t debug)
{
	if (debug)
		(void) printf("----\ntimestamp: %u: %sntimestamp: %u\ndirection: %u (%c)\n", \
		r->timestamp, ctime((time_t *)&r->timestamp), r->ntimestamp, r->direction, (char)r->direction);
	print_recorded_string(r, endptr);
	if (debug)
		(void) write(1, "\n", 1);
}

static struct rtsrecord *
next_rtsrecord(struct rtsrecord *r, void *endptr)
{
	char	*s;
	struct rtsrecord *ret;

	s = &r->start;
	while (*s != 0x1 && *s != 0x2)
	{
		if ((void *)s > endptr)
			return NULL;
		s++;
	}
	ret = (struct rtsrecord *)(s + 4);
	return (void *)ret > endptr ? NULL : ret;
}

#define PRINT_DEBUG	0

int
main(int ac, const char **av)
{
	char					*content;
	size_t					sz;
	char					*endptr;
	struct rtshdr			*hdr;
	struct rtsrecord		*r, *nr;
	struct timespec			rq;

	if (ac < 2)
		errx(1, "usage: %s /path/to/file", *av);
	sz = read_file(av[1], &content);
	endptr = content + sz;
	(void) printf("file starts at %p for %zu\n\n", content, sz);

	hdr = (struct rtshdr *)content;

	r = &hdr->hdr;
	nr = (struct rtsrecord *)(content + sizeof(struct rtshdr));
	while (r && nr)
	{
		if (r->direction == 'o')
			print_rtsrecord(r, endptr, PRINT_DEBUG);
		rq.tv_sec = nr->timestamp - r->timestamp;
		if (r->ntimestamp < nr->ntimestamp)
			rq.tv_nsec = (nr->ntimestamp - r->ntimestamp) * 1000;
		else
			rq.tv_nsec = (r->ntimestamp - nr->ntimestamp) * 1000;
		(void) nanosleep(&rq, NULL);
		r = nr;
		nr = next_rtsrecord(r, endptr);
	}
	return 0;
}
