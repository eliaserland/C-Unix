#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    int i;
    struct passwd *pw;

    if ((pw = getpwuid(getuid())) == NULL) {
        fprintf(stderr, "ERROR using getpwuid\n");
        exit(1);
    }
    printf("Your own password entry:\n");
    printf("username:\t%s\n", pw->pw_name);
    printf("password:\t%s\n", pw->pw_passwd);
    printf("uid:\t\t%d\n", pw->pw_uid);
    printf("gid:\t\t%d\n", pw->pw_gid);
    printf("comment:\t%s\n", pw->pw_gecos);
    printf("home dir:\t%s\n", pw->pw_dir);
    printf("shell:\t%s\n", pw->pw_shell);

    for (i = 1; i < argc; i++)
        if ((pw = getpwnam(argv[i])) == NULL)
            printf("%s: not found in passwd\n", argv[i]);
        else
            printf("The user id for %s is %d\n", argv[i], pw->pw_uid);

    return 0;
}
