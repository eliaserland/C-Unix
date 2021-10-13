#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <fcntl.h>
#include    <sys/time.h>
#include    <termios.h>

int main(int argc, char *argv[])
{
    fd_set          r_fds, err_fds;
    int             fifofd, numfds, n, i;
    char            buf[256];
    struct termios  tbuf, savebuf;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s FIFO\n", argv[0]);
        exit(1);
    }
    if ((fifofd = open(argv[1], O_RDONLY)) == -1) {
        fprintf(stderr, "FIFO must exist and be readable!\n");
        exit(1);
    }
    tcgetattr(STDIN_FILENO, &savebuf);
    tcgetattr(STDIN_FILENO, &tbuf);
    tbuf.c_lflag &= ~(ICANON | ECHO);
    tbuf.c_cc[VMIN] = 1;
    tbuf.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &tbuf);

    while (1) {
        FD_ZERO(&r_fds);
        FD_ZERO(&err_fds);
        FD_SET(fifofd, &r_fds);
        FD_SET(STDIN_FILENO, &r_fds);

        numfds = select(4, &r_fds, NULL,  &err_fds, NULL);
        if (numfds == 0)    /* Time expired - not    */
            continue;       /* valid in this example */

        if (FD_ISSET(STDIN_FILENO, &r_fds)) {
            n = read(STDIN_FILENO, buf, sizeof(buf));
            printf("read %d bytes from stdin:", n);
            for (i = 0; i < n; i++) putchar(buf[i]);
            if (buf[0] == 'q')
                break;
            putchar('\n');
        }
        if (FD_ISSET(fifofd, &r_fds)) {
            n = read(fifofd, &buf, sizeof(buf));
            printf("read %d bytes from FIFO:", n);
            for (i = 0; i < n; i++) putchar(buf[i]);
            putchar('\n');
        }
    }
    tcsetattr (0, TCSAFLUSH, &savebuf);
    return 0;
}

