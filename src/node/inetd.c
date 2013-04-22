/*
 * Copyright (C) 2013 Steve Schnepp <steve.schnepp@pwkf.org> - All rights reserved.
 * Copyright (C) 2013 Helmut Grohne <helmut@subdivi.de> - All rights reserved.
 * Copyright (C) 2013 Diego Elio Petteno <flameeyes@flameeyes.eu> - All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License v.2.
 */

#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#if !(defined(HAVE_WORKING_VFORK) || defined(S_SPLINT_S))
  #define vfork fork
#endif

int main(int argc, char *argv[]) {
	static const int yes = 1;
	char *s;
	struct sockaddr_in server;
	unsigned int port;
	int sock_listen, sock_accept;
	pid_t pid;

	if(argc < 3) {
		fprintf(stderr, "usage: %s [ipaddr:]port program "
				"[argv0 argv1 ...]\n", argv[0]);
		return 1;
	}
	
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	assert(argv[1] != NULL);
	s = strchr(argv[1], ':');
	if(NULL == s)
		s = argv[1];
	else {
		*s++ = '\0';
		if(0 != inet_aton(argv[1], &server.sin_addr)) {
			fprintf(stderr, "not an ip address: %s\n", argv[1]);
			return 1;
		}
	}
	if((1 != sscanf(s, "%u", &port)) ||
			port != (unsigned int)(uint16_t)port) {
		fprintf(stderr, "not a valid port: %s\n", s);
		return 1;
	}
	server.sin_port = htons(port);
	if((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket creation failed");
		return 1;
	}
	if(setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))
			== -1) { 
		perror("failed to set SO_REUSEADDR on socket");
	}
	if(bind(sock_listen, (struct sockaddr*)&server, sizeof(server)) < 0) {
		perror("failed to bind socket");
		close(sock_listen);
		return 1;
	}
	if(listen(sock_listen, 5) != 0) {
		perror("failed to listen on the socket");
		close(sock_listen);
		return 1;
	}
	while((sock_accept = accept(sock_listen, NULL, NULL)) != -1) {
		if(0 == (pid = vfork())) {
			close(sock_listen);
			dup2(sock_accept, 0);
			dup2(sock_accept, 1);
			close(sock_accept);
			execvp(argv[2], argv + 3);
			/* according to vfork(2) we must use _exit */
			_exit(1);
		} else {
			close(sock_accept);

			if(pid < 0) {
				perror("vfork failed");
				close(sock_listen);
				return 1;
			}
		}
	}
	perror("accept failed");
	close(sock_listen);
	return 1;
}
