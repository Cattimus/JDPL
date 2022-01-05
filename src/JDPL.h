#pragma once
#include<stdint.h>
#include<stddef.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef enum JDPL_TYPE_ENUM
{
	JDPL_TYPE_TEXT, JDPL_TYPE_NUM, JDPL_TYPE_DECIMAL, JDPL_TYPE_BOOL, JDPL_TYPE_OBJ, JDPL_TYPE_ARR, JDPL_TYPE_NULL, JDPL_TYPE_INVALID
}JDPL_TYPE;

typedef struct JDPL_VALUE
{
	JDPL_TYPE type; //type of held data
	int data_size;  //size of held data in bytes
	void* data;     //pointer to data
	
}jdpl_val;

typedef struct JDPL_ARRAY
{
	int count;     //number of items held by array
	int max_size;
	jdpl_val* arr; //array of value objects
	
}jdpl_arr;

typedef struct JDPL_OBJECT
{
	int count;
	int max_size;
	char** key_arr;
	jdpl_val*    val_arr;
}jdpl_obj;

/***********************************************************************
 *********************MEMORY ALLOCATION FUNCTIONS***********************
 ***********************************************************************/
jdpl_obj jdpl_new_obj()
{
	jdpl_obj to_return;
	
	to_return.max_size = 10;
	to_return.count = 0;
	
	//initialize key array
	to_return.key_arr = (char**)malloc(sizeof(char*) * 10);
	memset(to_return.key_arr, 0, sizeof(char*) * 10);
	
	//initialize value array
	to_return.val_arr = (jdpl_val*)malloc(sizeof(jdpl_val) * 10);
	memset(to_return.val_arr, 0, sizeof(jdpl_val) * 10);
	
	return to_return;
}

jdpl_arr jdpl_new_arr()
{
	jdpl_arr to_return;
	
	to_return.max_size = 10;
	to_return.count = 0;
	
	//initialize array
	to_return.arr = (jdpl_val*)malloc(sizeof(jdpl_val) * 10);
	memset(to_return.arr, 0, sizeof(jdpl_val) * 10);
	
	return to_return;
}

jdpl_val jdpl_new_val(JDPL_TYPE type, void* data, int data_size)
{
	jdpl_val to_return;
	to_return.type = type;
	to_return.data_size = data_size;
	
	//initialize and copy data to value
	to_return.data = malloc(data_size);
	memcpy(to_return.data, data, data_size);
	
	return to_return;
}

/***********************************************************************
 ***********************MEMORY FREE FUNCTIONS***************************
 ***********************************************************************/
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
		return;
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

void jdpl_setobj(jdpl_obj* to_set, const char* key, jdpl_val val)
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
			char** key_temp = (char**) realloc(to_set->key_arr, sizeof(char*) * to_set->max_size + 10);
			jdpl_val* val_temp = (jdpl_val*) realloc(to_set->val_arr, sizeof(jdpl_val) * to_set->max_size + 10);
			
			//error out if realloc is unsuccessful
			if(key_temp == NULL || val_temp == NULL)
			{
				fprintf(stderr, "jdpl_setobj: Memory allocation error. Could not realloc array of size: %d.\n", to_set->max_size);
				exit(1);
			}
			
			to_set->max_size += 10;
			to_set->key_arr = key_temp;
			to_set->val_arr = val_temp;
		}
		
		//assign new key and value
		to_set->key_arr[to_set->count] = (char*)malloc(sizeof(char) * strlen(key) + 1);
		memcpy(to_set->key_arr[to_set->count], key, strlen(key) + 1);
		to_set->val_arr[to_set->count] = val;
		
		to_set->count++;
	}
}

/***********************************************************************
 *********************OBJECT GET FUNCTIONS*******************************
 ***********************************************************************/

//number(int64)
int64_t* jdpl_getobj_i(jdpl_obj* to_search, const char* key)
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

//decimal(double)
double* jdpl_getobj_f(jdpl_obj* to_search, const char* key)
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

//bool(boolean)
int* jdpl_getobj_b  (jdpl_obj* to_search, const char* key)
{
	jdpl_val* to_return = jdpl_search_obj(to_search, key);
	
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
jdpl_obj* jdpl_getobj_obj(jdpl_obj* to_search, const char* key)
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
jdpl_arr* jdpl_getobj_arr(jdpl_obj* to_search, const char* key)
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

/***********************************************************************
 *********************ARRAY GET FUNCTIONS*******************************
 ***********************************************************************/

//array retrieve functions
//number(int64)
int64_t* jdpl_getarr_i(jdpl_arr* to_search, int index)
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
double* jdpl_getarr_f64(jdpl_arr* to_search, int index)
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
int* jdpl_getarr_b  (jdpl_arr* to_search, int index)
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
jdpl_obj* jdpl_getarr_obj(jdpl_arr* to_search, int index)
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
jdpl_arr* jdpl_getarr_arr(jdpl_arr* to_search, int index)
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
