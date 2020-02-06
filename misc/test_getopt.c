#include <stdio.h>
#include <unistd.h>

int
main(int ac, char **av)
{
	int	rc;

	(void) printf("opterr: %i; optreset: %i\n", opterr, optreset);
	while ((rc = getopt(ac, av, "adFkpqrt:")) != -1)
		(void) printf("rc: %c (%i); optarg: %s; optind: %i; optopt: %c (%i)\n", \
			rc, rc, optarg, optind, optopt, optopt);
	return 0;
}
