#include <stdio.h>
#include <time.h>
#include "JDPL.h"

#define PRINT_FAILED 1
#define DEBUG_BUILD 1

//color for text
#define ANSI_COLOR_GREEN "\033[0;32m"
#define ANSI_COLOR_RED   "\033[0;31m"
#define ANSI_COLOR_RESET "\033[0m"

int tests_run = 0;

//expected result: under 500ms
int obj_million_allocation();

//expected result: under 100ms
int arr_million_allocation();

//expected result: second copy does not match first
int obj_deep_copy();

//expected result: second copy does not match first
int arr_deep_copy();

char* object_output(char* input);
char* array_output(char* input);
int run_test(char* input, char* expected_output, char* log_message, char* (*check_function)(char*));

void run_unit_tests();

int main()
{
	run_unit_tests();
    return 0;
}

/*
  Run all unit tests
*/
void run_unit_tests()
{
	int pass_counter = 0;

	pass_counter += obj_million_allocation();
	pass_counter += obj_deep_copy();
	pass_counter += run_test(
		"{\"This\"  :   \"is\"  , \"A\":   \"Test\"  , \"ID\":   97012  , \"name\":   null, \"active\":   false, \"ratio\": 14.59218}",
		"{\"A\":\"Test\",\"This\":\"is\",\"ratio\":14.592180,\"active\":false,\"name\":null,\"ID\":97012}",
		"Basic object parsing",
		object_output
		);

	pass_counter += run_test(
		"{}",
		"{}",
		"Empty object parsing",
		object_output
		);
	
	pass_counter += run_test(
		"",
		"{}",
		"Empty string object parsing",
		object_output
		);

	pass_counter += run_test(
		"}",
		"{}",
		"No start (}) object parsing",
		object_output
		);

	pass_counter += run_test(
		"{",
		"{}",
		"No end ({) object parsing",
		object_output
		);

	pass_counter += run_test(
		"{]",
		"{}",
		"mismatched ({]) object parsing",
		object_output
		);

	pass_counter += run_test(
		"{\"This\"   \"is\"  , \"A\":   \"Test\"   \"ID\":   97012  , \"name\":, \"active\":   false, \"ratio\": 14.59218}",
		"{}",
		"Malformed object input",
		object_output
		);

	pass_counter += run_test(
		"{\"Name\"   : \"cattimus\", \"obj\"  : {\"test\"  :\"obj\"  }}",
		"{\"obj\":{\"test\":\"obj\"},\"Name\":\"cattimus\"}",
		"Object nested in object",
		object_output
		);

	pass_counter += run_test(
		"{\"Name\"   : \"cattimus\", \"arr\"    : [\"test\"  , \"obj\"  ]}",
		"{\"Name\":\"cattimus\",\"arr\":[\"test\",\"obj\"]}",
		"Array nested in object",
		object_output
		);

	printf("\n\n");

	pass_counter += arr_million_allocation();
	pass_counter += arr_deep_copy();
	pass_counter += run_test(
		"[\"We\",     \"like\",      \"to\"        ,\"party\",\"but on tuesday\"]",
		"[\"We\",\"like\",\"to\",\"party\",\"but on tuesday\"]",
		"Basic array parsing",
		array_output
		);

	pass_counter += run_test(
		"[]",
		"[]",
		"Empty array parsing",
		array_output
		);

	pass_counter += run_test(
		"",
		"[]",
		"Empty string array parsing",
		array_output
		);

	pass_counter += run_test(
		"]",
		"[]",
		"No start (]) array parsing",
		array_output
		);

	pass_counter += run_test(
		"[",
		"[]",
		"No end ([) array parsing",
		array_output
		);

	pass_counter += run_test(
		"[",
		"[]",
		"mismatched ([}) array parsing",
		array_output
		);

	pass_counter += run_test(
		"[\"We\"\"like\"\"to\",party\",\"but on tuesday\"]",
		"[]",
		"Array malformed input",
		array_output
		);

	pass_counter += run_test(
		"[1,2,    3,\"cattimus\", {\"test\"    : \"obj\"}]",
		"[1,2,3,\"cattimus\",{\"test\":\"obj\"}]",
		"Object nested in array",
		array_output
		);

	pass_counter += run_test(
		"[1,2,    3,\"cattimus\", [4,   5, 6  , \"Blimp\"]]",
		"[1,2,3,\"cattimus\",[4,5,6,\"Blimp\"]]",
		"Array nested in array",
		array_output
		);

	printf("\n\n");

	pass_counter += run_test(
		"{\"test\" : 18.192nice41282}",
		"{\"test\":18.192000}",
		"String in the middle of number",
		object_output
		);

	pass_counter += run_test(
		"{\"test\" : 181923test123}",
		"{\"test\":181923}",
		"String in the middle of number 2",
		object_output
		);

	pass_counter += run_test(
		"{\"test\" : 12828093198.19241}",
		"{\"test\":12828093198.192410}",
		"Long decimal print",
		object_output
		);
	

	printf("\nPassed tests: %d/%d\n", pass_counter, tests_run);
}

int run_test(char* input, char* expected_output, char* log_message, char* (*check_function)(char*))
{
	tests_run++;
	char* str = check_function(input);
	int result = strcmp(expected_output, str) == 0;

	printf("%s: %s\n" ANSI_COLOR_RESET, log_message, result ? ANSI_COLOR_GREEN "pass" :  ANSI_COLOR_RED "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("Expected result: %s\n", expected_output);
		printf("Actual result: %s\n\n", str);
	}

	free(str);
	return result;
}

char* object_output(char* input)
{
	jdpl_obj* test = jdpl_obj_fromstr(input);

	char* str = jdpl_obj_tostr(test);
	jdpl_free_obj(test);

	return str;
}

char* array_output(char* input)
{
	jdpl_arr* test = jdpl_arr_fromstr(input);

	char* str = jdpl_arr_tostr(test);
	jdpl_free_arr(test);

	return str;
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

	printf("Object allocation efficiency: %s\n" ANSI_COLOR_RESET, result ? ANSI_COLOR_GREEN "pass" : ANSI_COLOR_RED "fail");

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

	printf("Array allocation efficiency: %s\n" ANSI_COLOR_RESET, result ? ANSI_COLOR_GREEN "pass" : ANSI_COLOR_RED "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("expected result: <100ms\n");
		printf("actual result: %fms\n\n", time_elapsed);
	}

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

	printf("Object deep copy: %s\n" ANSI_COLOR_RESET, result ? ANSI_COLOR_GREEN "pass" : ANSI_COLOR_RED "fail");

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
	printf("Array deep copy: %s\n" ANSI_COLOR_RESET, result ? ANSI_COLOR_GREEN "pass" : ANSI_COLOR_RED "fail");

	if(PRINT_FAILED && (!result))
	{
		printf("Expected result: left(8192), right(500)\n");
		printf("Actual result: left(%f), right(%f)\n\n", *jdpl_arrgetnum(500, jdpl_arrgetarr(0, left)), *jdpl_arrgetnum(500, jdpl_arrgetarr(0, right)));
	}

	jdpl_free_arr(left);
	jdpl_free_arr(right);
	return result;
}
