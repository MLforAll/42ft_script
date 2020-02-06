#include <stdio.h>
#include <unistd.h>

#include "ft_getopt.h"

#define OPTSTRING	"adFkpqrt:"

int
main(int ac, char **av)
{
	int	rc;

	(void) puts("=> getopt()");
	(void) printf("opterr: %i; optreset: %i\n", opterr, optreset);
	while ((rc = getopt(ac, av, OPTSTRING)) != -1)
		(void) printf("  -> rc: %c (%i); optarg: %s; optind: %i; optopt: %c (%i)\n", \
			rc, rc, optarg, optind, optopt, optopt);
	(void) printf("optarg: %s; optind: %i; optopt: %i\n", optarg, optind, optopt);

	(void) puts("\n=> ft_getopt()");
	(void) printf("g_opterr: %i; g_optreset: %i\n", g_opterr, g_optreset);
	while ((rc = ft_getopt(ac, (const char **)av, OPTSTRING)) != -1)
		(void) printf("  -> rc: %c (%i); g_optarg: %s; g_optind: %i; g_optopt: %c (%i)\n", \
			rc, rc, g_optarg, g_optind, g_optopt, g_optopt);
	(void) printf("g_optarg: %s; g_optind: %i; g_optopt: %i\n", g_optarg, g_optind, g_optopt);

	return 0;
}
