#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net80211/ieee80211.h>
#include <net80211/ieee80211_ioctl.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "config.h"

int sock;

void piface(char *, char *, char *);
void getsock();

void
iface_run(char **out)
{
	char buf[MAXLEN];
	char all[MAXLEN];
	char addrnm[MAXLEN];
	int len = sizeof(struct sockaddr_in), n = 0;
	struct ifaddrs *ifap, *next;
	all[0] = '\0';
	if (getifaddrs(&ifap) == 0) {
		for (next = ifap; next; next = next->ifa_next) {
			if (next->ifa_addr != NULL && next->ifa_addr->sa_family == AF_INET) {
				memset(addrnm, 0, sizeof(addrnm));
				if (strncmp(next->ifa_name, "lo", 2) == 0) {
					continue;
				} else {
					getnameinfo(next->ifa_addr, len, addrnm, sizeof(addrnm), NULL, 0, NI_NUMERICHOST);
					piface(buf, next->ifa_name, addrnm);
					if (n++) {
						strlcat(all, SEP, MAXLEN);
						strlcat(all, buf, MAXLEN);
					} else {
						strlcpy(all, buf, MAXLEN);
					}
				}
			}
		}
		freeifaddrs(ifap);
	}
	*out = strndup(all, MAXLEN);
}

void
getsock()
{
	int naf = AF_INET;
	static int oaf = -1;

	if (oaf == naf)
		return;
	if (oaf != -1)
		close(sock);
	sock = socket(naf, SOCK_DGRAM, 0);
	if (sock == -1)
		oaf = -1;
	else
		oaf = naf;
}

void
piface(char *out, char *name, char *ip)
{
	struct ifreq ifr;
	struct sockaddr_in *sin, sin2;
	struct ieee80211_nwid nwid;

	getsock();
	
	memcpy(&sin2, &ifr.ifr_addr, sizeof(sin2));

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_data = (caddr_t)&nwid;
	strlcpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	if (ioctl(sock, SIOCG80211NWID, (caddr_t)&ifr) == 0) {
		snprintf(out, MAXLEN, "%s: %s (%s)", name, ip, nwid.i_nwid);
	} else {
		snprintf(out, MAXLEN, "%s: %s", name, ip);
	}
}
