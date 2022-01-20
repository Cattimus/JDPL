#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define JDPL_MEMSTEP 32

typedef enum JDPL_TYPE_ENUM
{
	JDPL_TYPE_TEXT, JDPL_TYPE_NUM, JDPL_TYPE_DECIMAL, JDPL_TYPE_BOOL, JDPL_TYPE_OBJ, JDPL_TYPE_ARR, JDPL_TYPE_NULL, JDPL_TYPE_INVALID
}JDPL_TYPE;

typedef struct JDPL_VALUE
{
	JDPL_TYPE type; //type of held data
	unsigned int data_size;  //size of held data in bytes
	void* data;     //pointer to data
	
}jdpl_val;

typedef struct JDPL_ARRAY
{
	unsigned int count;     //number of items held by array
	unsigned int max_size;
	jdpl_val* arr; //array of value objects
	
}jdpl_arr;

typedef struct JDPL_OBJECT
{
	unsigned int count;
	unsigned int max_size;
	char** key_arr;
	jdpl_val* val_arr;
}jdpl_obj;

/***********************************************************************
 *********************MEMORY ALLOCATION FUNCTIONS***********************
 ***********************************************************************/

jdpl_obj* jdpl_new_obj()
{
	jdpl_obj* to_return = (jdpl_obj*)malloc(sizeof(jdpl_obj));
	
	to_return->max_size = JDPL_MEMSTEP;
	to_return->count = 0;
	
	//initialize key array
	to_return->key_arr = (char**)malloc(sizeof(char*) * JDPL_MEMSTEP);
	memset(to_return->key_arr, 0, sizeof(char*) * JDPL_MEMSTEP);
	
	//initialize value array
	to_return->val_arr = (jdpl_val*)malloc(sizeof(jdpl_val) * JDPL_MEMSTEP);
	memset(to_return->val_arr, 0, sizeof(jdpl_val) * JDPL_MEMSTEP);
	
	return to_return;
}

jdpl_arr* jdpl_new_arr()
{
	jdpl_arr* to_return = (jdpl_arr*)malloc(sizeof(jdpl_arr));
	
	to_return->max_size = JDPL_MEMSTEP;
	to_return->count = 0;
	
	//initialize array
	to_return->arr = (jdpl_val*)malloc(sizeof(jdpl_val) * JDPL_MEMSTEP);
	memset(to_return->arr, 0, sizeof(jdpl_val) * JDPL_MEMSTEP);
	
	return to_return;
}

jdpl_val jdpl_vali(int data)
{
	jdpl_val to_return;
	to_return.type = JDPL_TYPE_NUM;
	
	to_return.data_size = sizeof(int64_t);
	to_return.data = malloc(to_return.data_size);
	int64_t data_copy = (int64_t) data;
	memcpy(to_return.data, &data_copy, sizeof(int64_t));
	
	return to_return;
}

jdpl_val jdpl_vall(int64_t data)
{
	jdpl_val to_return;
	to_return.type = JDPL_TYPE_NUM;
	
	to_return.data_size = sizeof(int64_t);
	to_return.data = malloc(to_return.data_size);
	memcpy(to_return.data, &data, sizeof(int64_t));
	
	return to_return;
}

jdpl_val jdpl_vals(const char* data)
{
	jdpl_val to_return;
	to_return.type = JDPL_TYPE_TEXT;
	
	to_return.data_size = strlen(data) + 1;
	to_return.data = malloc(to_return.data_size);
	memcpy(to_return.data, data, to_return.data_size);
	
	return to_return;
}

jdpl_val jdpl_valf(float data)
{
	jdpl_val to_return;
	to_return.type = JDPL_TYPE_DECIMAL;
	
	to_return.data_size = sizeof(double);
	to_return.data = malloc(to_return.data_size);
	double data_copy = (double)data;
	memcpy(to_return.data, &data_copy, sizeof(double));
	
	return to_return;
}

jdpl_val jdpl_vald(float data)
{
	jdpl_val to_return;
	to_return.type = JDPL_TYPE_DECIMAL;
	
	to_return.data_size = sizeof(double);
	to_return.data = malloc(to_return.data_size);
	memcpy(to_return.data, &data, sizeof(double));
	
	return to_return;
}

jdpl_val jdpl_valb(int data)
{
	jdpl_val to_return;
	to_return.type = JDPL_TYPE_BOOL;
	
	char bool_data[6];
	
	if(data)
	{
		strcpy(bool_data, "true");
	}
	else
	{
		strcpy(bool_data, "false");
	}
	
	to_return.data_size = sizeof(char) * 6;
	to_return.data = malloc(to_return.data_size);
	memcpy(to_return.data, bool_data, sizeof(char) * 6);
	
	return to_return;
}

//TODO - jdpl_valobj_copy() jdpl_valarr_copy() (deep copy variants)
jdpl_val jdpl_valobj(jdpl_obj* data)
{
	jdpl_val to_return;
	to_return.type = JDPL_TYPE_OBJ;
	
	to_return.data_size = sizeof(jdpl_obj*);
	to_return.data = data;
	
	return to_return;
}

jdpl_val jdpl_valarr(jdpl_arr* data)
{
	jdpl_val to_return;
	to_return.type = JDPL_TYPE_ARR;
	
	to_return.data_size = sizeof(jdpl_arr);
	to_return.data = data;
	
	return to_return;
}

//create a deep copy of the selected value
jdpl_val jdpl_val_copy(jdpl_val* to_copy)
{
	jdpl_val to_return;
	to_return.type = to_copy->type;
	to_return.data_size = to_copy->data_size;
	
	to_return.data = malloc(to_copy->data_size);
	memcpy(to_return.data, to_copy->data, to_copy->data_size);
	
	return to_return;
}

/***********************************************************************
 ***********************MEMORY FREE FUNCTIONS***************************
 ***********************************************************************/
//forward declaration (because these may be recursive)
void jdpl_free_arr(jdpl_arr*);
void jdpl_free_obj(jdpl_obj*);
void jdpl_free_val(jdpl_val*);

void jdpl_free_obj(jdpl_obj* to_free)
{
	if(to_free == NULL)
	{
		return;
	}
	
	for(int i = 0; i < to_free->count; i++)
	{
		if(to_free->key_arr[i] != NULL)
		{
			free(to_free->key_arr[i]);
			to_free->key_arr[i] = NULL;
		}
		
		jdpl_free_val(&to_free->val_arr[i]);
	}
	
	//free key array and value array
	if(to_free->key_arr != NULL)
	{
		free(to_free->key_arr);
		free(to_free->val_arr);
	
		to_free->key_arr = NULL;
		to_free->val_arr = NULL;
	}
	
	free(to_free);
	to_free = NULL;
}

void jdpl_free_arr(jdpl_arr* to_free)
{
	if(to_free == NULL)
	{
		return;
	}
	
	for(int i = 0; i < to_free->count; i++)
	{
		jdpl_free_val(&to_free->arr[i]);
	}
	
	if(to_free->arr != NULL)
	{
		free(to_free->arr);
		to_free->arr = NULL;
	}
	
	free(to_free);
	to_free = NULL;
}

void jdpl_free_val(jdpl_val* to_free)
{
	if(to_free == NULL)
	{
		return;
	}
	
	if(to_free->data == NULL)
	{
		return;
	}
	
	if(to_free->type == JDPL_TYPE_OBJ)
	{
		jdpl_free_obj((jdpl_obj*)to_free->data);
		to_free->data = NULL;
	}
	else if(to_free->type == JDPL_TYPE_ARR)
	{
		jdpl_free_arr((jdpl_arr*)to_free->data);
		to_free->data = NULL;
	}
	else
	{
		free(to_free->data);
		to_free->data_size = 0;
		to_free->data = NULL;
	}
}

//search a JSON object for a particular key. returns NULL if not found.
static jdpl_val* jdpl_search_obj(jdpl_obj* to_search, const char* key)
{
	for(int i = 0; i < to_search->count; i++)
	{
		if(strcmp(to_search->key_arr[i], key) == 0)
		{
			return &to_search->val_arr[i];
		}
	}
	
	return NULL;
}

/***********************************************************************
 ********************OBJECT SET FUNCTIONS*******************************
 ***********************************************************************/

//shallow copy value
void jdpl_objadd(jdpl_obj* to_set, const char* key, jdpl_val val)
{
	int replaced = 0;
	for(int i = 0; i < to_set->count; i++)
	{
		if(strcmp(to_set->key_arr[i], key) == 0)
		{
			jdpl_free_val(&to_set->val_arr[i]);
			to_set->val_arr[i] = val;
			replaced = 1;
		}
	}
	
	if(!replaced)
	{
		//expand key and value arrays
		if(to_set->count == to_set->max_size)
		{
			char** key_temp = (char**) realloc(to_set->key_arr, sizeof(char*) * (to_set->max_size + JDPL_MEMSTEP));
			jdpl_val* val_temp = (jdpl_val*) realloc(to_set->val_arr, sizeof(jdpl_val) * (to_set->max_size + JDPL_MEMSTEP));
			
			//error out if realloc is unsuccessful
			if(key_temp == NULL || val_temp == NULL)
			{
				fprintf(stderr, "jdpl_setobj: Memory allocation error. Could not realloc array of size: %d.\n", to_set->max_size);
				exit(1);
			}
			
			to_set->max_size += JDPL_MEMSTEP;
			to_set->key_arr = key_temp;
			to_set->val_arr = val_temp;
		}
		
		//assign new key and value
		to_set->key_arr[to_set->count] = (char*)malloc(sizeof(char) * strlen(key) + 1);
		memcpy(to_set->key_arr[to_set->count], key, strlen(key) + 1);
		to_set->key_arr[to_set->count][strlen(key)] = '\0'; //set null-terminator if not included already
		to_set->val_arr[to_set->count] = val;
		
		to_set->count++;
	}
}

//deep copy value
void jdpl_objadd_copy(jdpl_obj* to_set, const char* key, jdpl_val val)
{
	int replaced = 0;
	for(int i = 0; i < to_set->count; i++)
	{
		if(strcmp(to_set->key_arr[i], key) == 0)
		{
			jdpl_free_val(&to_set->val_arr[i]);
			to_set->val_arr[i] = jdpl_val_copy(&val);
			replaced = 1;
		}
	}
	
	if(!replaced)
	{
		//expand key and value arrays
		if(to_set->count == to_set->max_size)
		{
			char** key_temp = (char**) realloc(to_set->key_arr, sizeof(char*) * (to_set->max_size + JDPL_MEMSTEP));
			jdpl_val* val_temp = (jdpl_val*) realloc(to_set->val_arr, sizeof(jdpl_val) * (to_set->max_size + JDPL_MEMSTEP));
			
			//error out if realloc is unsuccessful
			if(key_temp == NULL || val_temp == NULL)
			{
				fprintf(stderr, "jdpl_setobj: Memory allocation error. Could not realloc array of size: %d.\n", to_set->max_size);
				exit(1);
			}
			
			to_set->max_size += JDPL_MEMSTEP;
			to_set->key_arr = key_temp;
			to_set->val_arr = val_temp;
		}
		
		//assign new key and value
		to_set->key_arr[to_set->count] = (char*)malloc(sizeof(char) * strlen(key) + 1);
		memcpy(to_set->key_arr[to_set->count], key, strlen(key) + 1);
		to_set->key_arr[to_set->count][strlen(key)] = '\0'; //set null-terminator if not included already
		to_set->val_arr[to_set->count] = jdpl_val_copy(&val);
		
		to_set->count++;
	}
}

/***********************************************************************
 *********************OBJECT GET FUNCTIONS*******************************
 ***********************************************************************/

//generic search
jdpl_val* jdpl_objget(jdpl_obj* to_search, const char* key)
{
	return jdpl_search_obj(to_search, key);
}

//number(int64)
int64_t* jdpl_objgeti(jdpl_obj* to_search, const char* key)
{
	jdpl_val* to_return = jdpl_search_obj(to_search, key);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_NUM)
	{
		return NULL;
	}
	
	return (int64_t*)to_return->data;
}

//text(string)
const char* jdpl_objgets(jdpl_obj* to_search, const char* key)
{
	jdpl_val* to_return = jdpl_search_obj(to_search, key);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_TEXT)
	{
		return NULL;
	}
	
	return (const char*)to_return->data;
}

//decimal(double)
double* jdpl_objgetf(jdpl_obj* to_search, const char* key)
{
	jdpl_val* to_return = jdpl_search_obj(to_search, key);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_DECIMAL)
	{
		return NULL;
	}
	
	return (double*)to_return->data;
}

//bool(boolean) 1 true 0 false -1 error
int jdpl_objgetb(jdpl_obj* to_search, const char* key)
{
	jdpl_val* to_return = jdpl_search_obj(to_search, key);
	
	if(to_return == NULL)
	{
		return -1;
	}
	
	if(to_return->type != JDPL_TYPE_BOOL)
	{
		return -1;
	}
	
	//check if 
	if(strcmp((const char*)to_return->data, "true") == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//obj(jdpl_obj)
jdpl_obj* jdpl_objgetobj(jdpl_obj* to_search, const char* key)
{
	jdpl_val* to_return = jdpl_search_obj(to_search, key);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_OBJ)
	{
		return NULL;
	}
	
	return (jdpl_obj*)to_return->data;
}

//arr(jdpl_arr)
jdpl_arr* jdpl_objgetarr(jdpl_obj* to_search, const char* key)
{
	jdpl_val* to_return = jdpl_search_obj(to_search, key);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_ARR)
	{
		return NULL;
	}
	
	return (jdpl_arr*)to_return->data;
}

/*************************************************************************
 ***************************PRINT FUNCTIONS*******************************
 *************************************************************************/
char* jdpl_val_tostr(jdpl_val*);
char* jdpl_obj_tostr(jdpl_obj*);
char* jdpl_arr_tostr(jdpl_arr*);

//convert value to string
char* jdpl_val_tostr(jdpl_val* to_convert)
{
	if(to_convert == NULL)
	{
		return NULL;
	}
	
	char* to_return = NULL;
	switch(to_convert->type)
	{
		case JDPL_TYPE_BOOL:
			to_return = (char*)malloc(6);
			memcpy(to_return, to_convert->data, 6);
			break;
			
		case JDPL_TYPE_DECIMAL:
			to_return = (char*)malloc(24);
			sprintf(to_return, "%lf", *(double*)to_convert->data);
			break;
			
		case JDPL_TYPE_NUM:
			to_return = (char*)malloc(24);
			sprintf(to_return, "%ld", *(int64_t*)to_convert->data);
			break;
			
		case JDPL_TYPE_TEXT:
			to_return = (char*)malloc(to_convert->data_size + 2);
			to_return[0] = '"';
			memcpy(to_return + 1, to_convert->data, to_convert->data_size);
			to_return[to_convert->data_size] = '"';
			to_return[to_convert->data_size + 1] = '\0';
			break;
			
		case JDPL_TYPE_OBJ:
			to_return = jdpl_obj_tostr((jdpl_obj*)to_convert->data);
			break;
			
		case JDPL_TYPE_ARR:
			to_return = jdpl_arr_tostr((jdpl_arr*)to_convert->data);
			break;
			
		case JDPL_TYPE_NULL:
			to_return = (char*)malloc(5);
			strcpy(to_return, "null");
			break;
			
		case JDPL_TYPE_INVALID:
			break;
	}
	
	return to_return;
}

//convert object to flattened string {obj}
char* jdpl_obj_tostr(jdpl_obj* to_convert)
{
	int str_size = 3;
	char* to_return = (char*)malloc(str_size);
	memset(to_return, 0, 3);
	
	strcat(to_return, "{");
	for(int i = 0; i < to_convert->count; i++)
	{
		int expand_val = 4;
		if(i == to_convert->count - 1)
		{
			expand_val -= 1;
		}
		
		//expand str array
		char* val_str = jdpl_val_tostr(&to_convert->val_arr[i]);
		str_size += strlen(val_str) + strlen(to_convert->key_arr[i]) + expand_val;
		
		char* temp = (char*)realloc(to_return, str_size);
		if(temp == NULL)
		{
			fprintf(stderr, "jdpl_obj_tostr: Memory allocation error. Could not realloc array of size: %d.\n", str_size);
			exit(1);
		}
		to_return = temp;
		
		//add new data
		strcat(to_return, "\"");
		strcat(to_return, to_convert->key_arr[i]);
		strcat(to_return, "\"");
		strcat(to_return, ":");
		strcat(to_return, val_str);
		
		if(i != to_convert->count - 1)
		{
			strcat(to_return, ",");
		}
		free(val_str);
	}
	strcat(to_return, "}");
	
	return to_return;
}

//convert array to flattened string [arr]
char* jdpl_arr_tostr(jdpl_arr* to_convert)
{
	int str_size = 3;
	char* to_return = (char*)malloc(str_size);
	memset(to_return, 0, 3);
	
	strcat(to_return, "[");
	for(int i = 0; i < to_convert->count; i++)
	{
		int expand_val = 1;
		if(i == to_convert->count - 1)
		{
			expand_val -= 1;
		}
		
		//expand str array
		char* val_str = jdpl_val_tostr(&to_convert->arr[i]);
		str_size += strlen(val_str) + expand_val;
		
		char* temp = (char*)realloc(to_return, str_size);
		if(temp == NULL)
		{
			fprintf(stderr, "jdpl_obj_tostr: Memory allocation error. Could not realloc array of size: %d.\n", str_size);
			exit(1);
		}
		to_return = temp;
		
		//add new data
		strcat(to_return, val_str);
		
		if(i != to_convert->count - 1)
		{
			strcat(to_return, ",");
		}
		free(val_str);
	}
	strcat(to_return, "]");
	
	return to_return;
}

//takes condensed input and prettifies it to human-readable json
//TODO - modify input string to be prettified rather than just print
void jdpl_prettify(char* to_prettify, unsigned int indent_size)
{
	int indent_level = 0;
	int in_quote = 0;
	for(int i = 0; i < strlen(to_prettify); i++)
	{
		if(i > 0)
		{
			if(to_prettify[i] == '"' && to_prettify[i-1] != '\\')
			{
				in_quote = !in_quote;
			}
		}
		
		if(!in_quote)
		{
			if(to_prettify[i] == '{' && to_prettify[i+1] != '}')
			{
				printf("{\n");
				indent_level++;
				printf("%*c", indent_level * indent_size, ' ');
			}
			
			else if(to_prettify[i] == '}' && to_prettify[i-1] != '{')
			{
				indent_level--;
				printf("\n");
				printf("%*c", indent_level * indent_size, ' ');
				printf("}");
				
				if(indent_level == 0)
				{
					printf("\n");
				}
			}
			
			else if(to_prettify[i] == '[' && to_prettify[i+1] != ']')
			{
				printf("[\n");
				indent_level++;
				printf("%*c", indent_level * indent_size, ' ');
			}
			
			else if(to_prettify[i] == ']' && to_prettify[i-1] != '[')
			{
				indent_level--;
				printf("\n");
				printf("%*c", indent_level * indent_size, ' ');
				printf("]");
				
				if(indent_level == 0)
				{
					printf("\n");
				}
			}
			
			else if(to_prettify[i] == ',')
			{
				printf(",\n");
				printf("%*c", indent_level * indent_size, ' ');
			}
			else
			{
				printf("%c", to_prettify[i]);
			}
		}
		else
		{
			printf("%c", to_prettify[i]);
		}
	}
}


//search a JSON array for a particular index. return NULL if out of bounds.
static jdpl_val* jdpl_search_arr(jdpl_arr* to_search, int index)
{
	if(index < to_search->count && index >= 0)
	{
		return &to_search->arr[index];
	}
	
	return NULL;
}

/***********************************************************************
 *********************ARRAY SET FUNCTIONS*******************************
 ***********************************************************************/

void jdpl_arradd(jdpl_arr* to_set, jdpl_val val)
{
	//expand key and value arrays if necessary
	if(to_set->count == to_set->max_size)
	{
		jdpl_val* val_temp = (jdpl_val*)realloc(to_set->arr, sizeof(jdpl_val) * (to_set->max_size + JDPL_MEMSTEP));
		
		//error out if realloc is unsuccessful
		if(val_temp == NULL)
		{
			fprintf(stderr, "jdpl_setobj: Memory allocation error. Could not realloc array of size: %d.\n", to_set->max_size);
			exit(1);
		}
		
		to_set->max_size += JDPL_MEMSTEP;
		to_set->arr = val_temp;
	}
	
	//assign new value
	to_set->arr[to_set->count] = val;
	to_set->count++;
}

//overwrite value at index with new value
void jdpl_arrset(jdpl_arr* to_set, unsigned int index, jdpl_val val)
{
	//index is out of bounds
	if(to_set->count < index)
	{
		return;
	}
	
	jdpl_free_val(&to_set->arr[index]);
	to_set->arr[index] = val;
}

/***********************************************************************
 *********************ARRAY GET FUNCTIONS*******************************
 ***********************************************************************/

//generic search
jdpl_val* jdpl_arrget(jdpl_arr* to_search, unsigned int index)
{
	return jdpl_search_arr(to_search, index);
}

//array retrieve functions
//number(int64)
int64_t* jdpl_arrgeti(jdpl_arr* to_search, unsigned int index)
{
	jdpl_val* to_return = jdpl_search_arr(to_search, index);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_NUM)
	{
		return NULL;
	}
	
	return (int64_t*)to_return->data;
}

//decimal(double)
double* jdpl_arrgetf(jdpl_arr* to_search, unsigned int index)
{
	jdpl_val* to_return = jdpl_search_arr(to_search, index);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_DECIMAL)
	{
		return NULL;
	}
	
	return (double*)to_return->data;
}

//bool(boolean)
int* jdpl_arrgetb(jdpl_arr* to_search, unsigned int index)
{
	jdpl_val* to_return = jdpl_search_arr(to_search, index);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_BOOL)
	{
		return NULL;
	}
	
	return (int*)to_return->data;
}

//obj(jdpl_obj)
jdpl_obj* jdpl_arrgetobj(jdpl_arr* to_search, unsigned int index)
{
	jdpl_val* to_return = jdpl_search_arr(to_search, index);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_OBJ)
	{
		return NULL;
	}
	
	return (jdpl_obj*)to_return->data;
}

//arr(jdpl_arr)
jdpl_arr* jdpl_arrgetarr(jdpl_arr* to_search, unsigned int index)
{
	jdpl_val* to_return = jdpl_search_arr(to_search, index);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_ARR)
	{
		return NULL;
	}
	
	return (jdpl_arr*)to_return->data;
}
