#include <stdio.h>
#include <time.h>
#include "JDPL.h"

//TODO - find easier method for automated unit testing

//expected result: under 500ms
double obj_million_allocation();

//expected result: under 100ms
double arr_million_allocation();

//expected result: {"A":"Test","This":"is","ratio":14.592180,"active":false,"name":null,"ID":97012}
const char* obj_str_result = "{\"A\":\"Test\",\"This\":\"is\",\"ratio\":14.592180,\"active\":false,\"name\":null,\"ID\":97012}";
char* obj_parsing();

//expected result: ["We","like","to","party","but on tuesday"]
const char* arr_str_result = "[\"We\",\"like\",\"to\",\"party\",\"but on tuesday\"]";
char* arr_parsing();

void run_unit_tests();

int main()
{
	run_unit_tests();
    return 0;
}

void run_unit_tests()
{
	int fail_counter = 0;
	
	double obj_time = obj_million_allocation();
	int result1 = obj_time < 500;
	printf("Object allocation test: %s\n", result1 ? "pass" : "fail");

	if(!result1)
	{
		printf("Expected result: <500ms. Actual result: %fms\n\n", obj_time);
		fail_counter++;
	}

	double arr_time = arr_million_allocation();
	int result2 = arr_time < 100;
	printf("Array allocation test: %s\n", result2 ? "pass" : "fail");

	if(!result2)
	{
		printf("Expected result: <100ms. Actual result: %fms\n\n", arr_time);
		fail_counter++;
	}

	const char* obj_str = obj_parsing();
	int result3 = strcmp(obj_str, obj_str_result) == 0;
	printf("Object parse test: %s\n", result3 ? "pass" : "fail");

	if(!result3)
	{
		printf("Expected result: %s\n", obj_str_result);
		printf("Actual result: %s\n\n", obj_str);
		fail_counter++;
	}

	const char* arr_str = arr_parsing();
	int result4 = strcmp(arr_str, arr_str_result) == 0;
	printf("Array parse test: %s\n", result4 ? "pass" : "fail");

	if(!result4)
	{
		printf("Expected result: %s\n", arr_str_result);
		printf("Actual result: %s\n\n", arr_str);
		fail_counter++;
	}

	printf("\nFailed tests: %d\n", fail_counter);
}

/*
 This test allocates a million values into a jdpl_obj
 in order to ensure the library works fast enough,
 the execution time of this must remain low
*/
double obj_million_allocation()
{
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

	return ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
}

/*
 This test allocates a million values into a jdpl_arr
 in order to ensure the library works fast enough,
 the execution time of this must remain low
*/
double arr_million_allocation()
{
	clock_t start = clock();

	jdpl_arr* test = jdpl_new_arr();
	for(int i = 0; i < 1000000; i++)
	{
		jdpl_arradd(jdpl_vali(i), test);
	}
	jdpl_free_arr(test);

	clock_t end = clock();

	return ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
}

/*
 This test checks to see if a test json string can be parsed correctly
*/
char* obj_parsing()
{
    jdpl_obj* test = jdpl_obj_fromstr("{\"This\":   \"is\"  , \"A\":   \"Test\"  , \"ID\":   97012  , \"name\":   null, \"active\":   false, \"ratio\": 14.59218}");
	char* str = jdpl_obj_tostr(test);
	jdpl_free_obj(test);

	return str;
}

/*
 This test checks to see if a test json array string can be parsed correctly
*/
char* arr_parsing()
{
	jdpl_arr* test = jdpl_new_arr();
	jdpl_arradd(jdpl_vals("We"), test);
	jdpl_arradd(jdpl_vals("like"), test);
	jdpl_arradd(jdpl_vals("to"), test);
	jdpl_arradd(jdpl_vals("party"), test);
	jdpl_arradd(jdpl_vals("but on tuesday"), test);

	char* str = jdpl_arr_tostr(test);
	jdpl_free_arr(test);

	return str;
}
