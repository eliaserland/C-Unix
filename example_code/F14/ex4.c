#include <stdio.h>

int main(void) {
    int passwordChecked=0;
    char name[8];
    printf("Enter your name: ");
    scanf("%s",name);
    while (!passwordChecked) {
        printf("mata in lösenord: \n");
        /*läs in och kolla lösenord */
        getchar();
        if (getchar() == 'x') {
          passwordChecked=1;
        }
    }
    printf("Nu borde lösenordet vara inmatat\n");

    return 0;
}

