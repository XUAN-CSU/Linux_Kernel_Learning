#include <stdio.h>
#include <stddef.h>

#define container_of(ptr, type, member) ({ \
        const void *__mptr = (ptr); \
        (type *)(__mptr - offsetof(type, member)); })

struct person {
    int age;
    char *name;
};

struct family {
    struct person father;
    struct person mother;
    int number_of_sons;
};

int main()
{
    struct person classmate;
    classmate.age = 18;
    classmate.name = "Shirley";

    struct person wlc;
    wlc.age = 20;
    wlc.name = "WangLeicheng";

    struct family fam;
    fam.father = wlc;
    fam.mother = classmate;
    fam.number_of_sons = 3;

    char **name_p = &wlc.name;
    struct person *the_father = container_of(name_p, struct person, name);
    struct family *my_family = container_of(the_father, struct family, father);
    printf("The mother's name is %s\n", my_family->mother.name);
    return 0;
 }

