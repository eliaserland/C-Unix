#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define oops(s) {perror(s); exit(-1); }
#define PORT 1999

int main(void)
{
    char c;
    int n, s, sfd;
    struct sockaddr_in saddr;

    if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        oops("socket")

    /* Build the network address for this server */
    bzero(&saddr, sizeof (saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr *)&saddr, sizeof (saddr)) != 0)
        oops("bind")

    if (listen(s, 1) != 0)
        oops("listen")

    for (;;) {
        if ((sfd = accept(s, NULL, NULL)) == -1)
            oops("accept")

        /* Process input from sfd and write output to sfd */
        while ((n = recv(sfd, &c, sizeof(c), 0)) > 0) {
            putchar(c);
        }
        putchar('\n');

        close(sfd);
    }
    exit(0);
}
