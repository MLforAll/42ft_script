#include <stdio.h>
#include <unistd.h>

int
main(int ac, char **av)
{
	int	rc;

	while ((rc = getopt(ac, av, "adFkpqrt:")) != -1)
		(void) printf("rc: %c (%i); optarg: %s; optind: %i; optopt: %c (%i); opterr: %i; optreset: %i\n", \
			rc, rc, optarg, optind, optopt, optopt, opterr, optreset);
	return 0;
}
