#include    <stdio.h>
#include    <unistd.h>
#include    <signal.h>

void sighand(int signo);

int main(void) {
  sigset_t newmask, oldmask, pendmask;

  signal(SIGINT, sighand);
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGINT);
  sigprocmask(SIG_BLOCK, &newmask, &oldmask);
  puts("Blocking SIGINT\n");
  sleep(8);
  sigpending(&pendmask);
  if (sigismember(&pendmask, SIGINT))
    puts("\nSIGINT pending");
  sigprocmask(SIG_SETMASK, &oldmask, NULL);
  puts("\nSIGINT unblocked");
  sleep(5);
  return 0;
}

void sighand(int signo) {
  if (signo == SIGINT)
    write(STDOUT_FILENO, "Handled SIGINT\n", 15);
}
