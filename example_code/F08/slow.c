#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <signal.h>
#define MY_LEN 256
static void sig_hdl(int);
int main(void)
{
    char buf[MY_LEN];
    int n;

    if (signal(SIGINT, sig_hdl) == SIG_ERR) {
        fprintf(stderr, "can't catch SIGINT");
        perror("");
        exit(1);
    }

    if ((n = read(STDIN_FILENO, buf, MY_LEN)) == -1) {
        perror("sigtst");
        exit(1);
    }
    buf[n-1] = '\0';
    printf("---%s---\n", buf);
    exit(0);
}
static void sig_hdl(int signo)
{
    if (signo == SIGINT)
      write(STDOUT_FILENO, "received SIGINT\n", 16);
    else
      write(STDERR_FILENO, "unknown signal\n", 15);
    return;
}
