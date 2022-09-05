/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <sys/ioctl.h>
#include <net/if_media.h>
#include "common.h"
#include "plugins.h"

int if_err_(int argc, char **argv)
{
	char *interface = basename(argv[0]);
	if (strncmp(interface, "if_err_", 7) != 0)
		return fail("if_err_ invoked with invalid basename");
	interface += 7;

	int mib[] = { CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0 };
	size_t bufferSize;
	if (sysctl(mib, 6, NULL, &bufferSize, NULL, 0) != 0)
		return 1;

	char *interfaces = malloc(bufferSize);
	if (sysctl(mib, 6, interfaces, &bufferSize, NULL, 0) != 0)
		return 1;

	char *interfacesStart = interfaces;

	if (argc > 1) {
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
		if (!strcmp(argv[1], "suggest")) {
			while (interfaces < interfacesStart + bufferSize) {
				struct if_msghdr *header = (struct if_msghdr *)interfaces;
				if (header->ifm_type != RTM_IFINFO
					|| !(header->ifm_flags & IFF_UP)
					|| !(header->ifm_flags & IFF_RUNNING)
					|| header->ifm_flags & IFF_LOOPBACK
					|| header->ifm_flags & IFF_POINTOPOINT)
					goto next;

				struct if_data *ifdata = &header->ifm_data;
				if (ifdata->ifi_type != IFT_ETHER)
					goto next;

				struct sockaddr_dl *sdl = (struct sockaddr_dl *)(header + 1);
				char *name = strndup(sdl->sdl_data, sdl->sdl_nlen);

				int sockfd;
				if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
					return 1;

				struct ifmediareq ifmr;
				bzero(&ifmr, sizeof(ifmr));
				strcpy(ifmr.ifm_name, name);
				if (ioctl(sockfd, SIOCGIFMEDIA, (caddr_t)&ifmr) < 0)
					return 1;

				if ((ifmr.ifm_status & IFM_AVALID) && !(ifmr.ifm_status & IFM_ACTIVE))
					goto next;

				puts(name);

next:
				interfaces += header->ifm_msglen;
			}
			return 0;
		}
		if (!strcmp(argv[1], "config")) {
			puts("graph_order rcvd trans");
			printf("graph_title %s errors\n", interface);
			puts("graph_args --base 1000\n"
			     "graph_vlabel packets in (-) / out (+) per "
			     "${graph_period}\n" "graph_category network");
			printf("graph_info This graph shows the amount of "
			       "errors on the %s network interface.\n",
			       interface);
			puts("rcvd.label packets\n"
			     "rcvd.type COUNTER\n"
			     "rcvd.graph no\n"
			     "rcvd.warning 1\n"
			     "trans.label packets\n"
			     "trans.type COUNTER\n"
			     "trans.negative rcvd\n" "trans.warning 1");
			print_warncrit("rcvd");
			print_warncrit("trans");
			return 0;
		}
	}

	while (interfaces < interfacesStart + bufferSize) {
		struct if_msghdr *header = (struct if_msghdr *)interfaces;
		struct if_data *ifdata = &header->ifm_data;
		struct sockaddr_dl *sdl = (struct sockaddr_dl *)(header + 1);
		char *name = strndup(sdl->sdl_data, sdl->sdl_nlen);
		if (strcmp(name, interface) != 0)
			goto next2;

		printf("rcvd.value %lu\n",  ifdata->ifi_ierrors);
		printf("trans.value %lu\n", ifdata->ifi_oerrors);

next2:
		interfaces += header->ifm_msglen;
	}


	return 0;
}
