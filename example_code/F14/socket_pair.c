#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#define oops(s) {perror(s); exit(-1); }
#define DATA1 "In Xanadu, did Kublai Khan..."
#define DATA2 "A stately pleasure dome decree..."
int main(void)
{
   int sockets[2], child;
   char buf[1024];
/*
   På system med dubbelriktade pipor kan man använda:
   if (pipe(sockets) < 0)
*/
   if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0)
      oops("opening stream socket pair")
   if ((child = fork()) == -1) {
      oops("fork")
   } else if (child) {     /* This is the parent. */
      close(sockets[0]);
      if (read(sockets[1], buf, 1024) < 0)
         oops("reading stream message")
      printf("p->%s\n", buf);
      if (write(sockets[1], DATA2, sizeof(DATA2)) < 0)
         oops("writing stream message")
      close(sockets[1]);
   } else {     /* This is the child. */
      close(sockets[1]);
      if (write(sockets[0], DATA1, sizeof(DATA1)) < 0)
         oops("writing stream message")
      if (read(sockets[0], buf, 1024) < 0)
         oops("reading stream message")
      printf("c->%s\n", buf);
      close(sockets[0]);
   }
   return 0;
}
