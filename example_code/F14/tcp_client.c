#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define oops(s) {perror(s); exit(-1); }
#define HOSTNAME "fuggle"
#define PORT 1999

int main(int argc, char *argv[])
{
    int s;
    struct sockaddr_in saddr;
    struct hostent *hp;

    /* Build the network address of the server */
    bzero(&saddr, sizeof (saddr));
    saddr.sin_family = AF_INET;
    if ((hp = gethostbyname(HOSTNAME)) == NULL)
        oops("gethostbyname")
    bcopy(hp->h_addr, &saddr.sin_addr, hp->h_length);
    saddr.sin_port = htons(PORT);

    if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        oops("socket")

/*
    Here is the place to bind the socket to a port if the client
    needs a specific port number
*/

    if (connect(s, (struct sockaddr *)&saddr, sizeof (saddr)) != 0)
        oops("socket")

	if (argc == 1) {
		if (send(s, "Hello!", 6, 0) == -1)
			oops("write")
	} else {
		if (send(s, argv[1], strlen(argv[1]), 0) == -1)
			oops("write")
	}
    close(s);
    exit(0);
}
