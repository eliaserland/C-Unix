#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include    <poll.h>
#include <termios.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    struct pollfd   p[2];
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

    p[1].fd = fifofd;
    p[1].events = POLLIN;
    p[0].fd = STDIN_FILENO;
    p[0].events = POLLIN;

    while (1) {
        numfds = poll(p, 2, 2);
        if (numfds == 0)
            continue;
        if (p[0].revents) {
            if (p[0].revents & POLLIN) {
                n = read(p[0].fd, buf, sizeof(buf));
                printf("read %d bytes from stdin:", n);
                for (i = 0; i < n; i++)
                    putchar(buf[i]);
                if (buf[0] == 'q')
                    break;
                putchar('\n');
                if (p[0].revents ^ POLLIN) {
                    printf("Something else also...\n");
                    break;
                }
            } else {
                printf("Something else...\n");
                break;
            }
        }
        if (p[1].revents) {
            if (p[1].revents & POLLIN) {
                n = read(p[1].fd, &buf, sizeof(buf));
                printf("read %d bytes from FIFO:", n);
                for (i = 0; i < n; i++)
                    putchar(buf[i]);
                putchar('\n');
                if (p[1].revents ^ POLLIN) {
                    printf("Something else also...\n");
                    break;
                }
            } else {
                printf("Something else...\n");
                break;
            }
        }
    }
    tcsetattr (0, TCSAFLUSH, &savebuf);
    return 0;
}
