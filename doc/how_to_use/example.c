// example.c - illustrates use of adtpp to support algebraic data types
// see example.adt for declarations/definitions that adtpp uses to generate
// example.h, included here

#include <stdio.h>

// define identifier myCtype using typedef if you want to use this
// type in ADT definitions - adtpp can't parse complex C types
typedef struct mystruct {int f1[5]; char f2[8];} myCtype;

// The Boehm et al. garbage collector, http://www.hboehm.info/gc/
// is a nice bit of work and can be used with adtpp as follows:
// #define ADT_MALLOC(s) GC_MALLOC(s)
// #define ADT_FREE(s) GC_FREE(s)

// include generated example.h (this includes <stdint.h> and
// <stdlib.h>)
#include "example.h"

// sum all the elements of a tree
long sum_tree(tree t) {
    switch_tree(t)
    case_Empty()
        return 0;
    case_Node(val, tl, tr)
        // val, tl and tr are fresh variables with scope just
	// this case, initialised to the arguments of the Node
        return val + sum_tree(tl) + sum_tree(tr);
    end_switch()
}

// increment all the elements of a tree
void inc_tree(tree t) {
    if_Node_ptr(t, valp, tlp, trp)
	(*valp)++;
        inc_tree(*tlp);
        inc_tree(*trp);
    end_if()
}

// sum list of integers, type ints or list<adt_int>
int sumlist(ints xs) {
    if_Cons_ints(xs, head, tail) // Note data constructor name
        return head + sumlist(tail);
    else()
        return 0;
    end_if()
}

// Example polymorphic code: return length of a list of anything
// Assuming length has been declared using a function declaration,
// and count_ints is declared as an instance, example.h will contain
// function prototypes for both functions plus a definition of
// count_ints that calls length
int
length(list xs) {
    int len = 0;
    while (1) {
        if_Cons(xs, head, tail)
            // type of head is adt_1 - can't do much with it except
            // assign it to something ot the same type
            len++;
            xs = tail;
        else()
            return len;
        end_if()
   }
}

// concatenate two lists (result shares second list; first list
// is not updated)
list concat(list xs, list ys) {
    if_Cons(xs, head, tail)
        return Cons(head, concat(tail, ys));
    else()
        return ys;
    end_if()
}

// takes Pair(x,y) and returns Pair(y,x)
pair_swp swap(pair xy) {
    if_Pair(xy, x, y)
        return Pair_pair_swp(y, x);
    end_if()
}

// zipWith from Haskell: given f, list [x1, x2,...] and list [y1, y2,...]
// return list [f(x1, y1), f(x2, y2),...]
// example.h contains the function prototype but we still need an ugly type for
// f in the C code here (it could be pinched from example.h then f inserted)
list_3 zipWith(adt_3 (*f)(adt_1, adt_2), list l1, list_2 l2){
    if_Cons(l1, hd1, tl1)
        if_Cons_list_2(l2, hd2, tl2)
            return Cons_list_3((*f)(hd1,hd2),
                               zipWith(f,tl1,tl2));
        else()
            return Nil_list_3();
        end_if();
    else()
        return Nil_list_3();
    end_if()
}

// map: given f and list [x1, x2,...] return list [f(x1), f(x2),...]
list_2 map(adt_2(*f)(adt_1), list l1){
    if_Cons(l1, hd1, tl1)
        return Cons_list_2((*f)(hd1), map(f,tl1));
    else()
        return Nil_list_2();
    end_if()
}

int main(int argc, char ** argv) {
    point origin, pt1, pt2, pt3; // a few variables of type point
    points pts1, pts2;
    colors cols;
    polygons polys;
    tree t1, t2, t3;

    origin = Point(0.0, 0.0);    // creates a point and assigns it to origin
    pt1 = Point(1.0, 1.0);
    pt2 = Point(0.0, 1.0);
    pt3 = Point(1.0, 0.0);

    t1 = Empty();
    t2 = Node(1, t1, Empty());
    t3 = Node(2, t2, t2);     // note: the subtrees are shared
    inc_tree(t3);             // t2 val incremented twice due to sharing!
    printf("Sum of nodes in t3 is %ld\n", sum_tree(t3));
    free_tree(t3);            // free memory for t3 only
    if_Node(t2, val, tl, tr)
        printf("t2 root value after increment is %ld\n", val);
    end_if();                 // semicolon optional

    // construct instances of polymorphic types: rather more verbose
    // than we would like because data constructors are renamed so the
    // type checker of C can pick up errors
    pts1 = Cons_points(origin,
                Cons_points(pt1, Cons_points(pt2, Nil_points())));
    pts2 = Cons_points(origin,
                Cons_points(pt1, Cons_points(pt3, Nil_points())));
    cols = Cons_colors(Red(), Cons_colors(Blue(), Nil_colors()));
    // use some higher order polymorphic functions: rather more
    // verbose again but *so* useful to have type checking of code like
    // this - it is hard to get it right the first time and much better
    // for a compiler to tell you where you have gone wrong than have a
    // runtime error. There are seven distinct list types to potentially
    // confuse just in the next line of code and errors casting to/from
    // void* will get past the compiler
    polys = mk_polygons(&Pair_polygon, cols,
                Cons_pointss(pts1, Cons_pointss(pts2, Nil_pointss())));
    printf("Length of swapped polygon list is %d\n",
        num_polygon_swps(map_swap_polygon(&swap_polygon, polys)));

    return 0;
}
