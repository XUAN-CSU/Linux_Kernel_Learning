#include "list.h"

#include <stdio.h>
#include <stdlib.h>

struct Car {
    int door_number;
    char *color;
    char *model;
    struct list_head list;
};

int main() 
{

    LIST_HEAD(carlist);

    struct Car *redcar = malloc(sizeof(struct Car));
    struct Car *bluecar = malloc(sizeof(struct Car));

    redcar->door_number = 4;
    redcar->color = "Red";
    redcar->model = "Sedan";
    INIT_LIST_HEAD(&redcar->list);

    bluecar->door_number = 2;
    bluecar->color = "Blue";
    bluecar->model = "Coupe";
    INIT_LIST_HEAD(&bluecar->list);

    list_add(&redcar->list, &carlist);
    list_add(&bluecar->list, &carlist);

    struct Car *car;
    printf("Car list:\n");
    list_for_each_entry(car, &carlist, list) {
        printf(" - %s %s with %d doors)\n", car->color, car->model, car->door_number);
    }

    free(redcar);
    free(bluecar);
    
    return 0;
}
