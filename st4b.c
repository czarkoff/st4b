#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include "config.h"

void apm_run(char **, char **);
void time_run(char **);
void iface_run(char **);

int
main(int argc, char **argv) {
	char *cpus, *bats, *times, *ifaces;

	while (1) {
		apm_run(&bats, &cpus);
		time_run(&times);
		iface_run(&ifaces);
		fprintf(stdout, FMTOUT, SEP, cpus, bats, times, ifaces);
		free(bats);
		free(cpus);
		free(times);
		fflush(stdout);
		sleep(1);
	}

	return 0;
}
