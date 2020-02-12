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
	uint32_t	size;
	uint32_t	bpad;
	uint32_t	timestamp;
	uint32_t	apad;
	uint32_t	utimestamp;
	uint32_t	direction;
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

#define HEX_CHARSET	"0123456789abcdef"

static void
print_recorded_string(struct rtsrecord *r, void *endptr)
{
	char	*s;
	uint8_t	d;
	uint8_t	shift;

	if (r->size == 0)
		return ;

	s = (char *)((uintptr_t)r + sizeof(struct rtsrecord));
	(void) write(1, s, r->size);
	(void) write(1, "\n", 1);

	shift = 0;
	for (uint32_t idx = 0; idx < r->size; idx++)
	{
		if (shift > 1)
		{
			(void) write(1, " ", 1);
			shift = 0;
		}

		d = s[idx] / 16;
		(void) write(1, &HEX_CHARSET[d], 1);
		d = s[idx] % 16;
		(void) write(1, &HEX_CHARSET[d], 1);

		shift++;
	}
}

inline static void
print_rtsrecord(struct rtsrecord *r, void *endptr, uint8_t debug)
{
	if (debug)
		(void) printf("----\ntimestamp: %u: %sutimestamp: %u\ndirection: %c\nsize: %u\n", \
		r->timestamp, ctime((time_t *)&r->timestamp), r->utimestamp, \
		(char)r->direction, r->size);
	print_recorded_string(r, endptr);
	if (debug && r->size)
		(void) write(1, "\n", 1);
}

inline static struct rtsrecord *
next_rtsrecord(struct rtsrecord *r, void *endptr)
{
	struct rtsrecord	*ret;

	ret = (struct rtsrecord *)((uintptr_t)r + sizeof(struct rtsrecord) + r->size);
	if ((void *)ret > endptr)
		return NULL;
	return ret;
}

#define PRINT_DEBUG	1

int
main(int ac, const char **av)
{
	char					*content;
	size_t					sz;
	void					*endptr;
	struct rtsrecord		*r, *nr;

	if (ac < 2)
		errx(1, "usage: %s /path/to/file", *av);

	sz = read_file(av[1], &content);
	endptr = (void *)(content + sz);

	(void) printf("file starts at %p for %zu\n", content, sz);
	r = (struct rtsrecord *)content;
	while ((nr = next_rtsrecord(r, endptr)))
	{
		print_rtsrecord(r, endptr, PRINT_DEBUG);
		r = nr;
	}

	return 0;
}
