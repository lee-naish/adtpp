#include<stdio.h>
#include<string.h>
#include "adt_t.h"
typedef char *c_string;

#define TYPE_LENGTH 500
// XXX see comment in parser.y re hard-coding 7
#define ADT_LOW_BITS 7
#define WARNING_AVOID(val) \
if(val); //handle unused value to avoid warnings

//----------------------   Definition for functions  ---------------------------
//Type definition for .h file
//If users want to use their typedefs, should add the primitive type here.


/*
basic logic about this function, assume the low 3 bits are used

    ( T_NUM_NONCONST>0 && v >= T_NUM_CONST ?     // 1.
        ( T_NUM_NONCONST==1 ?                     // 2.
            T_NUM_CONST
        :
            // tag = v&7,                         // 3.
            ( T_NUM_NONCONST>8 && (v&7) == 7 ?    // 4.
                *(Tstruct*)(v-(v&7))->ftag + T_NUM_CONST
            :
                (v&7) + T_NUM_CONST
            )
        )
    :
        v
    )

 */
int constructor_number(uintptr_t v, int T_NUM_CONST, int T_NUM_NONCONST, int tag){
	return  ( T_NUM_NONCONST>0 && v >= T_NUM_CONST ?     // 1.
        ( T_NUM_NONCONST==1 ?                     // 2.
            T_NUM_CONST
        :
            // tag = v&(ADT_LOW_BITS),                         // 3.
            (  ( T_NUM_NONCONST > (ADT_LOW_BITS + 1))  && ((v&ADT_LOW_BITS) == (ADT_LOW_BITS))  )  ?    // 4.
                tag + T_NUM_CONST
            :
                (v&(ADT_LOW_BITS)) + T_NUM_CONST
        )
    :
        v
    );
}


void field_if_rhs(stringlist_t arg_list, c_string Type, c_string Con, FILE* fp, int Tag, int num, int is_const){
	int i = num;
	if_cons_stringlist_t(arg_list, a_val, a_next)
		if_type_string(a_val, a_str)
			//if is a const constructor
			if(is_const == 1){
				fprintf(fp, "\\\n%s v%d=((struct _ADT_%s_%s*)((uintptr_t)_ADT_v))->f%d; ", a_str, i, Type, Con, i);
			}
			//check if the tag is larger than or equals to 7
			else if(Tag >= ADT_LOW_BITS){
				fprintf(fp, "\\\n%s v%d=((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-ADT_LOW_BITS))->f%d; ", a_str, i, Type, Con, i);
			}else{
				fprintf(fp, "\\\n%s v%d=((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-%d))->f%d; ", a_str, i, Type, Con, Tag, i);
			}
			i++;
			field_if_rhs(a_next, Type, Con, fp, Tag, i, is_const);
        else_if_func_string(start_string, end_string)
            //if is a const constructor
			if(is_const == 1){
				fprintf(fp, "\\\n%sv%d%s=((struct _ADT_%s_%s*)((uintptr_t)_ADT_v))->f%d; ", start_string, i, end_string, Type, Con, i);
			}
			//check if the tag is larger than or equals to 7
			else if(Tag >= ADT_LOW_BITS){
				fprintf(fp, "\\\n%sv%d%s=((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-ADT_LOW_BITS))->f%d; ", start_string, i, end_string, Type, Con, i);
			}else{
				fprintf(fp, "\\\n%sv%d%s=((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-%d))->f%d; ", start_string, i, end_string, Type, Con, Tag, i);
			}
			i++;
			field_if_rhs(a_next, Type, Con, fp, Tag, i, is_const);
		end_if()
	end_if()

	if_nil_stringlist_t(arg_list)
		return;
	end_if()
}


void field_if_args(stringlist_t arg_list, FILE* fp, int num){
	int i = num;
	if_cons_stringlist_t(arg_list, a_val, a_next)
		WARNING_AVOID(a_val)
		fprintf(fp, ", v%d", i++);
		field_if_args(a_next, fp, i);
	end_if()

	if_nil_stringlist_t(arg_list)
		return;
	end_if()
}


void field_defs(stringlist_t arg_list, FILE* fp, int num){
	int i = num;
	if_cons_stringlist_t(arg_list, a_val, a_next)
		if_type_string(a_val, a_str)
			fprintf(fp, "\n    %s f%d;", a_str, i++);
			field_defs(a_next, fp, i);
        else_if_func_string(start_string, end_string)
            fprintf(fp, "\n    %sf%d%s;", start_string, i++, end_string);
			field_defs(a_next, fp, i);
		end_if()
	end_if()

	if_nil_stringlist_t(arg_list)
		return;
	end_if()
}


void field_case_args(stringlist_t arg_list, FILE* fp, int num){
	int i = num;

	if_cons_stringlist_t(arg_list, a_val, a_next)
		WARNING_AVOID(a_val)
		if_type_string(a_val, a_str)
			WARNING_AVOID(a_str)
			if(i == 0){
				fprintf(fp, "v%d", i++);
			}
			else{
				fprintf(fp, ", v%d", i++);
			}
			field_case_args(a_next, fp, i);
        else()
			if(i == 0){
				fprintf(fp, "v%d", i++);
			}
			else{
				fprintf(fp, ", v%d", i++);
			}
			field_case_args(a_next, fp, i);
		end_if()
	end_if()

	if_nil_stringlist_t(arg_list)
		return;
	end_if()

}

// not quite so ugly version of above for arglist_to
// XXX should have polymorphic version for both
void print_macro_args(arglist_t arg_list, FILE* fp, int num){
	int i = num;

	if_cons_arglist_t(arg_list, a_val, a_next)
		WARNING_AVOID(a_val)
			if(i == 0){
				fprintf(fp, "v%d", i++);
			}
			else{
				fprintf(fp, ", v%d", i++);
			}
			print_macro_args(a_next, fp, i);
	end_if()

}


void field_if_ptr_rhs(stringlist_t arg_list, c_string Type, c_string Con, FILE* fp, int Tag, int num, int is_const){
	int i = num;
	if_cons_stringlist_t(arg_list, a_val, a_next)
		if_type_string(a_val, a_str)
			if(is_const == 1){
				fprintf(fp, "\\\n%s *v%d=&((struct _ADT_%s_%s*)((uintptr_t)_ADT_v))->f%d; ", a_str, i, Type, Con, i);
			}
			else if(Tag >= ADT_LOW_BITS){
				fprintf(fp, "\\\n%s *v%d=&((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-ADT_LOW_BITS))->f%d; ", a_str, i, Type, Con, i);
			}else{
				fprintf(fp, "\\\n%s *v%d=&((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-%d))->f%d; ", a_str, i, Type, Con, Tag, i);
			}
			i++;
			field_if_ptr_rhs(a_next, Type, Con, fp, Tag, i, is_const);
        else_if_func_string(start_string, end_string)
            if(is_const == 1){
				fprintf(fp, "\\\n%s*v%d%s=&((struct _ADT_%s_%s*)((uintptr_t)_ADT_v))->f%d; ", start_string, i, end_string, Type, Con, i);
			}else if(Tag >= ADT_LOW_BITS){
				fprintf(fp, "\\\n%s*v%d%s=&((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-ADT_LOW_BITS))->f%d; ", start_string, i, end_string, Type, Con, i);
			}else{
				fprintf(fp, "\\\n%s*v%d%s=&((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-%d))->f%d; ", start_string, i, end_string, Type, Con, Tag, i);
			}
			i++;
			field_if_ptr_rhs(a_next, Type, Con, fp, Tag, i, is_const);
		end_if()
	end_if()

	if_nil_stringlist_t(arg_list)
		return;
	end_if()
}


void arg_defs(stringlist_t arg_list, FILE* fp, int num){
	int i = num;
	if_cons_stringlist_t(arg_list, a_val, a_next)

		if_type_string(a_val, a_str)
			if(i == 0){
				fprintf(fp, "%s v%d", a_str, i);
			}else{
				fprintf(fp, ", %s v%d", a_str, i);
			}
			i++;
        else_if_func_string(start_string, end_string)
            if(i == 0){
				fprintf(fp, "%sv%d%s", start_string, i, end_string);
			}else{
				fprintf(fp, ", %sv%d%s", start_string, i, end_string);
			}
			i++;
		end_if()
		arg_defs(a_next, fp, i);
	end_if()

	if_nil_stringlist_t(arg_list)

		return;
	end_if()

}

void malloc_prototype(c_string Type, c_string Con, stringlist_t arg_list, FILE *fp){
	fprintf(fp, "\nstatic __inline %s %s(", Type, Con);


	arg_defs(arg_list, fp, 0);
	fprintf(fp, ")");
}

//generate a free function for each type
//like void free_TYPE(TYPE v);
void free_prototype(c_string Type, FILE* fp){
	fprintf(fp, "static __inline void free_%s(%s v)", Type, Type);
}



//----------------------   Definition for h file   -----------------------------



//_-----------------------------------------------------------------------------


//char* Type, char* Con, char input_list[][TYPE_LENGTH], int c_num, int num, int Tag, FILE* fp, int fnum
void hfile_generation(c_string Type, c_string Con, stringlist_t arg_list, FILE* fp, int Tag, int is_const, int cp, int np){


	// XXX this is very verbose - it can be cleaned up by generating
	// a more complex macro and getting cpp/gcc to simplify it at
	// compile time as we do in switches

	// This if branch only address the non-const constructor and its
	// number of args larger than 8 but not equal to 8
 	if(Tag >= ADT_LOW_BITS && is_const == 0 && np > ADT_LOW_BITS + 1){

		fprintf(fp, ";\nstruct _ADT_%s_%s {", Type, Con);

		fprintf(fp, "\n\tint ftag;");
		field_defs(arg_list, fp, 0);

		fprintf(fp, "};\n#define if_%s(v", Con);
		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);", Type);

		fprintf(fp, "\\\nif ((uintptr_t)(_ADT_v) >= %d && ((uintptr_t)(_ADT_v)&ADT_LOW_BITS)==ADT_LOW_BITS && (((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-ADT_LOW_BITS))->ftag == %d)){", cp, Type, Con, Tag);
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);


		fprintf(fp, "\n#define else_if_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if ((uintptr_t)(_ADT_v) >= %d && ((uintptr_t)(_ADT_v)&ADT_LOW_BITS)==ADT_LOW_BITS && (((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-ADT_LOW_BITS))->ftag == %d)) {", cp, Type, Con, Tag);

		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if ((uintptr_t)(_ADT_v) >= %d && ((uintptr_t)(_ADT_v)&ADT_LOW_BITS)==ADT_LOW_BITS && (((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-ADT_LOW_BITS))->ftag == %d)) {", cp, Type, Con, Tag);

		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);


		fprintf(fp, "\n#define if_%s_ptr(v", Con);
		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);{", Type);

		fprintf(fp, "\\\nif ((uintptr_t)(_ADT_v) >= %d && ((uintptr_t)(_ADT_v)&ADT_LOW_BITS)==ADT_LOW_BITS && (((struct _ADT_%s_%s*)((uintptr_t)_ADT_v-ADT_LOW_BITS))->ftag == %d)) {",cp, Type, Con, Tag);

		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define case_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag + cp, Type, Type, Type);
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);


		fprintf(fp, "\n#define case_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag + cp, Type, Type, Type);
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);
		fprintf(fp, "\n");
	}else if(cp == 1 && np == 0){
		// The number of constant constructor is 1 and the
		// non-constructor is 0
		fprintf(fp, ";\nstruct _ADT_%s_%s {", Type, Con);

		field_defs(arg_list, fp, 0);
		fprintf(fp, "};\n#define if_%s(v", Con);

		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if (1) {", Type);

		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if (1) {");
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if (1) {");
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define if_%s_ptr(v", Con);
		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if (1) {", Type);

		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define case_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag, Type, Type, Type);
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define case_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag, Type, Type, Type);
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);
		fprintf(fp, "\n");

}else if(cp == 0 && np == 1){
		// handle the very specially case.
		// when only have a non-const
		fprintf(fp, ";\nstruct _ADT_%s_%s {", Type, Con);

		field_defs(arg_list, fp, 0);
		fprintf(fp, "};\n#define if_%s(v", Con);

		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if (1) {", Type);

		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if (1) {");
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if (1) {");
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define if_%s_ptr(v", Con);
		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if (1) {", Type);

		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define case_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag + cp, Type, Type, Type);
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);


		fprintf(fp, "\n#define case_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag + cp, Type, Type, Type);
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);
		fprintf(fp, "\n");
	}else if(is_const == 0 && np == 1){
		// handle the very specially case.
		// when only have a non-const
		fprintf(fp, ";\nstruct _ADT_%s_%s {", Type, Con);

		field_defs(arg_list, fp, 0);
		fprintf(fp, "};\n#define if_%s(v", Con);

		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if ((uintptr_t)(_ADT_v) >= %d) {", Type, cp);

		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if ((uintptr_t)(_ADT_v) >= %d) {", cp);
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if ((uintptr_t)(_ADT_v) >= %d) {", cp);
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define if_%s_ptr(v", Con);
		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if ((uintptr_t)(_ADT_v) >= %d) {", Type, cp);

		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define case_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag + cp, Type, Type, Type);
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);


		fprintf(fp, "\n#define case_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag + cp, Type, Type, Type);
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);
		fprintf(fp, "\n");
	// for constant constructors
	}else if(is_const == 1){
		fprintf(fp, ";\nstruct _ADT_%s_%s {", Type, Con);

		field_defs(arg_list, fp, 0);
		fprintf(fp, "};\n#define if_%s(v", Con);

		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if (((uintptr_t)(_ADT_v))==%d) {", Type, Tag);

		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if (((uintptr_t)(_ADT_v))==%d) {", (Tag));
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if (((uintptr_t)(_ADT_v))==%d) {", (Tag));
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define if_%s_ptr(v", Con);
		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if (((uintptr_t)(_ADT_v))==%d) {", Type, (Tag));

		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define case_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag, Type, Type, Type);
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);


		fprintf(fp, "\n#define case_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag, Type, Type, Type);
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);
		fprintf(fp, "\n");

	}else {
		fprintf(fp, ";\nstruct _ADT_%s_%s {", Type, Con);

		field_defs(arg_list, fp, 0);
		fprintf(fp, "};\n#define if_%s(v", Con);

		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if ((uintptr_t)(_ADT_v) >= %d && ((uintptr_t)(_ADT_v)&ADT_LOW_BITS)==%d) {", Type, cp, Tag);

		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if ((uintptr_t)(_ADT_v) >= %d && ((uintptr_t)(_ADT_v)&ADT_LOW_BITS)==%d)  {", cp, (Tag));

		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define else_if_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n} else if ((uintptr_t)(_ADT_v) >= %d && ((uintptr_t)(_ADT_v)&ADT_LOW_BITS)==%d)  {", cp, (Tag));

		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define if_%s_ptr(v", Con);
		field_if_args(arg_list, fp, 0);

		fprintf(fp, ") \\\n{%s _ADT_v=(v);if ((uintptr_t)(_ADT_v) >= %d && ((uintptr_t)(_ADT_v)&ADT_LOW_BITS)==%d)  {", Type, cp, (Tag));

		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);

		fprintf(fp, "\n#define case_%s(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag + cp, Type, Type, Type);
		field_if_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);


		fprintf(fp, "\n#define case_%s_ptr(", Con);
		field_case_args(arg_list, fp, 0);

		fprintf(fp, ") \\\nbreak;}} case %d: {{%s _SW_tchk=_%s_tchk;}{char _%s_tchk; ", Tag + cp, Type, Type, Type);
		field_if_ptr_rhs(arg_list, Type, Con, fp, Tag, 0, is_const);
		fprintf(fp, "\n");

	}



}
/*

int _ADT_lowbit = (((uintptr_t)(_ADT_v)&ADT_LOW_BITS); \
int ftag = _ADT_lowbit ==ADT_LOW_BITS) ? _ADT_lowbit : 0; \

#define constructorNum(v, ftag) \\\n
{( T_NUM_NONCONST>0 && v > T_NUM_CONST-1 ?  \\\n
        ( T_NUM_NONCONST==1 ?  T_NUM_CONST   : \\\n
            (  ( T_NUM_NONCONST > (ADT_LOW_BITS + 1))  && ((v&ADT_LOW_BITS) == (ADT_LOW_BITS))  )  ?   \\\n
               				ftag + T_NUM_CONST  : \\\n
                   (v&(ADT_LOW_BITS)) + T_NUM_CONST) : v ) }\\\n



 */
void def_of_constructor_num(c_string Type, c_string Con, FILE *fp, int cp, int np){
// XXX should use _ADT_ prefix here and elsewhere
//        fprintf(fp, "#define _ADT_%s_T_NUM_CONST (%d) \n", Type, cp);

	fprintf(fp, "#define %s_T_NUM_CONST (%d) \n", Type, cp);
	fprintf(fp, "#define %s_T_NUM_NONCONST (%d) \n", Type, np);

	// XXX should define constructorNum macro so its optimised away when
	// there is a single constructor in the type
	fprintf(fp, "#define %s_constructorNum(v) \\\n", Type);
	fprintf(fp, "(( %s_T_NUM_NONCONST>0 && v >= %s_T_NUM_CONST ?  \\\n", Type, Type);
	fprintf(fp, " ( %s_T_NUM_NONCONST==1 ?  %s_T_NUM_CONST   : \\\n", Type, Type);
	fprintf(fp, "(( %s_T_NUM_NONCONST > (ADT_LOW_BITS + 1))  && ((v&ADT_LOW_BITS) == (ADT_LOW_BITS))  )  ?   \\\n", Type);
	fprintf(fp, " (*(int*)((uintptr_t)_ADT_v-ADT_LOW_BITS)) + %s_T_NUM_CONST  : \\\n", Type);
	fprintf(fp, "((v&(ADT_LOW_BITS)) + %s_T_NUM_CONST)) : v ) )\n\n", Type);

}
void mono_type_def_h_file(c_string Type, c_string Con, FILE *fp, int cp, int np, int is_enum){

	fprintf(fp, "\n/***************************/\ntypedef struct _ADT_%s{} *%s;\n",Type, Type);
	def_of_constructor_num(Type, Con, fp, cp, np);
	if(is_enum == 1){
		// XXX better if a free() function which does nothing is
		// generated
		// if the data structure is an enum, no free function is given
		fprintf(fp, "\n#define switch_%s(v) \\\n{%s _%s_tchk, _ADT_v=(v);", Type, Type, Type);
	}else{
		free_prototype(Type, fp);
		fprintf(fp, ";\n#define switch_%s(v) \\\n{%s _%s_tchk, _ADT_v=(v);", Type, Type, Type);
	}

	fprintf(fp, " \\\nswitch(%s_constructorNum((uintptr_t)(_ADT_v))){{{\n", Type);


}
//------------------------------------------------------------------------------







//--------------------------  Definition of .c file    -------------------------

#define STC_LENGTH 100

void field_malloc_eqs(stringlist_t arg_list, FILE* fp, int num){
	int i = num;
	if_cons_stringlist_t(arg_list, a_val, a_next)
		WARNING_AVOID(a_val)
		fprintf(fp, "\n\tv->f%d=v%d; ", i, i);
		i++;
		field_malloc_eqs(a_next, fp, i);
	end_if()

	if_nil_stringlist_t(arg_list)
		return;
	end_if()
}

void function_generation(c_string Type, c_string Con, stringlist_t arg_list, FILE* fp, int Tag, int is_const, int np){
	char STC[STC_LENGTH] = "struct _ADT_";
	strcat(STC, Type);
	strcat(STC, "_");
	strcat(STC, Con);

	malloc_prototype(Type, Con, arg_list, fp);

	if(is_const == 1){
		//a const should read the whole word
		fprintf(fp, "{\n\t%s *v=(%s*)%d;", STC, STC, Tag);
		field_malloc_eqs(arg_list, fp, 0);    // The subscript starts from 0
		fprintf(fp, "\n\treturn (%s)((uintptr_t)v);\n}\n", Type);
	}
	else if(Tag >= ADT_LOW_BITS && np > ADT_LOW_BITS + 1){
		//add extra space for second-tag
		fprintf(fp, "{\n\t%s *v=(%s*)ADT_MALLOC(sizeof(%s));", STC, STC, STC);
		fprintf(fp, "\n\tv->ftag = %d; ", Tag);
		field_malloc_eqs(arg_list, fp, 0);    // The subscript starts from 0
		fprintf(fp, "\n\treturn (%s)(ADT_LOW_BITS+(uintptr_t)v);\n}\n", Type);
	}else{
		fprintf(fp, "{\n\t%s *v=(%s*)ADT_MALLOC(sizeof(%s));", STC, STC, STC);
		field_malloc_eqs(arg_list, fp, 0);    // The subscript starts from 0
		fprintf(fp, "\n\treturn (%s)(%d+(uintptr_t)v);\n}\n", Type, Tag);
	}


}


void mono_type_def_function(c_string Type, FILE* fp, int is_const, int is_enum, int cp, int np){
	fprintf(fp, "\n\n/******************************************************************************/\n");
	if(is_enum == 0){
		free_prototype(Type, fp);
		fprintf(fp, "{\n\tif ((uintptr_t)(v) >= %d){\n\t\t", cp);
		// Only need to free the non-constant constructor
		if(np == 1){
			//if the number of non-const is exactly 1 then free the struct directly
			fprintf(fp, "ADT_FREE((void*)((uintptr_t)v));");
		}else{
			fprintf(fp, "ADT_FREE((void*)((uintptr_t)v&~ADT_LOW_BITS));");
		}
		// align space for presentation
		fprintf(fp, "\n\t}\n}\n\n");
	}

}
//------------------------------------------------------------------------------


//-------------------------    Main Fuction        -----------------------------


#define ARRAY_SIZE 200
#define CONTENT_SIZE 50000
#define FILENAME_SIZE 200

void adt_main(char filename[], c_string Type, c_string Con, stringlist_t arg_list, FILE* fp_h, int tag, int is_const, int cp, int np, int *is_init){


	//define the file name to be generated
	char file_name[FILENAME_SIZE];
	strcpy(file_name, filename);

	char path[] = "./";
	char suffix[] = ".h";
	char h_file_name[ARRAY_SIZE];

	//definition for .h file name and path
	strcpy(h_file_name, path);
	strcat(h_file_name, file_name);
	strcat(h_file_name, suffix);

	// XXX looks like fopen is called for each constructor
	// of each type, or some such horrendous thing -
	// should open once and pass in the file descriptor.
	// Same pattern is used elsewhere (eg in polymorphism)
	fp_h =fopen(h_file_name, "a+");
	int is_enum = 0;
	// if the number of non-const is 0, then
	// this data structure is an enum
	if(np == 0 && cp >= 0){
		is_enum = 1;
	}

	if(*is_init == 0){
		*is_init = 1;
		mono_type_def_h_file(Type, Con, fp_h, cp, np, is_enum);
		mono_type_def_function(Type, fp_h, is_const, is_enum, cp, np);
	}

	hfile_generation(Type, Con, arg_list, fp_h, tag, is_const, cp, np);

    function_generation(Type, Con, arg_list, fp_h, tag, is_const, np);

	fclose(fp_h);
}

// print #define for if_ etc to .h file
// if we have a type which is equivalent to an implicitly defined
// generic type we pass in "" as t1
void print_eqv_macro(FILE *fp, char *t1, char *t2, char *c, arglist_t as, char *m1, char *m2) {
    fprintf(fp, "#define %s%s_%s%s(", m1, c, t2, m2);
    print_macro_args(as, fp, 0);
    if (*t1) {
        fprintf(fp, ") %s%s_%s%s(", m1, c, t1, m2);
    } else { // avoid constructor postfix
        fprintf(fp, ") %s%s%s(", m1, c, m2);
    }
    print_macro_args(as, fp, 0);
    fprintf(fp, ")\n");
}

// output macros for two equivalent types
// same hideous coding style... (see adt_main)
void output_eqv_types(c_string filename, c_string t1, c_string t2, decl_t dec) {
    FILE* fp_h;
    ctrlist_t ctr_list;
    arglist_t al1;
    //define the file name to be generated
    char file_name[FILENAME_SIZE];
    strcpy(file_name, filename);

    char path[] = "./";
    char suffix[] = ".h";
    char h_file_name[ARRAY_SIZE];

    //definition for .h file name and path
    strcpy(h_file_name, path);
    strcat(h_file_name, file_name);
    strcat(h_file_name, suffix);

    // XXX fopen called multiple times... (see adt_main)
    fp_h =fopen(h_file_name, "a+");
    fprintf(fp_h, "#define %s %s\n", t2, t1);
    fprintf(fp_h, "#define free_%s(v) free_%s(v)\n", t2, t1);
    if_paramdecl(dec, s_head_decl, s_head_ctr_list, s_head_param_list)
        ctr_list = s_head_ctr_list;
        if_type_string(s_head_decl, dec_str)
            if(strcmp(t1, dec_str) == 0)
                t1 = ""; // implicit generic type - avoid constructor postfix
        end_if()
    else_if_decl(s_head_decl, s_head_ctr_list)
        ctr_list = s_head_ctr_list;
        if_type_string(s_head_decl, dec_str)
            if(strcmp(t1, dec_str) == 0)
                t1 = ""; // implicit generic type - avoid constructor postfix
        end_if()
    else()
        exit(-1);
    end_if()
    while (1) {
        if_cons_ctrlist_t(ctr_list, f_c_head, f_c_tail)
            if_ctr(f_c_head, f_c_h_name, f_c_h_arg)
                if_type_string(f_c_h_name, c_str)
                    // al1 = arg list of length one more than f_c_h_arg
                    al1 = cons_arglist_t(arg(type_string("")), f_c_h_arg);
                    print_eqv_macro(fp_h, t1, t2, c_str, f_c_h_arg, "", ""); // data cons
                    print_eqv_macro(fp_h, t1, t2, c_str, al1, "if_", "");
                    print_eqv_macro(fp_h, t1, t2, c_str, al1, "if_", "_ptr");
                    print_eqv_macro(fp_h, t1, t2, c_str, f_c_h_arg, "else_if_", "");
                    print_eqv_macro(fp_h, t1, t2, c_str, f_c_h_arg, "else_if_", "_ptr");
                    print_eqv_macro(fp_h, t1, t2, c_str, f_c_h_arg, "case_", "");
                    print_eqv_macro(fp_h, t1, t2, c_str, f_c_h_arg, "case_", "_ptr");
                else()
                    exit(-1);
                end_if()
             end_if()
             ctr_list = f_c_tail;
         else()
             break;
         end_if()
    }
    fclose(fp_h);
}

// more of the same...
void output_undef_type(c_string filename, c_string t1, c_string t2) {
    FILE* fp_h;
    //define the file name to be generated
    char file_name[FILENAME_SIZE];
    strcpy(file_name, filename);

    char path[] = "./";
    char suffix[] = ".h";
    char h_file_name[ARRAY_SIZE];

    //definition for .h file name and path
    strcpy(h_file_name, path);
    strcat(h_file_name, file_name);
    strcat(h_file_name, suffix);

    // XXX fopen called multiple times... (see adt_main)
    fp_h =fopen(h_file_name, "a+");
    fprintf(fp_h, "#define %s %s // latter should be defined somewhere!\n", t1, t2);
}

//------------------------------------------------------------------------------
