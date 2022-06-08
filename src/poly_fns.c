#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "adt_t.h"
#define WARNING_AVOID(val) \
if(val); //handle unused value to avoid warnings

// Polymorphic Functions
// ---------------------------------------------------------

// Reverses a list
list_t reverse_list(list_t input){
    if_cons(input, head, tail)
        return concat_lists(reverse_list(tail), cons(head, nil()));
    else()
        return nil();
    end_if()
}

// Returns the length of a list
int length(list_t list){
    if_cons(list, head, tail)
        WARNING_AVOID(head);
        return 1 + length(tail);
    else()
        return 0;
    end_if()
}

// Concatenates two lists
list_t concat_lists(list_t list1, list_t list2){
    if_cons(list1, lhead, ltail)
        return cons(lhead, concat_lists(ltail, list2));
    else()
        return list2;
    end_if()
}

// Returns head of list
adt_1 head(list_t a){
    if_cons(a, h, t)
        WARNING_AVOID(t);
        return h;
    else()
        printf("Error: Head function can only operate on non-empty list\n");
        exit(-1);
    end_if()
}

