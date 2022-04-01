#include <stdio.h>
#include <time.h>
#include "JDPL.h"

#define PRINT_FAILED 1
#define DEBUG_BUILD 1

int tests_run = 0;

//expected result: under 500ms
int obj_million_allocation();

//expected result: under 100ms
int arr_million_allocation();

//expected result: {"A":"Test","This":"is","ratio":14.592180,"active":false,"name":null,"ID":97012}
int obj_parsing();

//expected result: ["We","like","to","party","but on tuesday"]
int arr_parsing();

//expected result: second copy does not match first
int obj_deep_copy();

//expected result: second copy does not match first
int arr_deep_copy();

//expected result: "{}"
int obj_malformed_input();

//expected result: "[]"
int arr_malformed_input();

void run_unit_tests();

int main()
{
	run_unit_tests();
    return 0;
}

void run_unit_tests()
{
	int pass_counter = 0;

	pass_counter += obj_million_allocation();
	pass_counter += arr_million_allocation();
	pass_counter += obj_parsing();
	pass_counter += arr_parsing();
	pass_counter += obj_deep_copy();
	pass_counter += arr_deep_copy();
	pass_counter += obj_malformed_input();
	pass_counter += arr_malformed_input();

	printf("\nPassed tests: %d/%d\n", pass_counter, tests_run);
}

/*
  This test checks that a sufficiently mangled input will result in an empty json array string
  the goal is predictable behavior over undefined behavior
*/
int arr_malformed_input()
{
	tests_run++;
	const char* arr_str_result = "[]";
	jdpl_arr* test = jdpl_arr_fromstr("[\"We\"\"like\"\"to\",party\",\"but on tuesday\"]");

	char* str = jdpl_arr_tostr(test);
	jdpl_free_arr(test);

    int result = strcmp(arr_str_result, str) == 0;

	printf("Array parsing: %s\n", result ? "pass" : "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("Expected result: %s\n", arr_str_result);
		printf("Actual result: %s\n\n", str);
	}

	free(str);

	return result;
}

/*
  This test checks that a sufficiently mangled input will result in an empty json string
  the goal is predictable behavior over undefined behavior
*/
int obj_malformed_input()
{
	tests_run++;
	const char* obj_str_result = "{}";
    jdpl_obj* test = jdpl_obj_fromstr("{\"This\"   \"is\"  , \"A\":   \"Test\"   \"ID\":   97012  , \"name\":, \"active\":   false, \"ratio\": 14.59218}");
	char* str = jdpl_obj_tostr(test);
	jdpl_free_obj(test);

	int result = strcmp(obj_str_result, str) == 0;

	printf("Object parsing: %s\n", result ? "pass" : "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("Expected result: %s\n", obj_str_result);
		printf("Actual result: %s\n\n", str);
	}

	free(str);
	return result;
}

/*
 This test allocates a million values into a jdpl_obj
 in order to ensure the library works fast enough,
 the execution time of this must remain low
*/
int obj_million_allocation()
{
	tests_run++;
	clock_t start = clock();

	jdpl_obj* test = jdpl_new_obj();
	for(int i = 0; i < 1000000; i++)
	{
		char str[24];
		sprintf(str, "%d", i);
		jdpl_objadd(str, jdpl_vali(i), test);
	}
	clock_t end = clock();
	jdpl_free_obj(test);

    double time_elapsed = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
	int result = time_elapsed < 500;

	if(DEBUG_BUILD)
	{
		result = time_elapsed < 600;
	}

	printf("Object allocation efficiency: %s\n", result ? "pass" : "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("expected result: <500ms\n");
		printf("actual result: %fms\n\n", time_elapsed);
	}

	return result;
}

/*
 This test allocates a million values into a jdpl_arr
 in order to ensure the library works fast enough,
 the execution time of this must remain low
*/
int arr_million_allocation()
{
	tests_run++;
	clock_t start = clock();

	jdpl_arr* test = jdpl_new_arr();
	for(int i = 0; i < 1000000; i++)
	{
		jdpl_arradd(jdpl_vali(i), test);
	}
	clock_t end = clock();
	
	jdpl_free_arr(test);

	double time_elapsed = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
	int result = time_elapsed < 100;

	printf("Array allocation efficiency: %s\n", result ? "pass" : "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("expected result: <100ms\n");
		printf("actual result: %fms\n\n", time_elapsed);
	}

	return result;
}

/*
 This test checks to see if a test json string can be parsed correctly
*/
int obj_parsing()
{
	tests_run++;
	const char* obj_str_result = "{\"A\":\"Test\",\"This\":\"is\",\"ratio\":14.592180,\"active\":false,\"name\":null,\"ID\":97012}";
    jdpl_obj* test = jdpl_obj_fromstr("{\"This\":   \"is\"  , \"A\":   \"Test\"  , \"ID\":   97012  , \"name\":   null, \"active\":   false, \"ratio\": 14.59218}");
	char* str = jdpl_obj_tostr(test);
	jdpl_free_obj(test);

	int result = strcmp(obj_str_result, str) == 0;

	printf("Object parsing: %s\n", result ? "pass" : "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("Expected result: %s\n", obj_str_result);
		printf("Actual result: %s\n\n", str);
	}

	free(str);
	return result;
}

/*
 This test checks to see if a test json array string can be parsed correctly
*/
int arr_parsing()
{
	tests_run++;
	const char* arr_str_result = "[\"We\",\"like\",\"to\",\"party\",\"but on tuesday\"]";
	jdpl_arr* test = jdpl_arr_fromstr("[\"We\",\"like\",\"to\",\"party\",\"but on tuesday\"]");

	char* str = jdpl_arr_tostr(test);
	jdpl_free_arr(test);

    int result = strcmp(arr_str_result, str) == 0;

	printf("Array parsing: %s\n", result ? "pass" : "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("Expected result: %s\n", arr_str_result);
		printf("Actual result: %s\n\n", str);
	}

	free(str);

	return result;
}

/*
 This tests verifies that a copied jdpl_obj is a deep copy instead of a shallow copy
*/
int obj_deep_copy()
{
	tests_run++;
	jdpl_obj* test = jdpl_new_obj();

	for(int i = 0; i < 1000; i++)
	{
		char str[24];
		sprintf(str, "%d", i);
		jdpl_objadd(str, jdpl_vali(i), test);
	}

	//makes shallow copy of test
	jdpl_obj* left = jdpl_new_obj();
	jdpl_objadd("obj", jdpl_valobj(test), left);

	//makes deep copy of test
	jdpl_obj* right = jdpl_new_obj();
	jdpl_objadd("obj", jdpl_valobj_copy(test), right);

	jdpl_obj* left_test = jdpl_objgetobj("obj", left);
	jdpl_obj* right_test = jdpl_objgetobj("obj", right);

	*jdpl_objgetnum("500", left_test) = 8192;

	int result = (*jdpl_objgetnum("500", left_test) != *jdpl_objgetnum("500", right_test));

	printf("Object deep copy: %s\n", result ? "pass" : "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("Expected result: left(8192), right(500)\n");
		printf("Actual result: left(%f), right(%f)\n\n", *jdpl_objgetnum("500", left_test), *jdpl_objgetnum("500", right_test));
	}

	jdpl_free_obj(left);
	jdpl_free_obj(right);

	return result;
}

/*
 This tests verifies that a copied jdpl_arr is a deep copy instead of a shallow copy
*/
int arr_deep_copy()
{
	tests_run++;

	jdpl_arr* test = jdpl_new_arr();
	for(int i = 0; i < 1000; i++)
	{
		jdpl_arradd(jdpl_vali(i), test);
	}

	jdpl_arr* left = jdpl_new_arr();
	jdpl_arradd(jdpl_valarr(test), left);
	
	jdpl_arr* right = jdpl_new_arr();
	jdpl_arradd(jdpl_valarr_copy(test), right);

	*jdpl_arrgetnum(500, jdpl_arrgetarr(0, left)) = 8192;

	int result = *jdpl_arrgetnum(500, jdpl_arrgetarr(0, left)) != *jdpl_arrgetnum(500, jdpl_arrgetarr(0, right));
	printf("Array deep copy: %s\n", result ? "pass" : "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("Expected result: left(8192), right(500)\n");
		printf("Actual result: left(%f), right(%f)\n\n", *jdpl_arrgetnum(500, jdpl_arrgetarr(0, left)), *jdpl_arrgetnum(500, jdpl_arrgetarr(0, right)));
	}

	jdpl_free_arr(left);
	jdpl_free_arr(right);

	return result;
}
