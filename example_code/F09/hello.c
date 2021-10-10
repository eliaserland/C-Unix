#include <pthread.h>
#include <stdio.h>

void *print_message_function( void *ptr );

int main(void) {
   pthread_t thread1, thread2;
   char *message1 = "Hello", *message2 = "World";
   pthread_create(&thread1, NULL, print_message_function,
                  (void*)message1);
   pthread_create(&thread2, NULL, print_message_function,
                  (void*)message2);
   pthread_join(thread1, NULL);
   pthread_join(thread2, NULL);
   return 0;
}

void *print_message_function( void *ptr ) {
   char *message;
   message = (char *) ptr;
   printf("%s ", message);
   return NULL;
}
