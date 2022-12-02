/*
 * Copyright (C) 2008-2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2 or v.3.
 */

#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_types.h>
#include <net/if.h>
#include <net/route.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include "common.h"
#include "plugins.h"

int fw_packets(int argc, char **argv)
{
	if (argc > 1) {
		if (!strcmp(argv[1], "config")) {
			puts("graph_title Firewall Throughput\n"
			     "graph_args --base 1000 -l 0\n"
			     "graph_vlabel Packets/${graph_period}\n"
			     "graph_category network\n"
			     "received.label Received\n"
			     "received.graph no\n"
			     "received.type DERIVE\n"
			     "received.min 0\n"
			     "sent.label Sent\n"
			     "sent.draw AREA\n"
			     "sent.type DERIVE\n"
			     "sent.negative received\n"
			     "sent.min 0");
			return 0;
		}
		if (!strcmp(argv[1], "autoconf"))
			return writeyes();
	}

	int mib[] = { CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0 };
	size_t bufferSize;
	if (sysctl(mib, 6, NULL, &bufferSize, NULL, 0) != 0)
		return 1;

	char *interfaces = malloc(bufferSize);
	if (sysctl(mib, 6, interfaces, &bufferSize, NULL, 0) != 0)
		return 1;

	uint64_t receivedSum = 0;
	uint64_t sentSum = 0;
	char *interfacesStart = interfaces;
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

		receivedSum += ifdata->ifi_ipackets;
		sentSum     += ifdata->ifi_opackets;

next:
		interfaces += header->ifm_msglen;
	}

	printf("received.value %lu\n", receivedSum);
	printf("sent.value %lu\n", sentSum);

	return 0;
}
