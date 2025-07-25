#include <stdio.h>
#include <stddef.h>

#define container_of(ptr, type, member) ({ \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); })

struct person {
    int age;
    char *name;
};

int main()
{
    struct person classmate;
    classmate.age = 18;
    classmate.name = "Shirley";

    char *name_p = &classmate.name;
    struct person *girl = container_of(name_p, struct person, name);
    printf("The girl's name is %s\n", girl->name);
    return 0;
 }

