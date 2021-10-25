#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <stdarg.h>
char *vstrcat(char *first, ...);

int main(int argc, char *argv[])
{
    char *str;

    if (argc > 1) {
        fprintf(stderr, "%s takes no argument\n", argv[0]);
        exit(1);
    }

    if ((str = vstrcat("One", "Two", "Three", "Four", (char *)NULL)) == NULL) {
        fprintf(stderr, "felmeddelande...");
        exit(1);
    }
    printf("%ld   %s\n", strlen(str), str);
    free(str);

    if ((str = vstrcat("Usage: ", argv[0], " file", (char *)NULL)) == NULL) {
        fprintf(stderr, "felmeddelande...");
        exit(1);
    }
    printf("%ld   %s\n", strlen(str), str);
    free(str);
	return 0;
}







char *vstrcat(char *first, ...)
{
    size_t len;
    char *retbuf, *p;
    va_list argp;

    if (first == NULL) return NULL;
    len = strlen(first);
    va_start(argp, first);
    while((p = va_arg(argp, char *)) != NULL)
        len += strlen(p);
    va_end(argp);

    retbuf = malloc(len+1);
    if (retbuf == NULL)
        return NULL;
    strcpy(retbuf, first);

    va_start(argp, first);
    while((p = va_arg(argp, char *)) != NULL)
        strcat(retbuf, p);
    va_end(argp);

    return retbuf; /* Caller must call free() later */
}
