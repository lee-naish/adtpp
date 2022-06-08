#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "adt_t.h"
#define WARNING_AVOID(val) \
if(val); //handle unused value to avoid warnings

typedef char *c_string;

c_string param_concat(c_string, arglist_t, int);
c_string concat(c_string, c_string);
c_string construct_ADT_types_string(int);
c_string arg_concat(c_string, arglist_t, int);
c_string function_pointer_start(arg_t);
c_string function_pointer_end(arg_t);
// c_string find_instance_a(c_string, arglist_t, instlist_t);
arg_t simp_type(c_string, arglist_t, instlist_t);
arglist_t map_simp_type(arglist_t, instlist_t);
arg_t simp_top_type(c_string, arglist_t, instlist_t);
arg_t find_instance_a1(c_string, arglist_t, instlist_t, instlist_t);
int eq_string(string_t, string_t);
arg_t expand_with_insts(arg_t, instlist_t);
arg_t expand_with_insts1(arg_t, instlist_t, instlist_t, int);
arglist_t map_expand_with_insts(arglist_t, instlist_t);
instlist_t expand_all_insts(instlist_t, instlist_t);
arg_t expand_with_exp_insts(arg_t, instlist_t);
arg_t expand_with_exp_insts1(arg_t, instlist_t, instlist_t);
arglist_t map_expand_with_exp_insts(arglist_t, instlist_t);
arglist_t replace_sec_paramlist_a(arglist_t, arglist_t, arglist_t);
arg_t find_mono_inst(arg_t, instlist_t);
arglist_t map_find_mono_inst(arglist_t, instlist_t);
arglist_t clone_arg(arglist_t, arglist_t, arglist_t, instlist_t );
arglist_t clone_arg_a(arglist_t, arglist_t, arglist_t, instlist_t );
instlist_t map_to_gen_instance(instlist_t);
int inst_member(c_string, instlist_t);

// Concatenates two strings
c_string concat(c_string s_1, c_string s_2){
    c_string result = malloc(strlen(s_1) + strlen(s_2) + 1);
    
    strcpy(result, s_1);
    strcat(result, s_2);
    
    return result;
}

// Takes a string (name of declaration) and returns a decl_t
// from decllist_t with that name.
// XXX should check number of parameters also
// XXX Should return maybe_decl_t ?  Instead we return decl with empty
// string to indicate not defined
decl_t decl_lookup(c_string target,  decllist_t source_list){
    decl_t new_decl;
    
    if_cons_decllist_t(source_list, source_head, source_tail)
        if_paramdecl(source_head, s_head_decl, s_head_ctr_list, s_head_param_list)
            if_type_string(s_head_decl, s_h_decl_name)

                if (strcmp(s_h_decl_name,target)==0) {
                    new_decl = source_head;
                    return new_decl;
                } else {
                    return decl_lookup(target, source_tail);
                }
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
            WARNING_AVOID(s_head_param_list && s_head_ctr_list);
        else_if_decl(s_head_decl, s_head_ctr_list)
            if_type_string(s_head_decl, s_h_decl_name)

                if (strcmp(s_h_decl_name,target)==0) {
                    new_decl = source_head;
                    return new_decl;
                } else {
                    return decl_lookup(target, source_tail);
                }
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
        else()
            printf("Error: decl_lookup");
            exit(-1);
        end_if()
    else()
        // XXX want to avoid error for things which might be defined using
        // typedef etc in user code; we return generic decl with no
        // constructors
        // Should be an error for polymorphic decls but we can't
        // distinguish them here
        fprintf(stderr, "\n// Warning: No declaration of %s\n",  target);
        return decl(type_string(""), nil_ctrlist_t()); // for undefined
    end_if()
}

// Used to lookup a parameter in a data body and replace it with
// the instance argument type
// Takes a string (name of polymorphic temp)
// and returns the substitution 
arg_t lookup_paramlist_a(c_string target, arglist_t replace_args, arglist_t source_params) {

    // XXX should #define MAXSTRLEN 100 at the very least
    c_string found_string = (c_string) malloc(100);

    if_cons_arglist_t(replace_args, r_p_head, r_p_tail)
        if_cons_arglist_t(source_params, s_p_head, s_p_tail)
            if_arg(s_p_head, s_p_head_name)
                if_type_string(s_p_head_name, s_p_head_string)
                
                    if (strcmp(s_p_head_string, target) == 0) {
                        
                        return r_p_head;
//                         if_param(r_p_head, r_p_head_name)
//                             if_type_string(r_p_head_name, r_p_head_string)
//                             
//                                 strcpy(found_string, r_p_head_string);
//                                 return found_string; 
//                             else()
//                                 printf("Error: String Type\n;");
//                                 exit(-1);
//                             end_if()
//                         end_if()
//                         
                    } else {
                        return lookup_paramlist_a(target, r_p_tail, s_p_tail);
                    }
                else()
                    printf("Error: String Type\n;");
                    exit(-1);
                end_if()
            else()
		printf("Error: atomic expression expected\n");
                exit(-1);
            end_if()
        else()
        
            fprintf(stderr, "\nERROR: Instance poly temp count different from declaration");
            exit(-1);
            
        end_if()   
    else()
            // not found - return original
            return arg(type_string(target));
            
    end_if()
    
}


// Used to lookup a parameter in a data body and replace it with
// the instance argument type
// Takes a string (name of polymorphic temp)
// and returns the substitution 
c_string lookup_paramlist(c_string target, arglist_t replace_params, arglist_t source_params) {

    // XXX should #define MAXSTRLEN 100 at the very least
    c_string found_string = (c_string) malloc(100);

    if_cons_arglist_t(replace_params, r_p_head, r_p_tail)
        if_cons_arglist_t(source_params, s_p_head, s_p_tail)
            if_arg(s_p_head, s_p_head_name)
                if_type_string(s_p_head_name, s_p_head_string)
                
                    if (strcmp(s_p_head_string, target) == 0) {
                        
                        if_arg(r_p_head, r_p_head_name)
                            if_type_string(r_p_head_name, r_p_head_string)
                            
                                strcpy(found_string, r_p_head_string);
                                return found_string; 
                            else()
                                printf("Error: String Type\n;");
                                exit(-1);
                            end_if()
                        else()
		            printf("Error: atomic expression expected\n");
                            exit(-1);
                        end_if()
                        
                    } else {
                        return lookup_paramlist(target, r_p_tail, s_p_tail);
                    }
                else()
                    printf("Error: String Type\n;");
                    exit(-1);
                end_if()
            else()
		printf("Error: atomic expression expected\n");
                exit(-1);
            end_if()
        else()
        
            fprintf(stderr, "\nERROR: Instance poly temp count different from declaration");
            exit(-1);
            
        end_if()   
    else()
        
            found_string = "";
            return found_string;
            
    end_if()
    
}

// Compares to arglist_t.
// Used in mapping a data type to a declaration
int compare_p_list(arglist_t target_list, arglist_t source_list){
    if_cons_arglist_t(target_list, t_head, t_tail)
        if_cons_arglist_t(source_list, s_head, s_tail)
            if_arg(t_head, t_h_name)
                if_arg(s_head, s_h_name)
                    if_type_string(t_h_name, t_h_string)
                        if_type_string(s_h_name, s_h_string)
// printf("Comp %s with %s\n", s_h_string, t_h_string); // YYY
                            if (strcmp(s_h_string, t_h_string) == 0 ) {
                                return compare_p_list(t_tail, s_tail);
                            } else {
                                return 1;
                            }
                        else()
                            printf("Error: String Type\n;");
                            exit(-1);  
                        end_if()
                    else()
                        printf("Error: String Type\n;");
                        exit(-1);
                    end_if()
                else()
		    printf("Error: atomic expression expected\n");
                    exit(-1);
                end_if()
            else()
		printf("Error: atomic expression expected\n");
                exit(-1);
            end_if()
        else()
            return 1;
        end_if()  
    else()
        return 0;
    end_if()
}

// YYY
// Compares to arglist_t.
// Used in mapping a data type to a declaration
int compare_a_list(arglist_t target_list, arglist_t source_list){
// printf("compare_a_list\n"); // YYY
    if_cons_arglist_t(target_list, t_head, t_tail)
        if_cons_arglist_t(source_list, s_head, s_tail)
            switch_arg_t(t_head)
            case_arg(t_h_name)
// printf("compare_a_list arg\n"); // YYY
                if_arg(s_head, s_h_name)
                    if_type_string(t_h_name, t_h_string)
                        if_type_string(s_h_name, s_h_string)
// printf("Cmp %s with %s\n", s_h_string, t_h_string); // YYY
                            if (strcmp(s_h_string, t_h_string) == 0 ) {
                                return compare_a_list(t_tail, s_tail);
                            } else {
                                return 1;
                            }
                        else()
                            printf("Error: String Type\n;");
                            exit(-1);  
                        end_if()
                    else()
                        printf("Error: String Type\n;");
                        exit(-1);
                    end_if()
                else()
                    return 1;
                end_if()
            case_paramdata(t_h_name, t_h_args)
// printf("compare_a_list paramdata\n"); // YYY
                if_paramdata(s_head, s_h_name, s_h_args)
                    if_type_string(t_h_name, t_h_string)
                        if_type_string(s_h_name, s_h_string)
// printf("Cmp %s with %s\n", s_h_string, t_h_string); // YYY
                            if (strcmp(s_h_string, t_h_string) == 0 )
{int cl1, cl2;
                                cl1= compare_a_list(t_h_args, s_h_args);
                                    cl2 = compare_a_list(t_tail, s_tail);
// printf("rec comp %d %d\n", cl1, cl2);
                                return cl1||cl2;
}
/*
                                return compare_a_list(t_h_args, s_h_args)
                                    || compare_a_list(t_tail, s_tail);
*/
                            else
                                return 1;
                        end_if()
                    end_if()
                else()
// if_type_string(t_h_name, t_h_string)
// printf("not a paramdata %s\n", t_h_string); // YYY
// end_if()
                    return 1;
                end_if()
            case_argfunct(t_h_arglist1, t_h_arglist2)
                if_argfunct(s_head, s_h_arglist1, s_h_arglist2)
                    return compare_a_list(t_h_arglist1, s_h_arglist1)
                            || compare_a_list(t_h_arglist2, s_h_arglist2);
                else()
                    return 1;
                end_if()
            end_switch()
            printf("Oops!\n");
	    exit(-1);   // shouldn't happen
        else()
            return 1; // list lengths differ
        end_if()  
    else()
        return 0;
    end_if()
}

// Takes list of insts and expands RHS of each one so RHS has nothing
// defined by insts (just data decls and builtins) - canonical form
// Later we can use this list of expanded insts to expand other things
// more simply, and look for insts with identical expanded forms.
instlist_t expand_all_insts(instlist_t insts, instlist_t all_insts){
    arg_t rhs;
    string_t rhs_str;
    arglist_t rhs_args;

    if_cons_instlist_t(insts, i_head, i_tail)
        // would be so much simpler if rhs of inst was already an arg_t
        // and insts+paraminsts combined!
        if_inst(i_head, i_h_name, i_h_decl, i_h_paramlist)
            if (i_h_paramlist == nil_arglist_t())
                rhs = arg(i_h_decl);
            else
                rhs = paramdata(i_h_decl, i_h_paramlist);
            rhs = expand_with_insts(rhs, all_insts);
            if_arg(rhs, rhs_name)
                rhs_str = rhs_name;
                rhs_args = nil_arglist_t();
            else_if_paramdata(rhs_name, rhs_arglist)
                rhs_str = rhs_name;
                rhs_args = rhs_arglist;
            end_if()
if_type_string(rhs_str, r)
// if_type_string(i_h_name, s)
// // XXX NQR with multiple args - misses commas
// printf("instance %s = %s<%s>;\n", s, r, arg_concat("",rhs_args,1));
// end_if();
end_if();
            return cons_instlist_t(
                    inst(i_h_name, rhs_str, rhs_args),
                    expand_all_insts(i_tail, all_insts));
        else_if_paraminst(i_h_name, i_h_paramlist, i_h_decl, i_h_arglist)
            if (i_h_arglist == nil_arglist_t())
                rhs = arg(i_h_decl);
            else
                rhs = paramdata(i_h_decl, i_h_arglist);
            rhs = expand_with_insts(rhs, all_insts);
            if_arg(rhs, rhs_name)
                rhs_str = rhs_name;
                rhs_args = nil_arglist_t();
            else_if_paramdata(rhs_name, rhs_arglist)
                rhs_str = rhs_name;
                rhs_args = rhs_arglist;
            end_if()
// if_type_string(rhs_str, r)
// if_type_string(i_h_name, s)
// printf("instance %s<%s> = %s<%s>;\n", s,arg_concat("",i_h_paramlist,1), r, arg_concat("",rhs_args,1));
// end_if();
// end_if();
            return cons_instlist_t(
                    paraminst(i_h_name, i_h_paramlist, rhs_str, rhs_args),
                    expand_all_insts(i_tail, all_insts));
        else()
            printf("Error: impossible instance constructor\n;");
            exit(-1);
        end_if()
    else()
        return nil_instlist_t();
    end_if()               
}

// Takes type expression and expands using insts to canonical form where
// everything is defined by data decls or builtin (or undefined) types
// Search instance list; if we find instance matching top level, expand
// and recurse, otherwise (top level is canonical) recurse on each arg.
// Best put have recursion limit to catch recursive inst definitions (no
// base case).
arg_t expand_with_insts(arg_t argx, instlist_t source_i_list){
    return expand_with_insts1(argx, source_i_list, source_i_list, 0);
}

#define MAXERECLEVEL 50
// as above but pass in unsearched + complete source_i_list and
// recursion depth
arg_t expand_with_insts1(arg_t argx, instlist_t i_list, instlist_t source_i_list, int reclevel){
    arg_t new_arg;
    arglist_t new_args;
    if (reclevel > MAXERECLEVEL) {
        printf("Error: recursive/cyclic instance declared\n");
        return arg(type_string("_adt_cyclic_type"));
    }
    if_cons_instlist_t(i_list, i_l_head, i_l_tail) {
        if_arg(argx, argx_name) {
            if_inst(i_l_head, i_l_h_name, i_l_h_rhs, i_l_h_rhs_params) {
                if (eq_string(argx_name, i_l_h_name)) {
                    // We shouldn't have special case for arg!
                    if (i_l_h_rhs_params == nil_arglist_t()) {
                        new_arg = arg(i_l_h_rhs);
// if_type_string(i_l_h_rhs, i_str1)
// if_type_string(i_l_h_name, i_str)
// printf("recursive expand_with_insts1 arg %s -> %s\n", i_str, i_str1);
// end_if()
// end_if()
                    } else {
                        new_args = i_l_h_rhs_params;
                        new_args = map_expand_with_insts(new_args, source_i_list);
                        new_arg = paramdata(i_l_h_rhs, new_args);
                    }
// printf("returns %s\n", arg_concat("",
// cons_arglist_t(expand_with_insts1(new_arg, source_i_list, source_i_list,
// reclevel+1), nil_arglist_t()), 1));
                    return expand_with_insts1(new_arg, source_i_list, source_i_list, reclevel+1);
                }
            } end_if();
            // i_l_head is non-matching inst or paraminst
// printf("continuing expand_with_insts1 arg\n");
            return expand_with_insts1(argx, i_l_tail, source_i_list, reclevel);
        } else_if_paramdata(argx_name, arglistx) {
            if_paraminst(i_l_head, i_l_h_name, i_l_h_params, i_l_h_rhs, i_l_h_rhs_args) {
                if (eq_string(argx_name, i_l_h_name) &&
                        length_arglist(i_l_h_params) ==
                        length_arglist(arglistx)) {
                    // We shouldn't have special case for arg!
                    if (i_l_h_rhs_args == nil_arglist_t())
                        new_arg = arg(i_l_h_rhs);
                    else {
                        new_args = replace_sec_paramlist_a(i_l_h_rhs_args,
                                arglistx, i_l_h_params);
                        // maybe pass in reclevel??
                        new_args = map_expand_with_insts(new_args, source_i_list);
                        new_arg = paramdata(i_l_h_rhs, new_args);
                    }
// if_type_string(i_l_h_name, i_str)
// if_type_string(i_l_h_rhs, i_str1)
// printf("recursive expand_with_insts1 paramdata %s = %s\n", i_str, i_str1);
// end_if()
// end_if()
                    return expand_with_insts1(new_arg, source_i_list, source_i_list, reclevel+1);
                }
            } end_if();
            // i_l_head is non-matching paraminst or inst
// printf("continuing expand_with_insts1 paramdata\n");
            return expand_with_insts1(argx, i_l_tail, source_i_list, reclevel);
        } else_if_argfunct(argx_return, argx_arglist) {
            // XXX  should pass reclevel to map_expand_with_insts
            return argfunct(
                map_expand_with_insts(argx_return, source_i_list),
                map_expand_with_insts(argx_arglist, source_i_list));
        } else() {
            printf("Unexpected paramarg...?\n");
        } end_if()
    } else() {
        // no matching inst found
        // expand each arg with source_i_list
// printf("expand_with_insts1 no match\n");
        if_paramdata(argx, argx_name, arglistx)
// printf("expand_with_insts1 - map_expand_with_insts\n");
            new_args = map_expand_with_insts(arglistx, source_i_list);
            return paramdata(argx_name, new_args);
        // XXX ??? need } else_if_argfunct(argx_return, argx_arglist) {
        else()
            return argx;
        end_if();
    } end_if()
    // shouldn't get here
    return argx;
}

// Expanded arg with insts; assumes insts are already in canonical form
// (RHS expanded)
// Process bottom-up: args first then top level
arg_t expand_with_exp_insts(arg_t argx, instlist_t source_i_list){
    if_arg(argx, argx_name)
        return expand_with_exp_insts1(argx, source_i_list, source_i_list);
    else_if_paramdata(argx_name, arglistx)
        arglistx = map_expand_with_exp_insts(arglistx, source_i_list);
        return expand_with_exp_insts1(paramdata(argx_name, arglistx), source_i_list, source_i_list);
    else_if_argfunct(argx_return, argx_arglist)
        argx_return = map_expand_with_exp_insts(argx_return, source_i_list);
        argx_arglist = map_expand_with_exp_insts(argx_arglist, source_i_list);
        return expand_with_exp_insts1(argfunct(argx_return, argx_arglist), source_i_list, source_i_list);
    else()
        exit(-1);
    end_if()
}

// As above assuming args are expanded
arg_t expand_with_exp_insts1(arg_t argx, instlist_t i_list, instlist_t source_i_list){
    arg_t new_arg;
    arglist_t new_args;
    if_cons_instlist_t(i_list, i_l_head, i_l_tail) {
        if_arg(argx, argx_name) {
            if_inst(i_l_head, i_l_h_name, i_l_h_rhs, i_l_h_rhs_params) {
                if (eq_string(argx_name, i_l_h_name)) {
                    // We shouldn't have special case for arg!
                    if (i_l_h_rhs_params == nil_arglist_t()) {
                        new_arg = arg(i_l_h_rhs);
// if_type_string(i_l_h_rhs, i_str1)
// if_type_string(i_l_h_name, i_str)
// printf("recursive expand_with_insts1 arg %s -> %s\n", i_str, i_str1);
// end_if()
// end_if()
                    } else {
                        new_args = i_l_h_rhs_params;
                        new_arg = paramdata(i_l_h_rhs, new_args);
                    }
                    return new_arg;
                }
            } end_if();
            // i_l_head is non-matching inst or paraminst
            return expand_with_exp_insts1(argx, i_l_tail, source_i_list);
        } else_if_paramdata(argx_name, arglistx) {
            if_paraminst(i_l_head, i_l_h_name, i_l_h_params, i_l_h_rhs, i_l_h_rhs_args) {
                if (eq_string(argx_name, i_l_h_name) &&
                        length_arglist(i_l_h_params) ==
                        length_arglist(arglistx)) {
                    // We shouldn't have special case for arg!
                    if (i_l_h_rhs_args == nil_arglist_t())
                        new_arg = arg(i_l_h_rhs);
                    else {
                        new_args = replace_sec_paramlist_a(i_l_h_rhs_args,
                                arglistx, i_l_h_params);
                        new_arg = paramdata(i_l_h_rhs, new_args);
                    }
                    return new_arg;
                }
            } end_if();
            // i_l_head is non-matching paraminst or inst
            return expand_with_exp_insts1(argx, i_l_tail, source_i_list);
        } else_if_argfunct(argx_return, argx_arglist) {
            return argfunct(argx_return, argx_arglist);
        } else() {
            printf("Dodgey inst\n");
            exit(-1);
        } end_if()
    } else() {
        // no matching inst found
            return argx;
    } end_if()
    // shouldn't get here
    return argx;
}

// map for expand_with_insts
// should use generic map instance (need extra arg since we don't have
// closures)...
arglist_t map_expand_with_insts(arglist_t argxs, instlist_t source_i_list){
    if_cons_arglist_t(argxs, argx, argxs1)
// printf("map_expand_with_insts\n");
        return cons_arglist_t(expand_with_insts(argx, source_i_list),
            map_expand_with_insts(argxs1, source_i_list));
    else()
        return nil_arglist_t();
    end_if();
}

// map for expand_with_exp_insts
// should use generic map instance (need extra arg since we don't have
// closures)...
arglist_t map_expand_with_exp_insts(arglist_t argxs, instlist_t source_i_list){
    if_cons_arglist_t(argxs, argx, argxs1)
// printf("map_expand_with_insts\n");
        return cons_arglist_t(expand_with_exp_insts(argx, source_i_list),
            map_expand_with_exp_insts(argxs1, source_i_list));
    else()
        return nil_arglist_t();
    end_if();
}

// map for find_mono_inst
// should use generic map instance (need extra arg since we don't have
// closures)...
arglist_t map_find_mono_inst(arglist_t argxs, instlist_t source_i_list){
    if_cons_arglist_t(argxs, argx, argxs1)
// printf("map_expand_with_insts\n");
        return cons_arglist_t(find_mono_inst(argx, source_i_list),
            map_find_mono_inst(argxs1, source_i_list));
    else()
        return nil_arglist_t();
    end_if();
}

// returns monomorphic type equivalent to argx
arg_t find_mono_inst(arg_t argx, instlist_t source_i_list){
    // XXX best expand argx to canonical form here?
    argx = expand_with_exp_insts(argx, source_i_list);
// printf("---\n");
// source_i_list = expand_all_insts(source_i_list, source_i_list);
// printf("---\n");
    if_arg(argx, arg_name)
        if_type_string(arg_name, arg_string)
// printf("find_mono_inst argx = %s\n", arg_string);
            return find_instance_a1(arg_string, nil_arglist_t(), source_i_list, source_i_list);
        else()
            exit(1); // impossible
        end_if()
    else_if_paramdata(arg_name, arg_args)
        if_type_string(arg_name, arg_string)
// printf("find_mono_inst argx = %s<%s>\n", arg_string, arg_concat("",arg_args,1));
            return find_instance_a1(arg_string, arg_args, source_i_list, source_i_list);
        else()
            exit(1); // impossible
        end_if()
    else_if_argfunct(h_r_list, h_p_list)
        h_r_list = map_find_mono_inst(h_r_list, source_i_list);
        h_p_list = map_find_mono_inst(h_p_list, source_i_list);
        return argfunct(h_r_list, h_p_list);
    else()
        printf("Error: find_mono_inst failure\n");
        return argx;
    end_if()
}

/*
// Takes a data name and parameter list and maps it to an instance
// Used for polymorphic types within the data body
// YYY allows target_list to have have type expressions, not just
// params.  Matching must be recursive to handle that.
// in only call, new_arg_name = find_instance(f_a_h_string,
// new_param_list... cvt new_param_list to arglist
// YYY We repeatedly look for inst with matching RHS and return LHS
// until LHS returned is monomorphic (best put a limit on number of
// iterations to prevent possible infinite loop for instances which are
// recursive, which is an error)
// YYY need to recursively simplify args first, so we do it BUP -
// currently find_instance_a1 is top-down, which breaks?
// toplevsimp = string of simp
// simp = iter simp1
// simp1 = simp args; search instance_list and return match or error
c_string find_instance_a(c_string target_name, arglist_t target_list, instlist_t instance_list){
    arg_t new_arg, e_arg;
 
// printf("find_instance_a \n");
    new_arg = find_instance_a1(target_name, target_list, instance_list, instance_list);
    if_arg(new_arg, arg_name)
        if_type_string(arg_name, arg_string)
            return arg_string;
        end_if()
    else_if_paramdata(arg_name, arg_args)
        if_type_string(arg_name, arg_string)
            if (arg_args == nil_arglist_t())  // safe to compare constants
                return arg_string;
        end_if()
    else()
        printf("Error: find_instance_a failure\n");
    end_if()
    return "YYY";
}
*/

// Takes a data name and parameter list in expanded form and maps it to
// the first matching monomorphic instance (or returns same as input if
// there is no match and input has empty arg list, otherwise error)
arg_t find_instance_a1(c_string target_name, arglist_t target_list, instlist_t instance_list, instlist_t all_insts){
    arg_t e_arg;
    arglist_t e_i_h_arglist;
    string_t e_i_h_name;

// printf("find_instance_a1 %s<%s>\n", target_name, arg_concat("", target_list, 1));
    if_cons_instlist_t(instance_list, i_head, i_tail) {
        if_inst(i_head, i_h_name, i_h_decl, i_h_paramlist) {
// if_type_string(i_h_name, i_str)
// printf("expanding rhs of inst inside find_instance_a1 %s\n", i_str);
// end_if()
            if_type_string(i_h_decl, i_h_decl_string) {
            
// printf("Comparin with %s<%s>\n", i_h_decl_string, arg_concat("", e_i_h_arglist, 1)); // YYY
                if (strcmp(target_name, i_h_decl_string) != 0) {
                    return find_instance_a1(target_name, target_list, i_tail, all_insts);
                } else if (compare_a_list(target_list, i_h_paramlist) != 0){
                    return find_instance_a1(target_name, target_list, i_tail, all_insts);
                } else {
// if_type_string(i_h_name, i_str)
// printf("returning arg %s from find_instance_a1 %s\n", i_str, target_name);
// end_if()
                    return arg(i_h_name);
                }
            } else()
                printf("Error: String Type\n;");
                exit(-1);    
            end_if()
        // else()
        } else_if_paraminst(i_h_name, i_h_paramlist, i_h_decl, i_h_arglist) {
            // this instance is polymorphic - ignore YYY
            return find_instance_a1(target_name, target_list, i_tail, all_insts);
        } else()
            exit(2); // impossible??
        end_if()
//YYY
    } else()
        if (target_list == nil_arglist_t())
            return arg(type_string(target_name));
        else {
            c_string arg_list = arg_concat("", target_list, 1);
            fprintf(stderr, "\nERROR: Must instantiate data type: %s<%s>\n", target_name, arg_list);
            exit(-1);
        }
    end_if()
}


// Replaces parameters in a data declaration with those from an instance 
// YYY replace param by arg rather than param
// Deposits result in s_r_p_list
// Must be cleared after use
arglist_t replace_sec_paramlist_a(arglist_t current_list, arglist_t
replace_list, arglist_t source_list){
    arg_t new_arg;
    c_string new_name = (c_string)malloc(100);
    
    if_cons_arglist_t(current_list, c_head, c_tail)
// printf("replace_sec_paramlist_a\n"); // YYY
        if_arg(c_head, c_h_name)
            if_type_string(c_h_name, c_h_string)
            
                new_arg = lookup_paramlist_a(c_h_string, replace_list, source_list); 
// printf("replace_sec_paramlist arg - replacing\n"); // YYY
                return cons_arglist_t(new_arg, replace_sec_paramlist_a(c_tail, replace_list, source_list));
            else()
                printf("Error: String Type\n");
                exit(-1);
            end_if()
        else_if_paramdata(c_h_name, c_h_args)
            arglist_t new_args = replace_sec_paramlist_a(c_h_args, replace_list, source_list);
            return cons_arglist_t(paramdata(c_h_name, new_args),
                    replace_sec_paramlist_a(c_tail, replace_list, source_list));
	else_if_argfunct(h_r_list, h_p_list)
            h_r_list = replace_sec_paramlist_a(h_r_list, replace_list, source_list);
            h_p_list = replace_sec_paramlist_a(h_p_list, replace_list, source_list);
            return cons_arglist_t(argfunct(h_r_list, h_p_list),
                    replace_sec_paramlist_a(c_tail, replace_list, source_list));
        else()
            exit(-1);
        end_if()
    else()
        return nil_arglist_t();
    end_if()
}


// Replaces parameters in a data declaration with those from an instance 
// Deposits result in s_r_p_list
// Must be cleared after use
arglist_t replace_sec_paramlist(arglist_t current_list, arglist_t
replace_list, arglist_t source_list){
    arg_t new_param;
    c_string new_name = (c_string)malloc(100);
    
// printf("replace_sec_paramlist\n"); // YYY
    if_cons_arglist_t(current_list, c_head, c_tail)
        if_arg(c_head, c_h_name)
            if_type_string(c_h_name, c_h_string)
            
                new_name = lookup_paramlist(c_h_string, replace_list, source_list); 
                new_param = arg(type_string(new_name));
// printf("replace_sec_paramlist %s %s\n", c_h_string, new_name); // YYY
                
                return cons_arglist_t(new_param, replace_sec_paramlist(c_tail, replace_list, source_list));
            else()
                printf("Error: String Type\n");
                exit(-1);
            end_if()
        else() // YYY
            printf("Error: Param name expected\n");
            exit(-1);
        end_if()
    else()
        return nil_arglist_t();
    end_if()
}

// Clones the args of the ctr to the global arg list
// YYY replace param by arg rather than param
// a_list must be cleared after use
arglist_t clone_arg_a(arglist_t found_arglist, arglist_t
instance_arglist, arglist_t found_paramlist, instlist_t source_i_list){
    arg_t new_arg;
    arglist_t fn_arg_list;
    arglist_t fn_return;
    arglist_t new_param_list;
    c_string new_arg_name = (c_string) malloc(100);
    // c_string param_name = (c_string) malloc(100);
                
    if_cons_arglist_t(found_arglist, f_a_head, f_a_tail)
        if_arg(f_a_head, f_a_h_name)
            // treat in same way as param
            if_type_string(f_a_h_name, f_a_h_string)
                new_arg = lookup_paramlist_a(f_a_h_string, instance_arglist, found_paramlist); 
            end_if()
//             if_type_string(f_a_h_name, f_a_h_string)
// 
//                 strcpy(new_arg_name, f_a_h_string);
//                 new_arg = arg(type_string(new_arg_name));
// 
//             end_if()
        else_if_paramdata(f_a_h_name, found_sec_paramlist)
	    new_arg = paramdata(f_a_h_name,
		replace_sec_paramlist_a(found_sec_paramlist,
			instance_arglist, found_paramlist));
        else_if_argfunct(f_a_h_return, f_a_h_arglist)
            
            fn_return = reverse_arglist(clone_arg_a(f_a_h_return, instance_arglist, found_paramlist, source_i_list));
            fn_arg_list = reverse_arglist(clone_arg_a(f_a_h_arglist, instance_arglist, found_paramlist, source_i_list));
            new_arg = argfunct(fn_return, fn_arg_list);
            
        end_if()

        
        return cons_arglist_t(new_arg, clone_arg_a(f_a_tail,
instance_arglist, found_paramlist, source_i_list));
        
    else()
        return nil_arglist_t();
    end_if()
         
}


// Clones the args of the ctr to the global arg list
// With replacement of the parameters with instance parameters
// a_list must be cleared after use
arglist_t clone_arg(arglist_t found_arglist, arglist_t
instance_paramlist, arglist_t found_paramlist, instlist_t source_i_list){
// return clone_arg_a(found_arglist, instance_paramlist, found_paramlist, source_i_list);
    // replace params by their values
// printf("clone_arg, calling replace_sec_paramlist_a\n");
    found_arglist = replace_sec_paramlist_a(found_arglist, instance_paramlist, found_paramlist);
    // expand args to canonical form
// printf("clone_arg, calling map_expand_with_insts\n");
    found_arglist = map_expand_with_exp_insts(found_arglist, source_i_list);
    // expand args to canonical form
// printf("clone_arg, calling map_find_mono_inst\n");
    return map_find_mono_inst(found_arglist, source_i_list);
}

// Clones the ctr of the decl to the global ctr list
// c_list must be cleared after use
ctrlist_t clone_ctr(int newcons, c_string decl_name, c_string
        old_decl_name, ctrlist_t found_ctrlist, arglist_t instance_paramlist,
        arglist_t found_paramlist, instlist_t source_i_list){
    
    ctr_t new_ctr;
    c_string new_ctr_name;
    arglist_t new_arg_list;
    
    if_cons_ctrlist_t(found_ctrlist, f_c_head, f_c_tail)
        if_ctr(f_c_head, f_c_h_name, f_c_h_arg)
            if_type_string(f_c_h_name, f_c_h_string)
// printf("clone_ctr %s\n", f_c_h_string);
                // use newcons flag to determine if we need to add postfix
                if (newcons)
                    new_ctr_name = concat(f_c_h_string, concat("_", decl_name));
                else
                    new_ctr_name = f_c_h_string;

                new_arg_list = clone_arg(f_c_h_arg, instance_paramlist, found_paramlist, source_i_list);
                new_ctr = ctr(type_string(new_ctr_name), new_arg_list);
                return cons_ctrlist_t(new_ctr, clone_ctr(newcons, decl_name, old_decl_name, f_c_tail, instance_paramlist, found_paramlist, source_i_list));
             else()
                printf("Error: String Type\n;");
                exit(-1);   
            end_if()
        end_if()
    else()
        return nil_ctrlist_t();
    end_if()
}


// Clones the ctr of the decl to the global ctr list
// YYY replace param by arg rather than param
// c_list must be cleared after use???
ctrlist_t clone_ctr_a(c_string decl_name, c_string old_decl_name,
ctrlist_t found_ctrlist, arglist_t instance_arglist, arglist_t found_paramlist, instlist_t source_i_list){
    
    ctr_t new_ctr;
    c_string new_ctr_name;
    arglist_t new_arg_list;
    
    if_cons_ctrlist_t(found_ctrlist, f_c_head, f_c_tail)
        if_ctr(f_c_head, f_c_h_name, f_c_h_arg)
            if_type_string(f_c_h_name, f_c_h_string)
// printf("clone_ctr_a %s\n", f_c_h_string);
                // no postfix needed for polymorphic case here
                // XXX see clone_ctr if we generalise things
                new_ctr_name = f_c_h_string;
                new_arg_list = clone_arg_a(f_c_h_arg, instance_arglist, found_paramlist, source_i_list);
                new_ctr = ctr(type_string(new_ctr_name), new_arg_list);
                return cons_ctrlist_t(new_ctr, clone_ctr_a(decl_name, old_decl_name, f_c_tail, instance_arglist, found_paramlist, source_i_list));
             else()
                printf("Error: String Type\n;");
                exit(-1);   
            end_if()
        end_if()
    else()
        return nil_ctrlist_t();
    end_if()
}

// Takes each polymorphic instance of a polymorphic decl and instantiates
// to a polymorphic decl
decllist_t param_instance_to_decl(instlist_t instance_list, decllist_t source_d_list, instlist_t source_i_list){
    
    decl_t new_decl;
    decl_t found_decl;
    ctrlist_t new_ctr_list;
    c_string new_decl_name = (c_string) malloc(100);
    
    if_cons_instlist_t(instance_list, i_head, i_tail)
        if_inst(i_head, i_h_name, i_h_decl, i_h_paramlist)
            // monomorphic instance - ignore
	    return param_instance_to_decl(i_tail, source_d_list, source_i_list);
        else_if_paraminst(i_h_name, i_h_paramlist, i_h_decl, i_h_arglist)
            WARNING_AVOID(i_h_arglist);
            if_type_string(i_h_name,i_h_string)
// printf("processing %s, ", i_h_string);
                if_type_string(i_h_decl, old_decl_name)
                    strcpy(new_decl_name, i_h_string);
                    found_decl =  decl_lookup(old_decl_name, source_d_list);
    
                    if_paramdecl( found_decl, f_d_name, f_d_ctrlist, f_d_paramlist)
                        WARNING_AVOID(f_d_name);
// printf("clone_ctr_a \n");
                        new_ctr_list = clone_ctr_a(new_decl_name, old_decl_name, f_d_ctrlist, i_h_arglist, f_d_paramlist, source_i_list);
                	// printf("New paramdecl %s\n", new_decl_name);
                        new_decl = paramdecl(type_string(new_decl_name), new_ctr_list, (i_h_paramlist));
                        
                        return cons_decllist_t(new_decl, param_instance_to_decl(i_tail, source_d_list, source_i_list));
                   
                    else_if_decl(f_d_name, f_d_ctrlist)
                        new_ctr_list = clone_ctr_a(new_decl_name,
old_decl_name, f_d_ctrlist, nil_arglist_t(), nil_arglist_t(), source_i_list);
                        new_decl = decl(type_string(new_decl_name), new_ctr_list);
                        return cons_decllist_t(new_decl, param_instance_to_decl(i_tail, source_d_list, source_i_list));
                    else()
                        printf("Error: Decl Type\n;");
                        exit(-1);
                    end_if()
                else()
                    printf("Error: String Type\n;");
                    exit(-1);
                end_if()
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
        else()
            printf("Error: impossible instance constructor\n;");
            exit(-1);
        end_if()
    else()
        return nil_decllist_t();
    end_if()               
}


// Takes each monomorphic instance of a polymorphic decl and instantiates
// to a monomorphic decl;  First arg is list of generic insts which have
// been added from paramdecls - these don't need new constructor names,
// others do
decllist_t instance_to_decl(instlist_t i_list_gen, instlist_t instance_list, decllist_t source_d_list, instlist_t source_i_list){
    
    int newcons;
    decl_t new_decl;
    decl_t found_decl;
    ctrlist_t new_ctr_list;
    c_string new_decl_name = (c_string) malloc(100);
    
    if_cons_instlist_t(instance_list, i_head, i_tail)
        if_inst(i_head, i_h_name, i_h_decl, i_h_paramlist)
            if_type_string(i_h_name,i_h_string)
// printf("Processing %s\n", i_h_string);
                if_type_string(i_h_decl, old_decl_name)

                    strcpy(new_decl_name, i_h_string);
                    found_decl =  decl_lookup(old_decl_name, source_d_list);
    
                    if_paramdecl( found_decl, f_d_name, f_d_ctrlist, f_d_paramlist)
                        WARNING_AVOID(f_d_name);
                        newcons = !inst_member(i_h_string, i_list_gen);
                        new_ctr_list = clone_ctr(newcons, new_decl_name,
old_decl_name, f_d_ctrlist, i_h_paramlist, f_d_paramlist, source_i_list);
// printf("New decl %s\n", new_decl_name);
                        new_decl = decl(type_string(new_decl_name), new_ctr_list);
                        
                        return cons_decllist_t(new_decl, instance_to_decl(i_list_gen, i_tail, source_d_list, source_i_list));
                   
                    else_if_decl(f_d_name, f_d_ctrlist)
                        new_ctr_list = clone_ctr_a(new_decl_name, old_decl_name, f_d_ctrlist, nil_arglist_t(), nil_arglist_t(), source_i_list);
                        new_decl = decl(type_string(new_decl_name), new_ctr_list);
                        return cons_decllist_t(new_decl, instance_to_decl(i_list_gen, i_tail, source_d_list, source_i_list));
                    else()
                        printf("Error: Decl Type\n;");
                        exit(-1);
                    end_if()
                else()
                    printf("Error: String Type\n;");
                    exit(-1);
                end_if()
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
        // YYY
        else_if_paraminst(i_h_name, i_h_paramlist, i_h_decl, i_h_arglist)
            // polymorphic instance - ignore
	    return instance_to_decl(i_list_gen, i_tail, source_d_list, source_i_list);
        else()
            printf("Error: impossible instance constructor\n;");
            exit(-1);
        end_if()
    else()
        return nil_decllist_t();
    end_if()               
}

// check if string is a defined inst in list
int inst_member(c_string target, instlist_t instance_list) {
    if_cons_instlist_t(instance_list, i_head, i_tail)
        if_inst(i_head, i_h_name, i_h_decl, i_h_paramlist)
            if_type_string(i_h_name,i_h_string)
                if (strcmp(i_h_string, target) == 0)
                    return 1;
                else
                    return inst_member(target, i_tail);
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
        else()
            // polymorphic instance - ignore
	    return inst_member(target, i_tail);
        end_if()
    else()
        return 0;
    end_if()               
}

// Constructs a arglist_t of ADT types for replacing generic instances
arglist_t construct_ADT_List(arglist_t paramlist, int num){
    
    if (num > 0) {
        c_string name = (c_string)malloc(100);
        c_string number = (c_string)malloc(100);
        
        sprintf(number, "%i", num);
        strcpy(name, "adt_");
        strcat(name, number);
        paramlist = cons_arglist_t(arg(type_string(name)), paramlist);
        
        return construct_ADT_List(paramlist, num - 1);
        
    } else {
    
        return paramlist;
    
    }
}

// Constructs a generic monomorph instances with ADT parameters 
// from decls and places it in the instance list
instlist_t put_gen_instance(decllist_t decl_list){

    int d_h_p_length;
    arglist_t new_ADT_paramlist;
    inst_t new_instance;
    
    if_cons_decllist_t(decl_list, d_head, d_tail)
        if_paramdecl(d_head, d_h_name, d_h_ctrlist, d_h_paramlist)
            WARNING_AVOID(d_h_ctrlist);
            d_h_p_length = length_arglist(d_h_paramlist);
// printf("put_gen_instance %s\n", *(char**)d_h_name); // YYY
            new_ADT_paramlist = construct_ADT_List(nil_arglist_t(), d_h_p_length);
            new_instance = inst(d_h_name, d_h_name, new_ADT_paramlist);

            return cons_instlist_t(new_instance, put_gen_instance(d_tail));
        else()
            return put_gen_instance(d_tail);
        end_if()
    else()
        return nil_instlist_t();
    end_if()
}

// As above but input is instlist_t, not decllist_t
instlist_t map_to_gen_instance(instlist_t i_list) {
    int i_h_p_length;
    arglist_t new_ADT_paramlist;
    inst_t new_instance;
    
    if_cons_instlist_t(i_list, i_head, i_tail)
        if_paraminst(i_head, // d_h_name, d_h_ctrlist, d_h_paramlist)
i_h_name, i_h_paramlist, rhs_str, rhs_args)
            i_h_p_length = length_arglist(i_h_paramlist);
// printf("put_gen_instance %s\n", *(char**)i_h_name); // YYY
            new_ADT_paramlist = construct_ADT_List(nil_arglist_t(), i_h_p_length);
            new_instance = inst(i_h_name, i_h_name, new_ADT_paramlist);

            return cons_instlist_t(new_instance, map_to_gen_instance(i_tail));
        else()
            return map_to_gen_instance(i_tail);
        end_if()
    else()
        return nil_instlist_t();
    end_if()
}

// Removes the parametric decls after they have been
// instantiated
decllist_t remove_paramdecls(decllist_t decl_list){
    if_cons_decllist_t(decl_list, d_head, d_tail)
        if_decl(d_head, d_h_name, d_h_ctrlist)
            WARNING_AVOID(d_h_ctrlist && d_h_name);
            return cons_decllist_t(d_head, remove_paramdecls(d_tail));
        else()
            return remove_paramdecls(d_tail);
        end_if()
    else()
        return nil_decllist_t();
    end_if()
}

// Prints args visually for inspection
void print_out_args(arglist_t arg_list){
    if_cons_arglist_t(arg_list, a_head, a_tail)
        if_arg(a_head, a_h_name)
            if_type_string(a_h_name, a_h_string)
                printf("%s\n", a_h_string);
            end_if()
        else_if_paramdata(a_h_name, ptl)
            WARNING_AVOID(ptl);
            if_type_string(a_h_name, a_h_string)
                // c_string hello = param_concat("",ptl,1); // YYY
                c_string hello = "";
                printf("%s: %s\n", a_h_string, hello);
            end_if()
/*         else_if_argfunct(return_n, arg_list)
            printf("I am an arg\n"); */
        end_if()
        print_out_args(a_tail);
    else()
        printf("empty\n");
    end_if()
}


// Prints an string to file
void print_to_file(c_string content, c_string h_file_name){

    FILE *fp_h =fopen(h_file_name, "a+");
    fputs(content, fp_h);
    fclose(fp_h);

}

// Concatenates arglist_t for use in debugging
c_string param_concat(c_string acum, arglist_t param_list, int first){
    if_cons_arglist_t(param_list, p_head, p_tail)
        if_arg(p_head, p_name)
            if_type_string(p_name, p_string)
                if (first == 1){
                    first = 0;
                    return param_concat(concat(acum, p_string), p_tail, first);
                }else{
                    return param_concat(concat(acum, concat(", ", p_string)), p_tail, first);
                }
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
        else()
            printf("Error: param_concat\n");
            exit(-1);
        end_if()
    else()
        return acum;
    end_if()
}

// Concatenates an arglist_t for printing out function prototypes
c_string arg_concat(c_string acum, arglist_t arg_list, int first){
    if_cons_arglist_t(arg_list, a_head, a_tail)
        if_arg(a_head, a_name)
            if_type_string(a_name, a_string)
                if (first == 1){
                    first = 0;
                    return arg_concat(concat(acum, a_string), a_tail, first);
                }else{
                    return arg_concat(concat(acum, concat(", ", a_string)), a_tail, first);
                }
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
        else_if_argfunct(h_r_list, h_p_list)
            WARNING_AVOID(h_r_list && h_p_list);
            c_string return_decl_type_start = function_pointer_start(a_head);
            c_string return_decl_type_end = function_pointer_end(a_head);
            
            if (first == 1){
                first = 0;
                return arg_concat(concat(acum, concat(return_decl_type_start, return_decl_type_end)), a_tail, first);
            }else{
                return arg_concat(concat(acum, concat(", ", concat(return_decl_type_start, return_decl_type_end))), a_tail, first);
            }
        else_if_paramdata(arg_name, arg_args)
            // XXX only used for error messages - should check first == 1
            // etc as above and concat ","
            if_type_string(arg_name, arg_string)
                return arg_concat(concat(arg_concat(concat(acum, concat(arg_string, "<")),
                      arg_args, 1), ">"), a_tail, 0);
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
        else()
            printf("Error: Arg must be arg or argfunct to be printed\n");
            exit(-1);
        end_if()
    else()
        return acum;
    end_if()
}

// Concatenates an arglist_t with new variable names for printing out function aliases
c_string arg_concat_with_var(c_string acum, arglist_t arg_list, int first, int num){
    c_string name = (c_string)malloc(100);
    c_string number = (c_string)malloc(100);
    
    sprintf(number, "%i", num);
    strcpy(name, "v");
    strcat(name, number);
    
    if_cons_arglist_t(arg_list, a_head, a_tail)
        if_arg(a_head, a_name)
            if_type_string(a_name, a_string)

                if (first == 1){
                    first = 0;
                    return arg_concat_with_var(concat(acum, concat(a_string, concat(" ", name))), a_tail, first, ++num);
                }else{
                    return arg_concat_with_var(concat(acum, concat(", ", concat(a_string, concat(" ", name)))), a_tail, first, ++num);
                }
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
        else_if_argfunct(h_r_list, h_p_list)
            WARNING_AVOID(h_r_list && h_p_list);
            c_string return_decl_type_start = function_pointer_start(a_head);
            c_string return_decl_type_end = function_pointer_end(a_head);
            
            if (first == 1){
                first = 0;
                return arg_concat_with_var(concat(acum, concat(return_decl_type_start,concat(name, return_decl_type_end))), a_tail, first, ++num);
            }else{
                return arg_concat_with_var(concat(acum, concat(", ", concat(return_decl_type_start, concat(name, return_decl_type_end)))), a_tail, first, ++num);
            }
            
        else()
            printf("Error: arg_concat_with_var\n");
            exit(-1);
        end_if()
    else()
        return acum;
    end_if()
}

// Prints out casts and variable names for use in function aliases
c_string arg_concat_with_cast(c_string acum, arglist_t arg_list, int first, int num){
    c_string name;
    c_string number = (c_string)malloc(100);
    
    sprintf(number, "%i", num);
    name = concat("v", number);

    if_cons_arglist_t(arg_list, a_head, a_tail)
        if_arg(a_head, a_name)
            if_type_string(a_name, a_string)

                name = concat("(", concat(a_string, concat(") ", name)));
                
                if (first == 1){
                    first = 0;
                    return arg_concat_with_cast(concat(acum, name), a_tail, first, ++num);
                }else{
                    return arg_concat_with_cast(concat(acum, concat(", ", name)), a_tail, first, ++num);
                }
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
        else_if_argfunct(h_r_list, h_p_list)
            WARNING_AVOID(h_r_list && h_p_list);
            c_string return_decl_type_start = function_pointer_start(a_head);
            c_string return_decl_type_end = function_pointer_end(a_head);
            name = concat("(", concat(return_decl_type_start, concat(return_decl_type_end, concat(") ", name))));
            
            if (first == 1){
                first = 0;
                return arg_concat_with_cast(concat(acum, name), a_tail, first, ++num);
            }else{
                return arg_concat_with_cast(concat(acum, concat(", ", name)), a_tail, first, ++num);
            }
            

        else()
            printf("Error: arg_concat_with_cast\n");
            exit(-1);
        end_if()
    else()
        return acum;
    end_if()
}

// Prints a function prototype to file
void print_function_prototype(fn_t func, instlist_t source_i_list, c_string g_file_name){

    int length_list;
    arglist_t new_arg_list;
    arglist_t return_arg_list;
    
    if_fn(func, f_name, f_return, f_arglist, f_paramlist)
        c_string content;
        c_string arg_string_list;
        c_string return_string_list;
        
        length_list = length_arglist(f_paramlist);
        arglist_t new_ADT_paramlist = construct_ADT_List(nil_arglist_t(), length_list);
        
        // XXX should delete reverse_arglist here and in parser.y
        // after "function_definition:" ???
        new_arg_list = reverse_arglist(clone_arg(f_arglist, new_ADT_paramlist, f_paramlist, source_i_list));
        arg_string_list = arg_concat("", new_arg_list, 1);
        
        return_arg_list = clone_arg(f_return, new_ADT_paramlist, f_paramlist, source_i_list);
        
        if_type_string(f_name, f_string)

            arg_t head_arg = head_arg_t(return_arg_list);
            
            if_arg(head_arg, head_name)
                WARNING_AVOID(head_name);
                return_string_list = arg_concat("", return_arg_list, 1);
                content = concat(return_string_list, concat(" ", concat(f_string, concat("(", concat(arg_string_list, ");\n")))));
            else_if_argfunct(head_return_list, head_arg_list)
                WARNING_AVOID(head_return_list && head_arg_list);
                c_string func_start = function_pointer_start(head_arg);
                c_string func_end = function_pointer_end(head_arg);
                
                content = concat(func_start, concat(f_string, concat("(", concat(arg_string_list, concat(")", concat(func_end, ";\n"))))));
            else()
                printf("Error: print_function_prototype\n");
                exit(-1);
            end_if()

            print_to_file(content, g_file_name);
        end_if() 
    end_if()
                            
}

// Prints a function prototype list to file
void print_functionlist_prototypes(fnlist_t fn_list, instlist_t source_i_list, c_string g_file_name){    
    if_cons_fnlist_t(fn_list, f_head, f_tail)
        print_function_prototype(f_head, source_i_list, g_file_name);
        print_functionlist_prototypes(f_tail, source_i_list, g_file_name);
    end_if()
}

// Initiates function prototype printing
void print_function_prototype_section(fnlist_t fn_list, instlist_t source_i_list, c_string g_file_name){

    c_string content = "\n/* Function Prototypes */\n/*----------------------------------------------------------------------------*/\n\n";
    print_to_file(content, g_file_name);
    
    print_functionlist_prototypes(fn_list, source_i_list, g_file_name);
    
}

// Takes a function name and function list and returns a
// function with that name
fn_t function_lookup(c_string target, fnlist_t source_list){
    fn_t new_function;
    
    if_cons_fnlist_t(source_list, source_head, source_tail)
        if_fn(source_head, s_head_name, s_head_return, s_head_arg_list, s_head_param_list)
            if_type_string(s_head_name, s_head_string)

                if (strcmp(s_head_string, target)==0) {
                    new_function = source_head;
                    return new_function;
                } else {
                    return function_lookup(target, source_tail);
                }
            else()
                printf("Error: String Type\n;");
                exit(-1);
            end_if()
            WARNING_AVOID(s_head_param_list && s_head_arg_list && s_head_return);
        else()
            exit(-1);
        end_if()
    else()
        fprintf(stderr, "\nERROR: No function of name %s\n",  target);
        exit(-1);
    end_if()

}

arglist_t argfunct_to_arg(arglist_t a_list){
    if_cons_arglist_t(a_list, a_head, a_tail)
        if_argfunct(a_head, return_list, p_list)
            WARNING_AVOID(return_list && p_list);
            c_string funct_data_name;
            funct_data_name = arg_concat("", cons_arglist_t(a_head, nil_arglist_t()), 1);
            return cons_arglist_t(arg(type_string(funct_data_name)), argfunct_to_arg(a_tail));
        else()
            return cons_arglist_t(a_head, argfunct_to_arg(a_tail));
        end_if()
    else()
        return nil_arglist_t();
    end_if()
}

c_string function_pointer_start(arg_t return_a){
    if_arg(return_a, r_a_name)
        if_type_string(r_a_name, r_a_string)
            return concat(r_a_string, " ");
        else()
            printf("Error: String Type\n;");
            exit(-1);
        end_if()
    else_if_argfunct(inner_return_list, inner_arg_list)
        WARNING_AVOID(inner_arg_list);
        return concat(function_pointer_start(head_arg_t(inner_return_list)), "(*");
    else()
        printf("Error: Only argfuncts and args allowed in function_pointer_start\n");
        exit(-1);
    end_if()
}

c_string function_pointer_end(arg_t return_a){
    if_arg(return_a, r_a_name)
        WARNING_AVOID(r_a_name);
        return "";
    else_if_argfunct(inner_return_list, inner_arg_list)
        return concat(")(", concat( arg_concat("", inner_arg_list, 1), concat( ")", function_pointer_end(head_arg_t(inner_return_list)))));
    else()
        printf("Error: Only argfuncts and args allowed in function_pointer_end\n");
        exit(-1);
    end_if()
}

// Prints a function alias to file
void print_function_alias(inst_t finstance, instlist_t source_i_list, fnlist_t source_f_list, c_string g_file_name){
    c_string function_declaration;
    c_string inline_string = "static __inline ";
    c_string param_string;
    c_string return_type_string;
    c_string function_call;
    c_string function_definition;
    c_string return_decl_type;
    fn_t found_function;
    arglist_t new_arg_list;
    arglist_t return_arg_list;
    arglist_t func_arg_list;
    arg_t head_arg;
    
    if_inst(finstance, f_i_name, f_lookup_name, f_lookup_paramlist)
        if_type_string(f_lookup_name, f_lookup_string)
            found_function = function_lookup(f_lookup_string, source_f_list);
            if_fn(found_function, f_name, f_return, f_arglist, f_paramlist)
                if_type_string(f_name, f_string)
                    if_type_string(f_i_name, function_name_string)
                    
// printf("clone_arg(f_arglist,...\n");
                        new_arg_list = reverse_arglist(clone_arg(f_arglist, f_lookup_paramlist, f_paramlist, source_i_list));
                        param_string = arg_concat_with_var("", new_arg_list, 1, 0);
                        
// printf("clone_arg(f_return,...\n");
                        return_arg_list = clone_arg(f_return, f_lookup_paramlist, f_paramlist, source_i_list);
                        head_arg = head_arg_t(return_arg_list);
                                           
                        if_arg(head_arg, head_name)
                            WARNING_AVOID(head_name);
                            return_decl_type = arg_concat("", return_arg_list, 1);
                            function_declaration = concat(inline_string, concat(return_decl_type, concat(" ", concat(function_name_string,
                                concat("(", concat(param_string, "){\n"))))));
                        else_if_argfunct(h_r_list, h_p_list)
                            WARNING_AVOID(h_r_list && h_p_list);
                            c_string return_decl_type_start = function_pointer_start(head_arg);
                            c_string return_decl_type_end = function_pointer_end(head_arg);
                            return_decl_type = concat(return_decl_type_start, return_decl_type_end);
                            
                            function_declaration = concat(inline_string, concat(return_decl_type_start, concat("", concat(function_name_string,
                                concat("(", concat(param_string, concat(")", concat( return_decl_type_end,"{\n"))))))));
                        else()
                            printf("Error: print_function_alias\n");
                            exit(-1);
                        end_if()

                        print_to_file(function_declaration, g_file_name);
                        
                        if (strcmp(return_decl_type, "void") == 0){
                            return_type_string = "    ";
                        } else {
                            return_type_string = concat("    return (", concat(return_decl_type, ") "));
                        }
                        
                        int length_list = length_arglist(f_paramlist);
                        arglist_t new_ADT_paramlist =
construct_ADT_List(nil_arglist_t(), length_list);
                        
                        func_arg_list = reverse_arglist(clone_arg(f_arglist, new_ADT_paramlist, f_paramlist, source_i_list));
                        
                        function_call = concat(f_string, concat("(", concat(arg_concat_with_cast("", func_arg_list, 1, 0), ");\n}\n\n")));
                        function_definition = concat(return_type_string, function_call);
                        
                        print_to_file(function_definition, g_file_name);

                    end_if()
                end_if()
            end_if()
        end_if()
    end_if()
}

// Prints a list of function aliases to file
void print_function_aliases(instlist_t finst_list, instlist_t source_i_list, fnlist_t source_f_list, c_string g_file_name){
    if_cons_instlist_t(finst_list, f_head, f_tail)
        print_function_alias(f_head, source_i_list, source_f_list, g_file_name);
        print_function_aliases(f_tail, source_i_list, source_f_list, g_file_name);
    end_if()            
}

// Initiates function alias printing
void print_function_alias_section(instlist_t finst_list, instlist_t source_i_list, fnlist_t source_f_list, c_string g_file_name){
    c_string content = "\n/* Function Aliases */\n/*----------------------------------------------------------------------------*/\n\n";
    print_to_file(content, g_file_name);
    
    print_function_aliases(finst_list, source_i_list, source_f_list, g_file_name);
    
    content = "/*----------------------------------------------------------------------------*/\n";
    print_to_file(content, g_file_name);

}

// Constructs a string of ADT type declarations
c_string construct_ADT_types_string(int num){
    
    if (num > 0) {
        c_string name;
        c_string number = (c_string)malloc(100);
        
        sprintf(number, "%i", num);
        name = concat("typedef struct _ADT_Gen_", concat(number, concat("{} *adt_", concat(number, ";\n"))));
        return concat(construct_ADT_types_string(num - 1), name);
        
    } else {
    
        return "";
    
    }
}

// equality comparison for string_t (ignores func_string
int eq_string(string_t s1, string_t s2) {
    if_type_string(s1, s1_s)
        if_type_string(s2, s2_s)
            if (strcmp(s1_s, s2_s) == 0)
                return 1;
        end_if()
    end_if()
    return 0;
}


