#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <machine/apmvar.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include "config.h"

#define _PATH_APM_SOCKET	"/var/run/apmdev"
#define CLEN 20

enum apm_action {
	NONE,
	SUSPEND,
	STANDBY,
	HIBERNATE,
	GETSTATUS,
	SETPERF_LOW,
	SETPERF_HIGH,
	SETPERF_AUTO,
};

struct apm_command {
	int vno;
	enum apm_action action;
};

struct apm_reply {
	int vno;
	int newstate;
	int perfmode;
	int cpuspeed;
	struct apm_power_info batterystate;
	int error;
};

static int open_socket();
void batcolor(char *, int, int, int);

static int
open_socket()
{
	int fd;
	const char *sockname = _PATH_APM_SOCKET;
	struct sockaddr_un s_un;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd != -1) {
		s_un.sun_family = AF_UNIX;
		strlcpy(s_un.sun_path, sockname, sizeof(s_un.sun_path));
		if (connect(fd, (struct sockaddr *)&s_un, sizeof(s_un)) == -1) {
			close(fd);
			fd = -1;
		}
	}
	return fd;
}

void
apm_run(char **batst, char **cpust)
{
	char buf[MAXLEN];
	char out[MAXLEN];
	char batc[CLEN];
	int len, fd = open_socket();
	struct apm_command command;
	struct apm_reply reply;

	bzero(&reply, sizeof reply);
	reply.batterystate.battery_state = APM_BATT_UNKNOWN;
	reply.batterystate.ac_state = APM_AC_UNKNOWN;
	reply.perfmode = 0;
	reply.cpuspeed = 0;

	command.action = GETSTATUS;
	command.vno = APM_BATTERY_ABSENT;

	if (send(fd, &command, sizeof(command), 0) == sizeof(command) && recv(fd, &reply, sizeof(reply), 0) == sizeof(reply)) {
		batcolor(batc, reply.batterystate.battery_state, reply.batterystate.ac_state, reply.batterystate.battery_life);
		snprintf(out, MAXLEN, " %sBAT: %d%%%%", batc, reply.batterystate.battery_life);

		if ((int)reply.batterystate.minutes_left > 0) {
			snprintf(buf, MAXLEN, ", %d min", reply.batterystate.minutes_left);
			strlcat(out, buf, MAXLEN);
		}
		strlcat(out, "%{F-}", MAXLEN);
		*batst = strndup(out, MAXLEN);

		asprintf(cpust, "CPU: %d MHz", reply.cpuspeed);
	}
	close(fd);
}

void
batcolor(char *color, int bst, int ast, int est)
{
	int c1r, c1g, c1b, c2r, c2g, c2b;
	if (bst == APM_BATT_HIGH && ast == APM_AC_ON) {
		strlcpy(color, "%{F-}", CLEN);
		return;
	} else {
		switch (bst) {
		case APM_BATT_HIGH:
			if (est > 0) {
				c1r = 0x99;
				c1g = 0xFF;
				c1b = 0x99;
				c2r = 0xFF;
				c2g = 0xFF;
				c2b = 0x99;
				break;
			}
		case APM_BATT_LOW:
			if (est > 0) {
				c1r = 0xFF;
				c1g = 0xFF;
				c1b = 0x99;
				c2r = 0xFF;
				c2g = 0x99;
				c2b = 0x99;
				break;
			}
		case APM_BATT_CHARGING:
			if (est > 0) {
				c1r = 0x0;
				c1g = 0x0;
				c1b = 0xFF;
				c2r = 0x90;
				c2g = 0x90;
				c2b = 0xFF;
				break;
			}
		default:
			strlcpy(color, "%{F#FF9090}", CLEN);
			return;
		}
	}
	if (c1r != c2r)
		c1r -= (c1r - c2r) * est / 100;
	if (c1g != c2g)
		c1g -= (c1g - c2g) * est / 100;
	if (c1b != c2b)
		c1b -= (c1b - c2b) * est / 100;
	snprintf(color, CLEN, "%%{F#%2X%2X%2X}", c1r, c1g, c1b);
}
