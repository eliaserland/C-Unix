#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void set_fl(int, int);
void clr_fl(int, int);

int main(void)
{
    int     ntowrite;
    char    buf[1000];

    set_fl(STDIN_FILENO, O_NONBLOCK);  /* set nonblocking */

    while ((ntowrite = read(STDIN_FILENO, buf, sizeof(buf)))) {
        if (ntowrite > 0) {
            write(STDOUT_FILENO, buf, ntowrite);
        } else {
            write(STDOUT_FILENO, "nothing to read\n", 16);
            sleep(2);
        }
    }

    clr_fl(STDIN_FILENO, O_NONBLOCK);  /* clear nonblocking */
    exit(0);
}

void set_fl(int fd, int flags) /* flags are file status flags to turn on */
{
    int     val;

    if ( (val = fcntl(fd, F_GETFL, 0)) < 0)
        fprintf(stderr, "fcntl F_GETFL error\n");

    val |= flags;       /* turn on flags */

    if (fcntl(fd, F_SETFL, val) < 0)
        fprintf(stderr, "fcntl F_SETFL error\n");
}

void clr_fl(int fd, int flags) /* flags are file status flags to turn on */
{
    int     val;

    if ( (val = fcntl(fd, F_GETFL, 0)) < 0)
        fprintf(stderr, "fcntl F_GETFL error\n");

    val &= ~flags;       /* turn on flags */

    if (fcntl(fd, F_SETFL, val) < 0)
        fprintf(stderr, "fcntl F_SETFL error\n");
}
