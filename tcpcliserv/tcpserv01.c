#include	"unp.h"

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	int opt;
	int nsecs = 360;
	while ((opt = getopt(argc, argv, "t:")) != -1) {
		switch (opt) {
		case 't':
			nsecs = atoi(optarg);
			break;
		default: /* '?' */
			fprintf(stderr, "Usage: %s [-t nsecs]\n",
			argv[0]);
			exit(EXIT_FAILURE);
		}
	}


	struct linger ling;
        struct timeval timeout;
        timeout.tv_sec = nsecs;
        timeout.tv_usec = 0;


	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
                ling.l_onoff = 1;
                ling.l_linger = 0;
                if (setsockopt(connfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0) {
                         perror("setsockopt error");
                }
                if (setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
                         perror("setsockopt error");
                }
                if (setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
                         perror("setsockopt error");
                }

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
			str_echo(connfd);	/* process the request */
			exit(0);
		}
		Close(connfd);			/* parent closes connected socket */
	}
}
