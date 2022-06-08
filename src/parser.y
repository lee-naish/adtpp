
%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "adt_t.h"
extern int yylex();
extern int yyparse();
extern FILE *yyin;
#define CON_NUM_SIZE 5000
#define FILENAME_SIZE 200
#define ARRAY_SIZE 200
#define CONTENT_SIZE 50000
#define TYPE_LENGTH 500
#define TYPE_NUM 1000
#define QUANTITY_ADT_TYPES 20
#define WARNING_AVOID(val) \
if(val); //handle unused value to avoid warnings

typedef char *c_string;

// adtpp.c
extern void adt_main(char file_name[], c_string Type, c_string Con, stringlist_t arg_list, FILE* fp_h, int tag, int is_const, int cp, int np, int *is_init);

// polymorph.c
extern instlist_t map_to_gen_instance(instlist_t);
extern instlist_t put_gen_instance(decllist_t);
extern decllist_t param_instance_to_decl(instlist_t, decllist_t, instlist_t);
extern decllist_t instance_to_decl(instlist_t, instlist_t, decllist_t, instlist_t);
extern decllist_t remove_paramdecls(decllist_t);
extern void print_function_prototype_section(fnlist_t, instlist_t, c_string);
extern void print_function_alias_section(instlist_t, instlist_t, fnlist_t, c_string);
extern c_string construct_ADT_types_string(int);
extern c_string concat(c_string, c_string);
extern arglist_t argfunct_to_arg(arglist_t);
extern void print_out_args(arglist_t arg_list);
extern c_string function_pointer_start(arg_t);
extern c_string function_pointer_end(arg_t);
extern instlist_t expand_all_insts(instlist_t, instlist_t);
extern int eq_string(string_t, string_t);
extern int compare_a_list(arglist_t, arglist_t);
extern decl_t decl_lookup(c_string,  decllist_t);
extern void output_eqv_types(c_string, c_string, c_string, decl_t);
extern void output_undef_type(c_string, c_string, c_string);

//---------------         Definition of global variables       -----------------

decllist_t      d_list;     // Global list of declarations
instlist_t       i_list;      // Global list of declaration instances
fnlist_t          f_list;      // Global list of functions
instlist_t       f_i_list;    // Global list of function instances

FILE *fp_h;
int yydebug=0;

char constructor_list[CON_NUM_SIZE][TYPE_LENGTH];
int constructor_num = 0;

char file_name[FILENAME_SIZE];        //filename without the suffix
char file[FILENAME_SIZE];            //filename includes the suffix
char type_list[TYPE_NUM][TYPE_LENGTH]; //store the total type name
char g_file_name[ARRAY_SIZE];
int type_list_length = 0;

void print_decl(decl_t input);
void print_ctrlist(c_string d_str, stringlist_t con_list, stringlist_t arg_list, ctrlist_t input, int tag, int *cp, int *np);
void print_decllist(decllist_t input);
// void print_arglist(c_string d_str, c_string c_str, stringlist_t con_list, stringlist_t arg_list, arglist_t input, int tag, int *cp, int *np);
void remove_left_chev(char[]);


//-----------------------     Collection of type name    -----------------------
int collect_type_name(char filename[], char type_list[][TYPE_LENGTH]){
    FILE *fp;
    if((fp = fopen(filename, "r")) == NULL){
        fprintf(stderr, "Cannot open file %s\n", filename);
        exit(-1);
    }
    char indicator[TYPE_LENGTH], type_name[TYPE_LENGTH];
    int wrong_position = 0;
    char target[] = "data";
    int p = 0;
    while(fscanf(fp, "%s %s", indicator, type_name) != EOF){

        if(strcmp(indicator, target) == 0 && wrong_position == 0){
            //if indicater is "data", then type_name should be the type name
            int l = strlen(type_name);
            if(type_name[l - 1] == '{'){
                type_name[l - 1] = '\0';
                remove_left_chev(type_name);
                strcpy(type_list[p++], type_name);
            }else{
                remove_left_chev(type_name);
                strcpy(type_list[p++], type_name);
            }
        }else if(strcmp(type_name, target) == 0){
            //if the type_name is "data", then the type_name is in next round
            wrong_position = 1;
            continue;
        }else if(wrong_position == 1){
            //indicator should be the type name
            //clean the signal of wrong_position
            int l = strlen(indicator);
            wrong_position = 0;
            if(indicator[l - 1] == '{'){
                indicator[l - 1] = '\0';
                remove_left_chev(indicator);
                strcpy(type_list[p++], indicator);
            }else{
                remove_left_chev(indicator);
                strcpy(type_list[p++], indicator);
            }
        }
    }
    fclose(fp);
    return p;
}

int check_in_array(char type_list[][TYPE_LENGTH], int size, char input[]){
    int i;
    for(i = 0; i < size; i++){
        if(strcmp(type_list[i], input) == 0){
            return 1;
        }
    }
    return 0;
}


//------------------------------------------------------------------------------

char *remove_file_suffix(c_string mystr) {
    char *retstr;
    if (mystr == NULL)
         return NULL;
    if ((retstr = (char *)malloc(strlen(mystr) + 1)) == NULL)
        return NULL;
    strcpy (retstr, mystr);
    char *lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return retstr;
}

#ifndef ADTPP_VERSION
#define ADTPP_VERSION "1.??????"
#endif

// we use ifndef ADT_GEN_INCLUDE to avoid duplications (particularly the
// generic ADT structs used for type variables - these would otherwise cause
// redefinition errors if multiple .h files are included)
// XXX probably should do similar so the same .h file can be included
// multiple times (would need the file name here to do that).
// XXX Best also do some checking of sizeof(double) for adt_float plus
// do a bunch of mallocs and check ADT_LOW_BITS plus the malloc return value is
// greater than the maximum likely number of constants in an ADT (65536 should
// be sufficient for now).
void initializaion_files(FILE* fp_h){
    char content[CONTENT_SIZE];
    c_string h_file_include_content1, h_file_include_content;
    char h_file_include_head[] =
    "// DO NOT EDIT: generated by adtpp version "
    ADTPP_VERSION
    "\n// (even reading this may cause permanent mental impairment)\n\
#ifndef ADT_GEN_INCLUDE\n\
#define ADT_GEN_INCLUDE\n\
#include <stdint.h>\n\
#include <stdlib.h>\n\
typedef char *adt_string;\n\
typedef intptr_t adt_int;\n\
typedef intptr_t adt_char;\n\
typedef uintptr_t adt_uint;\n\
typedef double adt_float;\n\
#define end_switch() }}}}\n\
#define default() break;}} default: {{\n\
#define else() } else {\n\
#define end_if() }}\n\
#ifndef ADT_MALLOC\n\
#define ADT_MALLOC(s) malloc(s)\n\
#endif\n\
#ifndef ADT_FREE\n\
#define ADT_FREE(s) free(s)\n\
#endif\n\
#ifndef ADT_LOW_BITS\n\
#define ADT_LOW_BITS 7\n\
#endif\n\n";
    c_string h_file_include_ADT_type = construct_ADT_types_string(QUANTITY_ADT_TYPES);
    h_file_include_content1 = concat(h_file_include_head,h_file_include_ADT_type);
    h_file_include_content = concat(h_file_include_content1,"#endif// ADT_GEN_INCLUDE\n");

// XXX ADT_LOW_BITS should depend on architecture/malloc
// there is some code in the old repo to compute it, which probably
// should be used to generate lowbits.h which this code includes
// rather than hard-coding 7 above. Similarly, the size of adt_float,
// pointers to structs and functions and char compared to pointer to
// void should be checked and the value of pointers returned from
// ADT_MALLOC should be greater than the number of constants in any ADT
// Here we do a bit of basic checking
    // check a bunch of things are all the same size as void *
    if (sizeof(adt_float) != sizeof(void *))
        printf("Oops - check sizeof(adt_float)!\n");
    if (sizeof(char *) != sizeof(void *))
        printf("Oops - check sizeof(char *)!\n");
    if (sizeof(struct _ADT_Gen_1 *) != sizeof(void *))
        printf("Oops - check sizeof(struct pointers)!\n");
    if (sizeof(&initializaion_files) != sizeof(void *))
        printf("Oops - check sizeof(function pointers)!\n");
    if (sizeof(adt_float) != sizeof(void *))
        printf("Oops - check sizeof(adt_float)!\n");
    // do a few mallocs (really want ADT_MALLOC but we dont know what
    // that might be) and check addresses returned, plus addresses of
    // a static variable (in case ADT_MALLOC uses that) to check
    // alignment and addresses are not too small (small "adresses" are
    // used for constants in ADTs).
    { intptr_t i, addr, addr_or, addr_min; 
    addr_or =
    addr_min = (intptr_t) &d_list; // static var d_list
    for (i = 0; i < 10; i++) { // we use 10 malloc calls of various sizes
	addr = (intptr_t) malloc(1+i*9); // we don't free the space...
	addr_or |= addr;
        if (addr < addr_min)
            addr_min = addr;
    }
    if (addr_or & ADT_LOW_BITS)
        printf("Oops - check ADT_LOW_BITS! Address %lx\n", (long) addr_or);
    if (addr_min < 65536)
        printf("Oops - limit on number of constants in ADT %lx\n", (long) addr_min);
    }

//define the file name to be generated

    char path[] = "./";
    char suffix_1[] = ".h";
    char h_file_name[ARRAY_SIZE];


//definition for .h file name and path
    strcpy(h_file_name, path);
    strcat(h_file_name, file_name);
    strcat(h_file_name, suffix_1);
    
    strcpy(g_file_name, h_file_name);


    fp_h =fopen(h_file_name, "w+");
    strcpy(content, h_file_include_content);
    fputs(content, fp_h);

    fclose(fp_h);
    // printf("Initialize.h files successfully\n");
}

//check whether the constructors have been defined before
void check_exist_con(char *s){
    int i = 0;
    for(i = 0; i < constructor_num; i++){
        if(strcmp(constructor_list[i], s) == 0){
            fprintf(stderr, "\nERROR: constructor \"%s\" multiply defined\n\n", s);
              // exit(-1); // best carry on???
        }
    }
}

// void con_list_print(stringlist_t con_list){
//         if_cons_stringlist_t(con_list, con_val, con_next)
//             if_type_string(con_val, con_str)
//                 printf("print out the con : %s\n", con_str);
//                 con_list_print(con_next);
//             end_if()
//         end_if()
// 
//         if_nil_stringlist_t(con_list)
//             return;
//         end_if()
// }

c_string type_normalization(c_string input, c_string temp){
    //Check if the type is within the prim_types.
    //normalize the type when out of range of prim types.
    int NOT_PRIM_TPYE = check_in_array(type_list, type_list_length, input);

    if(NOT_PRIM_TPYE == 0){
        return input;
    }else{
        strcpy(temp, "struct _ADT_");
        strcat(temp, input);
        strcat(temp, "*");
        temp[strlen(temp)] = '\0';
        return temp;
    }
}

int is_const(ctr_t input){
    if_ctr(input, c_val, c_arglist)
        if_cons_arglist_t(c_arglist, c_arg, c_next)
            if(c_arg && c_next && c_val); //handle unused value to avoid warnings
            return 0;
        end_if()

        if_nil_arglist_t(c_arglist)
            return 1;
        end_if()
    end_if()
    return 0;
}
//------------------------------------------------------------------------------

void fetch_arglist(arglist_t input, stringlist_t *temp_arglist){
    while(1){
    if_cons_arglist_t(input, al_head, al_tail)
        if_arg(al_head, a_val)
            if_type_string(a_val, a_str)
                if(check_in_array(type_list, type_list_length, a_str)){
                    char *temp_string_for_type = (char *)malloc(sizeof(char) * (TYPE_LENGTH));    // add space for normalization
                    a_str = type_normalization(a_str , temp_string_for_type);
                }
                *temp_arglist = cons_stringlist_t(type_string(a_str), *temp_arglist);
            end_if()
        else_if_argfunct(r_list, arg_list)
            WARNING_AVOID(r_list && arg_list);
            
            c_string start_arg_func = function_pointer_start(al_head);
            c_string end_arg_func = function_pointer_end(al_head);
            
            *temp_arglist = cons_stringlist_t(func_string(start_arg_func, end_arg_func), *temp_arglist);
	else()
		printf("Error: unknown type parameter\n");
        end_if()
	input = al_tail;
    else()
        return;
    end_if()
    }
}
// arg_list = cons_stringlist_t(type_string(a_str), arg_list);

//-----------------------     Function Definition     --------------------------

// should have been written long ago and used all over the place
char *string_string(string_t d_w) {
    if_type_string(d_w, d_s)
        return d_s;
    else()
        exit(-1);
    end_if()
}

char *decl_name(decl_t d) {
    if_decl(d, d_n, d_cs)
        return string_string(d_n);
    else_if_paramdecl(d_n, d_cs, d_ps)
        return string_string(d_n);
    end_if()
    return ""; // won't happen
}


// handle type equivalences:
// scan though inst list.  If we have a (monomorphic) inst
// where RHS is undefined or a monomorphic type (defined by
// data declaration), output equivalence and delete the inst.
// Otherwise, if we have a (monomorphic) inst, each
// subsequent one which is equivalent (same RHS) should be deleted
// and macros for the later type defined in terms of the former type
// should be output.
void do_type_eqv(instlist_t *i_listp, decllist_t d_list){
    instlist_t *ilp;
    c_string i_str, il_str, d_str, f_d_str, dl_str;
    decl_t f_dec;

    if_cons_instlist_t_ptr(*i_listp, i_headp, i_tailp)
        if_inst(*i_headp, i_h_name, i_h_decl, i_h_paramlist) {
            i_str = string_string(i_h_name);
            d_str = string_string(i_h_decl);
            f_dec = decl_lookup(d_str, d_list);
            f_d_str = decl_name(f_dec);
// printf("do_type_eqv '%s',  '%s'\n", i_str, f_d_str);
            if (!*f_d_str || // empty string = undefined decl
                    i_h_paramlist== nil_arglist_t()) { // monomorphic
                output_undef_type(file_name, i_str, d_str);
		// remove this inst from list
		*i_listp = *i_tailp;
            } else {
                // scan through looking inst with for same RHS
                ilp = i_tailp;
// printf("searching for dups of %s;\n", i_str);
                while(1) {
                    if_cons_instlist_t_ptr(*ilp, il_headp, il_tailp)
                        if_inst(*il_headp, il_h_name, il_h_decl, il_h_paramlist) 
                            il_str = string_string(il_h_name);
                            dl_str = string_string(il_h_decl);
// XXX ?here? (or somewhere else) we should check for types being
// multiply defined.  Here we can check multiply defined
// monomorphic types easily (which affect the output)
// if(!strcmp(d_str, dl_str))
// printf("i_str, dl_str, d_str %s, %s %s;\n", i_str, dl_str, d_str);
                            if (!strcmp(d_str, dl_str)
                                   && !compare_a_list(i_h_paramlist, il_h_paramlist)) {
                                output_eqv_types(file_name, i_str, il_str, f_dec);
// printf("deleting %s, same as %s;\n", il_str, i_str);
                                // remove this inst from list
                                *ilp = *il_tailp;
                            }
                        end_if() // ignore paraminsts - no longer needed??
                        ilp = il_tailp;
                    else()
                        break; // reached end of *ilp
                    end_if()
                }
            }
        } end_if() // ignore paraminsts (could delete from list)
        do_type_eqv(i_tailp, d_list);
    end_if()
    return;
}

void print_decllist(decllist_t input){
    if_cons_decllist_t(input, dl_head, dl_tail)
//         printf("\nDeclaration\n");
        print_decl(dl_head);            //print out current decl
        print_decllist(dl_tail);        //turn to next decl
    else()
        return;
    end_if()
}

// print out the string in decl
void print_decl(decl_t input){
    //store the list of cons and arguments for each decl
    stringlist_t con_list, arg_list;
    //count the number of const and non-const for tagging
    int const_num = 0;
    int non_const_num = 0;
    int *const_pointer;
    int *non_const_pointer;
    const_pointer = &const_num;
    non_const_pointer = &non_const_num;

    con_list = nil_stringlist_t();
    arg_list = nil_stringlist_t();
    if_decl(input, d_head, d_tail)
        if_type_string(d_head, d_str)
//             printf("data %s {\n", d_str);
            // we don't really want any printing done here but this function
            // changes global var constructor_num so we can't just delete it:(
            print_ctrlist(d_str, con_list, arg_list, reverse_ctrlist(d_tail), 0, const_pointer, non_const_pointer);
//             //before enter the next decl, generate the corresponding codes.
//             //print out the number of const and non-const
//             printf("}\n");
            // printf("# of Constant: %d, # of Non-Constant: %d\n", *const_pointer, *non_const_pointer);
            int const_tag = 0;
            int non_const_tag = 0;
            stringlist_t temp_arglist;
            stringlist_t *p_arglist;
            p_arglist = &temp_arglist;
            ctrlist_t temp_ctrlist = reverse_ctrlist(d_tail);
            int initial = 0;
            int *init = &initial;
            while(1){
                if_cons_ctrlist_t(temp_ctrlist, con_val, con_next)
                    //initial the temp arglist for each con
                    temp_arglist = nil_stringlist_t();
                    if_ctr(con_val, c_val, c_arglist)
                        if_type_string(c_val, c_str)
                            
                            fetch_arglist(c_arglist, p_arglist);
                            // For non-const
                            if_cons_arglist_t(c_arglist, al_head, al_tail)
                                if(al_head && al_tail); //handle unused value to avoid warnings
                                adt_main(file_name, d_str, c_str, reverse_stringlist(temp_arglist), fp_h, non_const_tag++, 0, *const_pointer, *non_const_pointer, init);
                            end_if()

                            // For const
                            if_nil_arglist_t(c_arglist)
                                adt_main(file_name, d_str, c_str, reverse_stringlist(temp_arglist), fp_h, const_tag++, 1, *const_pointer, *non_const_pointer, init);
                            end_if()

                        end_if()
                        temp_ctrlist = con_next;
                    end_if()
                end_if()

                // end of conlist, then break
                if_nil_ctrlist_t(temp_ctrlist)
                    break;
                end_if()
            }

        end_if()
    end_if();

}

// count the number of const and non-const
void count_number_of_const_nonconst(arglist_t input, int *cp, int *np){
    //if there is a number of arguments, the number of
    //non-const add 1
    if_cons_arglist_t(input, arg_val, arg_next)
        WARNING_AVOID(arg_val && arg_next)
        *np = *np + 1;
    end_if()

    //if there is no any argument, then it is a const
    //the number of const add 1
    if_nil_arglist_t(input)
        *cp = *cp + 1;
    end_if()
}

//print out the ctrlist
void print_ctrlist(c_string d_str, stringlist_t con_list, stringlist_t arg_list, ctrlist_t input, int tag, int *cp, int *np){
    if_cons_ctrlist_t(input, cl_head, cl_tail)
        // For each cons_ctrlist_t contains ctr and the rest of ctrlist
        if_ctr(cl_head, c_head, c_tail)
            if_type_string(c_head, c_str)
                check_exist_con(c_str);
                strcpy(constructor_list[constructor_num++], c_str);
//                 printf("    %s(", c_str);
                //count the number of const and non-const
                count_number_of_const_nonconst(c_tail, cp, np);
                // Add up the new con
                con_list = cons_stringlist_t(type_string(c_str), con_list);
//                 print_arglist(d_str, c_str, con_list, arg_list, c_tail, tag, cp, np);
//                 printf(");\n");
                print_ctrlist(d_str, con_list, arg_list, cl_tail, ++tag, cp, np);

            end_if()
        end_if()

    end_if()

    if_nil_ctrlist_t(input){                     // end of ctrlist
        con_list = reverse_stringlist(con_list);
        return;
    }
    end_if()
}

// // XXX various args not used
// // not much point in all this printing anyway - its mostly noise
// // on stdout whichis ignored.
// // The main printing to the .h file is done by adt_main, called directly from
// // print_decl (awful...)
// void print_arglist(c_string d_str, c_string c_str, stringlist_t con_list, stringlist_t arg_list, arglist_t input, int tag, int *cp, int *np){
//     if_cons_arglist_t(input, al_head, al_tail)
//         if_arg(al_head, a)
//             if_type_string(a, a_str)
//                 printf("%s", a_str);
//                 if(check_in_array(type_list, type_list_length, a_str)){
//                     char *temp_string_for_type = (char *)malloc(sizeof(char) * (TYPE_LENGTH));    // add space for normalization
//                     a_str = type_normalization(a_str , temp_string_for_type);
//                 }
//                 arg_list = cons_stringlist_t(type_string(a_str), arg_list);
//                 if (al_tail != nil_arglist_t()) {
//                     printf(", ");
//                     print_arglist(d_str, c_str, con_list, arg_list, al_tail, tag, cp, np);            //turn to tail arglist
//                 }
//             end_if()
//         else_if_argfunct(r_list, a_list)
//             WARNING_AVOID(r_list && a_list);
//             c_string start_arg_func = function_pointer_start(al_head);
//             c_string end_arg_func = function_pointer_end(al_head);
//             // printf("%s%s", start_arg_func, end_arg_func);
//             // XXX NQR? - prints commas if r_list has > 1 element
//             print_arglist(d_str, c_str, con_list, arg_list, r_list, tag, cp, np);
//             printf(" func(");
//             print_arglist(d_str, c_str, con_list, arg_list, a_list, tag, cp, np);
//             printf(")");
//             arg_list = cons_stringlist_t(func_string(start_arg_func, end_arg_func), arg_list);
//             if (al_tail != nil_arglist_t()) {
//                 printf(", ");
//                 print_arglist(d_str, c_str, con_list, arg_list, al_tail, tag, cp, np);            //turn to tail arglist
//             }
//         end_if()
//     end_if()
// 
//     //end of arglist
//     if_nil_arglist_t(input)
//         return;
//     end_if()
// }


void check_suffix(char *target, char *suffix){
    int t_len = strlen(target);
    int s_len = strlen(suffix);
    int i = 0;
    while(s_len > 0){
        if(target[t_len - s_len] != suffix[i]){
            fprintf(stderr, "\nERROR: filename should end with \"%s\"\n\n",  suffix);
            exit(-1);
        }
        i++;
        s_len = s_len - 1;
    }
}

// Takes a string and stops it at the first '<'
void remove_left_chev(char type_name[]){
    int len = strlen(type_name);
    int i = 0;
    while(i < len){
        if (type_name[i] == '<'){
            type_name[i] = '\0';
            break;
        }
        i++;
    }
}

// Prints string array visually for inspection
void print_string_array(char type_list[][TYPE_LENGTH], int size){
    int i;
    for(i = 0; i < size; i++){
        printf("%s\n", type_list[i]);
    }
}

// Places new instance data types in type_list array
int instances_to_type_list(instlist_t inst_list, char type_list[][TYPE_LENGTH], int size){
    if_cons_instlist_t(inst_list, i_head, i_tail)
        if_inst(i_head, i_h_name, i_h_decl, i_h_paramlist)
            WARNING_AVOID(i_h_paramlist && i_h_decl);
            if_type_string(i_h_name, i_h_string)
                strcpy(type_list[size++], i_h_string);
            end_if()
        end_if()
        return instances_to_type_list(i_tail, type_list, size);
    else()
        return size;
    end_if()
}

// ------------------------------------------------------------------------------ //


void yyerror(const char *str)            //default function for handliing errors in yacc
{
    fprintf(stderr,"error: %s\n",str);
}


int yywrap()
{
    return 1;
}



//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    decllist_t d_list_pi, d_list_gen_d;
    instlist_t i_list_gen_d, i_list_gen_pi, exp_i_list;
    //Too many arguments

    if(argc > 2){
        fprintf(stderr, "\nERROR: Too many parameters, please input the name of adt file, should end with \".adt\"\n\n");
          exit(-1);
    }else if(argc == 1){
        fprintf(stderr, "\nERROR: Too few parameters, please input the name of adt file, should end with \".adt\"\n\n");
          exit(-1);
    }else{
        char suffix[] = ".adt";
        check_suffix(argv[1], suffix);
    }

    strcpy(file_name, remove_file_suffix(argv[1]));

    strcpy(file, argv[1]);
    FILE *f;
    if((f = fopen(file, "r")) == NULL){
        fprintf(stderr, "\nERROR: Cannot find %s, please check the filename, should end with \".adt\"\n\n", file);
          exit(-1);
    }
    // parse the ADT file and get the whole type name
    // type_list_length is the total number of type name
    // XXX this is awful - the file should be parsed
    // just once, with the proper parser, and this info
    // extracted if needed
    type_list_length = collect_type_name(file, type_list);

//     printf("// output file: %s.h, source file: %s\n", file_name, file);
    // printf("New version started\n");
    initializaion_files(fp_h);
    
    //-------------    global variables initializaion     ----------------------
    d_list = nil_decllist_t();
    i_list = nil_instlist_t();
    f_list = nil_fnlist_t();
    f_i_list = nil_instlist_t();
    //--------------------------------------------------------------------------
    // parse the input files




    yyin = f;
    do{
        if(yyparse() != 0)
        printf("Syntax Error");
    }while(!feof(yyin));


    fclose(yyin);

    // Add instance names to type_list and update type_list_length
    type_list_length = instances_to_type_list(i_list, type_list, type_list_length);
    // XXX need to distinguish gen_instance types from d_list here with
    // gen_instance types from decls returned from
    // param_instance_to_decl so former don't have postfixed data cons
    // names
    // Need complete i_list + orig d_list + d_list from poly insts to
    // process the two separate classes of generic monomorphic insts.

    // reverse to make order etc more pretty/intuitive
    i_list = reverse_instlist(i_list);
    d_list = reverse_decllist(d_list);
    // get lists of all implicit generic instances from d_list, i_list
    i_list_gen_d = put_gen_instance(d_list);
    i_list_gen_pi = map_to_gen_instance(i_list);
    // get list of all instances, including implicit generic ones
    // we put the latter first so if there are equivalent types we use
    // the generic names and have simpler constructor names
    i_list = concat_inst_list(i_list_gen_d, concat_inst_list(i_list_gen_pi, i_list));
    // get expanded version of all insts
    exp_i_list = expand_all_insts(i_list, i_list);
    // convert all paraminsts to decls and add to d_list
    d_list_pi = param_instance_to_decl(exp_i_list, d_list, exp_i_list);
    d_list = concat_decl_list(d_list_pi, d_list);
    // print and delete all insts equivalent to previous insts
    // (exp_i_list can be modified here)
    do_type_eqv(&exp_i_list, d_list);
    // convert (monomorphic) insts into decls
    // Pass in list of generic insts from decls - all other insts need
    // new constructor names
    d_list_gen_d = instance_to_decl(i_list_gen_d, exp_i_list, d_list, exp_i_list);
//    d_list_gen_pi = instance_to_decl(1, 
 //       exp_i_list, d_list, exp_i_list);
    
    // Removes polymorphic declarations
    d_list = remove_paramdecls(d_list);
    // add decls generated from (monomorphic) insts
    d_list = concat_decl_list(d_list, d_list_gen_d);

    // Print out decllist
    print_decllist(d_list);
    
    // Print Function Prototypes
    print_function_prototype_section(f_list, exp_i_list, g_file_name);
    
    // Print Function alias
    print_function_alias_section(f_i_list, exp_i_list, f_list, g_file_name);


//     printf("\n// All done\n");
    return 0;
}
%}

//------------------------------------------------------------------------------





//--------------------  Token Definition ---------------------------------------



%union                     //redifine the global variable of yylval
                        // which used in the test.l file
{
        int number;
        char *string;
        
// XXX use std type names from adt_t.h
        struct _ADT_decl_t*  decl_val;
        struct _ADT_ctr_t* ctr_val;
        struct _ADT_arg_t* arg_val;
//         struct _ADT_param_t* param_val;
        struct _ADT_arg_t* param_val;
        struct _ADT_fn_t* fn_val;
        struct _ADT_inst_t* inst_val;
        
        struct _ADT_ctrlist_t* ctr_list_val;
        struct _ADT_arglist_t* arg_list_val;
        struct _ADT_arglist_t* param_list_val;
        struct _ADT_instlist_t* inst_list_val;
        struct _ADT_fnlist_t* fn_list_val;   
}


%token <number> NUMBER
%token <string> WORD
%token OBRACE EBRACE SEMICOLON FBRACE SBRACE COMMA DATA
%token OCHEV ECHEV DATAASS COLON INST FUNC FINST HOF




%type <fn_val>               function_prototype;
%type <arg_list_val>     function_dec;
%type <arg_list_val>          function_definition;
%type <inst_list_val>          instances;
%type <inst_val>                instance;
%type <decl_val>                data_type_definition;
%type <arg_val>                 data_name;
%type <arg_list_val>      param_var_list;
%type <ctr_list_val>           definition_content;
%type <ctr_list_val>           type_statements;
%type <ctr_val>                 type_statement;
%type <arg_list_val>         arguments;


//------------------------------------------------------------------------------






//-----------------------Parsing Structure Definition --------------------------

%%
commands:                    //regards all strings as "commands" and each
                            //"commands"  contain a number of "command"
    |
    command commands
    ;


command:
    data_type_definition maybesemicolon
    {
        d_list = cons_decllist_t($1, d_list);
    }
    |
    INST instances maybesemicolon
    {
        i_list = concat_inst_list($2, i_list);
    }
    |
    function_prototype maybesemicolon
    {
        f_list = cons_fnlist_t($1, f_list);
    }
    |
    FINST instances maybesemicolon
    {
        f_i_list = concat_inst_list($2, f_i_list);
    }
    ;
    
function_prototype:
// XXX arguments should be single data_name (arg, for return type) and
// type should be arg_t, not arglist_t???
     function_dec arguments WORD function_definition
    {
        $$ = fn(type_string($3), reverse_arglist($2), $4, $1);
    }
;

function_dec:
    FUNC OCHEV param_var_list ECHEV
    {
        $$ = $3;
    }
;
    
// XXX reverse_arglist here and when this is processed....?
function_definition:
    FBRACE arguments SBRACE
    {
        $$ = reverse_arglist($2);
    }
    |
    FBRACE SBRACE
    {
        $$ = nil_arglist_t();
    }
;

// Parametric Polymorphic Instances
instances:
    instance SEMICOLON
    {
        $$ = cons_instlist_t($1, nil_instlist_t());
    }
    |
    instance COMMA instances SEMICOLON
    {
        $$ = cons_instlist_t($1, $3);
    }
    ;

instance:
   
    WORD OCHEV param_var_list ECHEV DATAASS WORD OCHEV arguments ECHEV
    {
        $$ = paraminst(type_string($1),$3,type_string($6),$8);
    }
    
    |
    WORD DATAASS WORD OCHEV arguments ECHEV
    {
        $$ = inst(type_string($1),type_string($3),$5);
    }
    |
    WORD DATAASS WORD
    {
        $$ = inst(type_string($1),type_string($3),nil_arglist_t());
    }
    ;

maybesemicolon: // optional semicolon after closing brace of type definition
    | SEMICOLON
    ;


data_type_definition:        //each type definition need a token of DATA, and
                            //data type name as WORD which can be any string
                            //with a detail content of definition
    DATA WORD definition_content
    {
        $$ = decl(type_string($2), reverse_ctrlist($3));
    }
    |
    DATA WORD OCHEV param_var_list ECHEV definition_content
    {
        $$ = paramdecl(type_string($2), reverse_ctrlist($6), $4);
    }
    ;

data_name:
// get rid of paramarg - just use arg
//     OCHEV WORD ECHEV
//     {
//         $$ = paramarg(type_string($2));
//     }
//     |
    // WORD OCHEV param_var_list ECHEV 
    WORD OCHEV arguments ECHEV 
    {
        $$ = paramdata(type_string($1), $3);
    }
    |
    data_name HOF FBRACE arguments SBRACE
    {
        
        $$ = argfunct(cons_arglist_t($1, nil_arglist_t()), $4);
    }
    |
    data_name HOF FBRACE SBRACE
    {
        
        $$ = argfunct(cons_arglist_t($1, nil_arglist_t()), nil_arglist_t());
    }
    |
    WORD
    {
        $$ = arg(type_string($1));
    }
;


param_var_list:
    WORD COMMA param_var_list
    {
        $$ = cons_arglist_t(arg(type_string($1)), $3);
    }
    | WORD
    { 
        $$ = cons_arglist_t(arg(type_string($1)), nil_arglist_t());
    }
;

definition_content:            //a definition content contains an open brace, type
                            //statements and a close brace
    OBRACE type_statements EBRACE
    {
        $$ = $2;
    }
;


type_statements:

    {
        $$ = nil_ctrlist_t();
    }
    |
    type_statement SEMICOLON type_statements
    {
        $$ = cons_ctrlist_t($1, $3);
    }
    ;


type_statement:                //There are two kinds of type statement: with
                            //arguments and non-argument
    WORD FBRACE arguments SBRACE
    {
        $$ = ctr(type_string($1), ($3));
    }
    |
    WORD FBRACE SBRACE
    {
        $$ = ctr(type_string($1), nil_arglist_t());
    }

    ;
arguments: 
    data_name COMMA arguments
    {
        $$ = cons_arglist_t($1, $3);
    }
    |
    data_name
    {
        $$ = cons_arglist_t($1, nil_arglist_t());
    }
    ;
    

%%

//------------------------------------------------------------------------------
