#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    int fd;
	
    if (argc != 2) {
        fprintf(stderr, "This program takes one arguments\n");
        return 1;
    }
    if ((fd = open("fil", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR)) < 0) {
        perror(argv[0]);
        return 1;
    }

    if (dup2(fd, STDOUT_FILENO) < 0) {
        perror(argv[0]);
        close(fd);
        return 1;
    }
    execlp("ls", "ls", "-l", argv[1], (char *) NULL);
    perror("exec misslyckades\n");

    return 1;
}
