#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//TODO - parse JSON input from file
//TODO - parse JSON input from strings/memory

#define JDPL_MEMSTEP 1024
#define JDPL_MIN_SIZE 1024

typedef enum JDPL_TYPE_ENUM
{
	JDPL_TYPE_TEXT, JDPL_TYPE_NUM, JDPL_TYPE_BOOL, JDPL_TYPE_OBJ, JDPL_TYPE_ARR, JDPL_TYPE_NULL, JDPL_TYPE_INVALID
}JDPL_TYPE;

typedef struct JDPL_ARRAY  jdpl_arr;
typedef struct JDPL_OBJECT jdpl_obj;

typedef struct JDPL_VALUE
{
	JDPL_TYPE type; //type of held data
	size_t data_size;  //size of held data in bytes

	//union that will hold data
	union DATA
	{
		double num;
		char* text;
		int bool;
		jdpl_obj* obj;
		jdpl_arr* arr;
		int null;
	}data;
	
}jdpl_val;

typedef struct JDPL_ARRAY
{
	size_t count;     //number of items held by array
	size_t max_size;
	jdpl_val** arr; //array of value objects
	
}jdpl_arr;

//keypair struct for hash table
typedef struct JDPL_KEYPAIR
{
	char* key;
	jdpl_val* value;
}jdpl_keypair;

typedef struct JDPL_OBJECT
{
	size_t count;
	size_t max_size;
	jdpl_keypair** hashmap;
}jdpl_obj;

//hashing function for keys
static uint32_t hash_str(const char* key, size_t size)
{
	uint32_t hash_value = 0;
	size_t key_size = strlen(key);
	for(size_t i = 0; i < key_size; i++)
	{
		hash_value = ((hash_value * 251) + key[i]) % size;
	}
	
	return hash_value;
}

/***********************************************************************
 *********************MEMORY ALLOCATION FUNCTIONS***********************
 ***********************************************************************/

jdpl_obj* jdpl_new_obj()
{
	jdpl_obj* to_return = (jdpl_obj*)malloc(sizeof(jdpl_obj));
	
	to_return->max_size = JDPL_MEMSTEP;
	to_return->count = 0;
	
	//initialize keypair array (hash table)
	to_return->hashmap = (jdpl_keypair**)malloc(sizeof(jdpl_keypair*) * JDPL_MIN_SIZE);
	memset(to_return->hashmap, 0, sizeof(jdpl_keypair*) * JDPL_MIN_SIZE);
	
	return to_return;
}

jdpl_arr* jdpl_new_arr()
{
	jdpl_arr* to_return = (jdpl_arr*)malloc(sizeof(jdpl_arr));
	
	to_return->max_size = JDPL_MEMSTEP;
	to_return->count = 0;
	
	//initialize array
	to_return->arr = (jdpl_val**)malloc(sizeof(jdpl_val*) * JDPL_MEMSTEP);
	memset(to_return->arr, 0, sizeof(jdpl_val*) * JDPL_MEMSTEP);
	
	return to_return;
}

jdpl_val* jdpl_vali(int data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_NUM;
	to_return->data_size = sizeof(double);
	to_return->data.num = (double)data;
	return to_return;
}

jdpl_val* jdpl_vall(int64_t data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_NUM;
	to_return->data_size = sizeof(double);
	to_return->data.num = (double)data;
	
	return to_return;
}

jdpl_val* jdpl_vals(const char* data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_TEXT;
	to_return->data_size = strlen(data) + 1;
	to_return->data.text = malloc(to_return->data_size);
	memcpy(to_return->data.text, data, to_return->data_size);
	
	return to_return;
}

jdpl_val* jdpl_valf(float data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_NUM;
	to_return->data_size = sizeof(double);
	to_return->data.num = (double)data;
	
	return to_return;
}

jdpl_val* jdpl_vald(double data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_NUM;
	to_return->data_size = sizeof(double);
	to_return->data.num = data;
	
	return to_return;
}

jdpl_val* jdpl_valb(int data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_BOOL;
	to_return->data_size = sizeof(int);
	to_return->data.bool = data;

	return to_return;
}

jdpl_val* jdpl_valnull()
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = JDPL_TYPE_NULL;
	to_return->data_size = sizeof(int);
	to_return->data.null = 1;
	
	return to_return;
}

//TODO - jdpl_valobj_copy() jdpl_valarr_copy() (deep copy variants)
jdpl_val* jdpl_valobj(jdpl_obj* data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_OBJ;
	to_return->data_size = sizeof(jdpl_obj*);
	to_return->data.obj = data;
	
	return to_return;
}

jdpl_val* jdpl_valarr(jdpl_arr* data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_ARR;
	to_return->data_size = sizeof(jdpl_arr);
	to_return->data.arr = data;
	
	return to_return;
}

//create a deep copy of the selected value
jdpl_val* jdpl_val_copy(jdpl_val* to_copy)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	
	to_return->type = to_copy->type;
	to_return->data_size = to_copy->data_size;

	if(to_copy->type != JDPL_TYPE_TEXT)
	{
		to_return->data = to_copy->data;
	}
	else
	{
		to_return->data.text = malloc(to_copy->data_size);
		memcpy(to_return->data.text, to_copy->data.text, to_copy->data_size);
	}
	
	return to_return;
}

/***********************************************************************
 ***********************MEMORY FREE FUNCTIONS***************************
 ***********************************************************************/
//forward declaration (because these may be recursive)
void jdpl_free_arr(jdpl_arr*);
void jdpl_free_obj(jdpl_obj*);
void jdpl_free_val(jdpl_val*);
void jdpl_free_keypair(jdpl_keypair*);

void jdpl_free_obj(jdpl_obj* to_free)
{
	if(to_free == NULL)
	{
		return;
	}
	
	if(to_free->hashmap == NULL)
	{
		return;
	}
	
	for(int i = 0; i < to_free->max_size; i++)
	{
		if(to_free->hashmap[i] != NULL)
		{
			jdpl_free_keypair(to_free->hashmap[i]);
			to_free->hashmap[i] = NULL;
		}
	}
	
	free(to_free->hashmap);
	to_free->hashmap = NULL;
	
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
		jdpl_free_val(to_free->arr[i]);
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
	
	if(to_free->type == JDPL_TYPE_OBJ)
	{
		jdpl_free_obj(to_free->data.obj);
		to_free->data.obj = NULL;
	}
	else if(to_free->type == JDPL_TYPE_ARR)
	{
		jdpl_free_arr(to_free->data.arr);
		to_free->data.arr = NULL;
	}
	else if(to_free->type == JDPL_TYPE_TEXT)
	{
		free(to_free->data.text);
		to_free->data.text = NULL;
	}

	//set value to null
	to_free->type = JDPL_TYPE_NULL;
	to_free->data_size = sizeof(int);
	to_free->data.null = 1;
	
	free(to_free);
	to_free = NULL;
}

void jdpl_free_keypair(jdpl_keypair* to_free)
{
	if(to_free == NULL)
	{
		return;
	}
	
	if(to_free->key != NULL)
	{
		free(to_free->key);
		to_free->key = NULL;
	}
	
	if(to_free->value != NULL)
	{
		jdpl_free_val(to_free->value);
		to_free->value = NULL;
	}
	
	free(to_free);
	to_free = NULL;
}


/***********************************************************************
 ********************OBJECT SET FUNCTIONS*******************************
 ***********************************************************************/

//search a JSON object for a particular key. returns NULL if not found.
static int jdpl_search_obj(const char* key, jdpl_obj* to_search)
{
	uint32_t index = hash_str(key, to_search->max_size);
	uint32_t orig_val = index;
	while(index < to_search->max_size)
	{
		if(to_search->hashmap[index] == NULL)
		{
			return index;
		}
		else
		{
			if(strcmp(key, to_search->hashmap[index]->key) == 0)
			{
				return index;
			}
		}
		
		index += 1;
		
		if(index == to_search->max_size)
		{
			index = 0;
		}
		
		//exit condition if there are no indexes available
		if(index == orig_val)
		{
			return -1;
		}
	}
	
	//no index is available
	return -1;
}

static void jdpl_hashmap_reinsert(jdpl_obj* dest, jdpl_keypair* val)
{
	int key_val = hash_str(val->key, dest->max_size);
	while(key_val < dest->max_size)
	{
		if(dest->hashmap[key_val] == NULL)
		{
			dest->hashmap[key_val] = val;
			break;
		}
		key_val++;
		
		if(key_val == dest->max_size)
		{
			key_val = 0;
		}
	}
	
	dest->count += 1;
}

//resize hashmap
static void jdpl_resize_hashmap(jdpl_obj* src, size_t new_size)
{
	if(new_size <= src->count)
	{
		fprintf(stderr, "jdpl_resize_hashmap: new size is smaller than current item count. Resize aborted.\n");
		return;
	}
	
	if(new_size < JDPL_MIN_SIZE)
	{
		new_size = JDPL_MIN_SIZE;
	}
	
	int arr_count = 0;
	jdpl_keypair** temp_array = (jdpl_keypair**)malloc(sizeof(jdpl_keypair*) * src->max_size);
	memset(temp_array, 0, sizeof(jdpl_keypair*) * src->max_size);
	
	//load all hashes to temporary array
	for(int i = 0; i < src->max_size; i++)
	{
		if(src->hashmap[i] != NULL)
		{
			temp_array[arr_count] = src->hashmap[i];
			arr_count++;
		}
	}
	
	//resize hashmap to new size
	src->hashmap = (jdpl_keypair**)realloc(src->hashmap, sizeof(jdpl_keypair*) * new_size);
	src->count = 0;
	src->max_size = new_size;
	for(int i = 0; i < src->max_size; i++)
	{
		src->hashmap[i] = NULL;
	}
	
	//reinsert items
	for(int i = 0; i < arr_count; i++)
	{
		jdpl_hashmap_reinsert(src, temp_array[i]);
	}
	
	free(temp_array);
}

//add keypair objects directly
static void jdpl_objadd_keypair(jdpl_keypair* to_add, jdpl_obj* to_set)
{
	//resize array if it's getting too full(too full = too slow)
	if(to_set->count >= to_set->max_size - to_set->max_size / 4)
	{
		jdpl_resize_hashmap(to_set, to_set->max_size * 1.5);
	}
	
	//search for existing keys
	int index = jdpl_search_obj(to_add->key, to_set);
	
	//no room in object for new index (should never happen)
	if(index == -1)
	{
		fprintf(stderr, "jdpl_objadd: resizing error - not enough room in hashmap for new object with key %s\n", to_add->key);
		return;
	}
	
	//overwrite old index
	if(to_set->hashmap[index] != NULL)
	{
		jdpl_free_keypair(to_set->hashmap[index]);
		to_set->hashmap[index] = NULL;
	}
	
	//write new value
	to_set->hashmap[index] = to_add;
	to_set->count += 1;
	
}

//shallow copy value
void jdpl_objadd(const char* key, jdpl_val* val, jdpl_obj* to_set)
{
	//resize array if it's getting too full(too full = too slow)
	if(to_set->count >= to_set->max_size - to_set->max_size / 4)
	{
		jdpl_resize_hashmap(to_set, to_set->max_size * 1.5);
	}
	
	//search for existing keys
	int index = jdpl_search_obj(key, to_set);
	
	//no room in object for new index (should never happen)
	if(index == -1)
	{
		fprintf(stderr, "jdpl_objadd: resizing error - not enough room in hashmap for new object with key %s\n", key);
		return;
	}
	
	//initialize new keypair
	jdpl_keypair* to_write = (jdpl_keypair*)malloc(sizeof(jdpl_keypair));
	memset(to_write, 0, sizeof(jdpl_keypair));
	
	//deep copy key
	to_write->key = (char*)malloc(sizeof(char) * strlen(key) + 1);
	strcpy(to_write->key, key);
	
	//shallow copy value
	to_write->value = val;
	
	//overwrite old index
	if(to_set->hashmap[index] != NULL)
	{
		jdpl_free_keypair(to_set->hashmap[index]);
		to_set->hashmap[index] = NULL;
	}
	
	//write new value
	to_set->hashmap[index] = to_write;
	to_set->count += 1;
	
}

//deep copy value
void jdpl_objadd_copy(jdpl_obj* to_set, const char* key, jdpl_val val);

/***********************************************************************
 *********************OBJECT GET FUNCTIONS*******************************
 ***********************************************************************/

//generic search
jdpl_val* jdpl_objget( const char* key, jdpl_obj* to_search)
{
	int index = jdpl_search_obj(key, to_search);
	if(index < 0)
	{
		return NULL;
	}
	
	return to_search->hashmap[index]->value;
}

//text(string)
const char* jdpl_objgets(const char* key, jdpl_obj* to_search)
{
	int index = jdpl_search_obj(key, to_search);
	if(index < 0)
	{
		return NULL;
	}
	jdpl_val* to_return = to_search->hashmap[index]->value;
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_TEXT)
	{
		return NULL;
	}
	
	return to_return->data.text;
}

//number (always double)
double* jdpl_objgetnum(const char* key, jdpl_obj* to_search)
{
	int index = jdpl_search_obj(key, to_search);
	if(index < 0)
	{
		return NULL;
	}
	jdpl_val* to_return = to_search->hashmap[index]->value;
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_NUM)
	{
		return NULL;
	}
	
	return &to_return->data.num;
}

//bool(boolean) 1 true 0 false -1 error
int* jdpl_objgetb(const char* key, jdpl_obj* to_search)
{
	int index = jdpl_search_obj(key, to_search);
	if(index < 0)
	{
		return NULL;
	}
	jdpl_val* to_return = to_search->hashmap[index]->value;
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_BOOL)
	{
		return NULL;
	}
	
	return &to_return->data.bool;
}

//obj(jdpl_obj)
jdpl_obj* jdpl_objgetobj(const char* key, jdpl_obj* to_search)
{
	int index = jdpl_search_obj(key, to_search);
	if(index < 0)
	{
		return NULL;
	}
	jdpl_val* to_return = to_search->hashmap[index]->value;
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_OBJ)
	{
		return NULL;
	}
	
	return to_return->data.obj;
}

//arr(jdpl_arr)
jdpl_arr* jdpl_objgetarr(const char* key, jdpl_obj* to_search)
{
	int index = jdpl_search_obj(key, to_search);
	if(index < 0)
	{
		return NULL;
	}
	jdpl_val* to_return = to_search->hashmap[index]->value;
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_ARR)
	{
		return NULL;
	}
	
	return to_return->data.arr;
}


/***********************************************************************
 **********************ARRAY SET FUNCTIONS*******************************
 ***********************************************************************/

//search a JSON array for a particular index. return NULL if out of bounds.
static jdpl_val* jdpl_search_arr(jdpl_arr* to_search, int index)
{
	if(index < to_search->count && index >= 0)
	{
		return to_search->arr[index];
	}
	
	return NULL;
}

void jdpl_arradd(jdpl_val* val, jdpl_arr* to_set)
{
	//expand key and value arrays if necessary
	if(to_set->count == to_set->max_size)
	{
		jdpl_val** val_temp = (jdpl_val**)realloc(to_set->arr, sizeof(jdpl_val*) * (to_set->max_size + JDPL_MEMSTEP));
		
		//error out if realloc is unsuccessful
		if(val_temp == NULL)
		{
			fprintf(stderr, "jdpl_setobj: Memory allocation error. Could not realloc array of size: %ld.\n", to_set->max_size);
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
void jdpl_arrset(jdpl_val* val, unsigned int index, jdpl_arr* to_set)
{
	//index is out of bounds
	if(to_set->count < index)
	{
		return;
	}
	
	jdpl_free_val(to_set->arr[index]);
	to_set->arr[index] = val;
}

/***********************************************************************
 **********************ARRAY GET FUNCTIONS*******************************
 ***********************************************************************/

//generic search
jdpl_val* jdpl_arrget(unsigned int index, jdpl_arr* to_search)
{
	return jdpl_search_arr(to_search, index);
}

double* jdpl_arrgetnum(unsigned int index, jdpl_arr* to_search)
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
	
	return &to_return->data.num;
}

//bool(boolean)
int* jdpl_arrgetb(unsigned int index, jdpl_arr* to_search)
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
	
	return &to_return->data.bool;
}

//obj(jdpl_obj)
jdpl_obj* jdpl_arrgetobj(unsigned int index, jdpl_arr* to_search)
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
	
	return to_return->data.obj;
}

//arr(jdpl_arr)
jdpl_arr* jdpl_arrgetarr(unsigned int index, jdpl_arr* to_search)
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
	
	return to_return->data.arr;
}

/*************************************************************************
 ***************************TO_STR FUNCTIONS******************************
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
		{
			to_return = (char*)malloc(6);
			if(to_convert->data.bool)
			{
				strcpy(to_return, "true");
			}
			else
			{
				strcpy(to_return, "false");
			}
			break;
		}
			
		case JDPL_TYPE_NUM:
		{
			to_return = (char*)malloc(24);
			double num = to_convert->data.num;
			if(num - (int64_t)num == 0)
			{
				sprintf(to_return, "%ld", (int64_t)num);
			}
			else
			{
				sprintf(to_return, "%lf", num);
			}
		}
			break;
			
		case JDPL_TYPE_TEXT:
		{
			to_return = (char*)malloc(to_convert->data_size + 2);
			to_return[0] = '"';
			memcpy(to_return + 1, to_convert->data.text, to_convert->data_size);
			to_return[to_convert->data_size] = '"';
			to_return[to_convert->data_size + 1] = '\0';
			break;
		}
			
		case JDPL_TYPE_OBJ:
		{
			to_return = jdpl_obj_tostr(to_convert->data.obj);
			break;
		}
			
		case JDPL_TYPE_ARR:
		{
			to_return = jdpl_arr_tostr(to_convert->data.arr);
			break;
		}
			
		case JDPL_TYPE_NULL:
		{
			to_return = (char*)malloc(5);
			strcpy(to_return, "null");
			break;
		}
			
		case JDPL_TYPE_INVALID:
			break;
	}
	
	return to_return;
}

//convert keypair to string
static char* jdpl_keypair_tostr(jdpl_keypair* to_convert)
{
	char* value_str = jdpl_val_tostr(to_convert->value);
	size_t key_size = strlen(to_convert->key);
	size_t value_size = strlen(value_str);
	int str_size = key_size + value_size + 5;
	
	char* to_return = (char*)malloc(str_size);
	
	to_return[0] = '"';
	memcpy(to_return + 1, to_convert->key, key_size);
	to_return[key_size + 1] = '"';
	to_return[key_size + 2] = ':';
	to_return[key_size + 3] = ' ';
	memcpy(to_return+key_size+4, value_str, value_size);
	to_return[str_size - 1] = '\0';
	
	free(value_str);
	return to_return;
}


//convert object to flattened string {obj}
char* jdpl_obj_tostr(jdpl_obj* to_convert)
{
	int str_size = 3;
	int str_index = 1;
	int read_objects = 0;
	char* to_return = (char*)malloc(str_size);
	memset(to_return, 0, 3);
	
	to_return[0] = '{';
	for(int i = 0; i < to_convert->max_size; i++)
	{
		jdpl_keypair* keypair = to_convert->hashmap[i];
		if(keypair == NULL)
		{
			continue;
		}
		
		int expand_val = 1;
		if(read_objects == to_convert->count - 1)
		{
			expand_val -= 1;
		}
		
		char* keypair_str = jdpl_keypair_tostr(keypair);
		size_t chunk_len = strlen(keypair_str);
		int starting_index = str_index;
		str_size += chunk_len + expand_val;
		
		char* temp = (char*)realloc(to_return, str_size);
		if(temp == NULL)
		{
			fprintf(stderr, "jdpl_obj_tostr: Memory allocation error. Could not realloc array of size: %d.\n", str_size);
			exit(1);
		}
		to_return = temp;
		
		memcpy(to_return + starting_index, keypair_str, chunk_len);
		str_index += chunk_len;
		
		if(read_objects != to_convert->count - 1)
		{
			to_return[str_index] = ',';
			str_index++;
		}
		
		read_objects++;
		free(keypair_str);
	}
	to_return[str_size - 2] = '}';
	to_return[str_size - 1] = '\0';
	
	return to_return;
}

//convert array to flattened string [arr]
char* jdpl_arr_tostr(jdpl_arr* to_convert)
{
	int str_size = 3;
	int str_index = 1;
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
		char* val_str = jdpl_val_tostr(to_convert->arr[i]);
		size_t val_size = strlen(val_str);
		str_size += val_size + expand_val;
		
		char* temp = (char*)realloc(to_return, str_size);
		if(temp == NULL)
		{
			fprintf(stderr, "jdpl_obj_tostr: Memory allocation error. Could not realloc array of size: %d.\n", str_size);
			exit(1);
		}
		to_return = temp;
		
		//add new data
		memcpy(to_return + str_index, val_str, val_size);
		str_index += val_size;
		
		if(i != to_convert->count - 1)
		{
			to_return[str_index] = ',';
			str_index++;
		}
		free(val_str);
	}
	to_return[str_size-2] = ']';
	to_return[str_size-1] = '\0';
	
	return to_return;
}

//takes condensed input and prettifies it to human-readable json
void jdpl_prettify(char** to_prettify, unsigned int indent_size)
{
	int index = 0;
	size_t str_max = (strlen(*to_prettify) + 1) * 1.5;
	char* str = (char*)malloc(str_max);
	char* src_str = *to_prettify;
	size_t src_len = strlen(src_str);
	
	int indent_level = 0;
	int in_quote = 0;
	for(size_t i = 0; i < src_len; i++)
	{
		if(i > 0)
		{
			if(src_str[i] == '"' && src_str[i-1] != '\\')
			{
				in_quote = !in_quote;
			}
		}
		
		if(!in_quote)
		{
			if(src_str[i] == '{' && src_str[i+1] != '}')
			{
				str[index] = '{';
				str[index+1] = '\n';
				index += 2;
				indent_level++;
				
				for(int x = 0; x < indent_level * indent_size; x++)
				{
					str[index] = ' ';
					index++;
				}
			}
			
			else if(src_str[i] == '}' && src_str[i-1] != '{')
			{
				indent_level--;
				str[index] = '\n';
				index++;
				
				for(int x = 0; x < indent_level * indent_size; x++)
				{
					str[index] = ' ';
					index++;
				}
				
				str[index] = '}';
				index++;
				if(indent_level == 0)
				{
					str[index] = '\n';
				}
			}
			
			else if(src_str[i] == '[' && src_str[i+1] != ']')
			{
				str[index] = '[';
				str[index+1] = '\n';
				index += 2;
				indent_level++;
				
				for(int x = 0; x < indent_level * indent_size; x++)
				{
					str[index] = ' ';
					index++;
				}
			}
			
			else if(src_str[i] == ']' && src_str[i-1] != '[')
			{
				indent_level--;
				str[index] = '\n';
				index++;
				
				for(int x = 0; x < indent_level * indent_size; x++)
				{
					str[index] = ' ';
					index++;
				}
				
				str[index] = ']';
				index++;
				if(indent_level == 0)
				{
					str[index] = '\n';
				}
			}
			
			else if(src_str[i] == ',')
			{
				str[index] = ',';
				str[index+1] = '\n';
				index += 2;
				
				for(int x = 0; x < indent_level * indent_size; x++)
				{
					str[index] = ' ';
					index++;
				}
			}
			else
			{
				str[index] = src_str[i];
				index++;
			}
		}
		else
		{
			str[index] = src_str[i];
			index++;
		}
		
		if(index >= str_max * 0.75)
		{
			str = (char*)realloc(str, str_max * 1.5);
			str_max = str_max * 1.5;
		}
	}
	
	str = (char*)realloc(str, index+1);
	str[index] = '\0';
	
	free(*to_prettify);
	*to_prettify = str;
}

/*************************************************************************
 ****************************PARSING FUNCTIONS****************************
 *************************************************************************/
jdpl_obj* jdpl_obj_fromstr(const char* str);
jdpl_arr* jdpl_arr_fromstr(const char* str);

static jdpl_val* parse_value(const char* str, int len)
{
	//flags for finding out which data type we have and content length
	int is_str    = 0; //flag and also length counter
	int is_obj    = 0; //flag and also offset value
	int is_arr    = 0; //flag and also offset value
	int is_number = 0; //flag and also length counter
	
	//flags for counting whitespace preceeding content
	int count_spaces = 1;
	int whitespace_counter = 0;
	
	//determine type and leading whitespace
	for(size_t i = 0; i < len; i++)
	{
		const char* cur = (str+i);
		if(isspace(*cur) && count_spaces)
		{
			whitespace_counter++;
			continue;
		}
		
		//data is str
		else if(*cur == '"')
		{
			is_str = 2;
			is_number = 0;
			i++;
			
			//determine length of string
			while(str[i] != '"' && i < len)
			{
				is_str++;
				i++;
			}
			break;
		}
		
		//data is json object
		else if(*cur == '{')
		{
			is_obj = i;
			break;
		}
		else if(*cur == '[')
		{
			is_arr = i;
			break;
		}
		else if(isdigit(*cur) || *cur == '.')
		{
			count_spaces = 0;
			is_number++;
		}
	}
	
	if(is_str)
	{
		//copy substr for value
		size_t data_size = is_str - 2;
		char* value = (char*)malloc(data_size + 1);
		memcpy(value, str+1+whitespace_counter, data_size);
		value[data_size] = '\0';
		
		//return new value
		jdpl_val* to_return = jdpl_vals(value);
		free(value);
		return to_return;
	}
	else if(is_number)
	{
		//copy substr for value
		size_t data_size = is_number;
		char* value = (char*)malloc(data_size + 1);
		memcpy(value, str+whitespace_counter, data_size);
		value[data_size] = '\0';
		
		//return new value
		jdpl_val* to_return = jdpl_vald(atof(value));
		free(value);
		return to_return;
	}
	else if(is_obj)
	{
		return jdpl_valobj(jdpl_obj_fromstr(str+is_obj));
	}
	else if(is_arr)
	{
		return jdpl_valarr(jdpl_arr_fromstr(str+is_arr));
	}
	
	
	//parse for boolean or null values
	else
	{
		if(len >= 5 + whitespace_counter)
		{
			return jdpl_valnull();
		}
		
		const char* cur = str+whitespace_counter;
		if(strncmp(cur, "true", 4) == 0)
		{
			return jdpl_valb(1);
		}
		else if(strncmp(cur, "null", 4) == 0)
		{
			return jdpl_valnull();
		}
		else if(strncmp(cur, "false", 5) == 0)
		{
			return jdpl_valb(0);
		}
	}
	
	//error case where value cannot be parsed correctly. This shouldn't destroy the overall structure.
	return jdpl_valnull();
}


static jdpl_keypair* parse_keypair(const char* str)
{
	jdpl_keypair* to_return = (jdpl_keypair*)malloc(sizeof(jdpl_keypair));
	to_return->key = NULL;
	to_return->value = NULL;
	
	size_t len = strlen(str);
	int in_quote = 0;
	int obj_indentation = 0;
	int arr_indentation = 0;
	int separator_index = -1;
	
	int key_set = 0;
	int key_start = 0;
	int key_end = 0;
	
	int data_start = 0;
	int data_end = 0;
	
	for(size_t i = 0; i < len; i++)
	{
		if(i > 0)
		{
			if(str[i] == '"' && str[i-1] != '\\')
			{
				in_quote = !in_quote;
				
				//find key
				if(separator_index < 0 && !key_set)
				{
					key_start = i;
					key_set = 1;
				}
				else if (separator_index < 0 && key_set)
				{
					key_end = i;
				}
			}
		}
		else
		{
			if(str[i] == '"')
			{
				in_quote = !in_quote;
				
				//find key
				if(separator_index < 0 && !key_set)
				{
					key_start = i;
					key_set = 1;
				}
				else if (separator_index < 0 && key_set)
				{
					key_end = i;
				}
			}
		}
		
		if(!in_quote)
		{
			if(str[i] == ':')
			{
				separator_index = i;
				data_start = separator_index + 1;
			}
			
			//string number etc
			else if(str[i] == ',')
			{
				data_end = i-1;
				break;
			}
			
			//object
			else if(str[i] == '{')
			{
				data_start = i;
				obj_indentation++;
			}
			else if(str[i] == '}')
			{
				//end of json object
				if(obj_indentation == 0)
				{
					data_end = i;
					break;
				}
				
				obj_indentation--;
				if(obj_indentation == 0 && arr_indentation == 0)
				{
					data_end = i;
					break;
				}
			}
			
			//array
			else if(str[i] == '[')
			{
				data_start = i;
				arr_indentation++;
			}
			
			else if(str[i] == ']')
			{
				//end of json array
				if(arr_indentation == 0)
				{
					data_end = i;
					break;
				}
				
				arr_indentation--;
				if(obj_indentation == 0 && arr_indentation == 0)
				{
					data_end = i;
					break;
				}
			}
			
			else if(!isspace(str[i]))
			{
				data_end++;
			}
		}
		else
		{
			data_end++;
		}
		
	}
	
	//make copy of key
	int key_len = key_end - key_start - 1;
	to_return->key = (char*)malloc(key_len + 1);
	to_return->key[key_len] = '\0';
	memcpy(to_return->key, str + key_start + 1, key_len);
	
	//parse value
	to_return->value = parse_value(str+data_start, data_end - data_start);
	
	return to_return;
}

jdpl_obj* jdpl_obj_fromstr(const char* str)
{
	jdpl_obj* to_return = jdpl_new_obj();
	
	size_t len = strlen(str);
	if(str[0] != '{')
	{
		return NULL;
	}
	
	for(size_t i = 0; i < len; i++)
	{
		if(i > 0)
		{
			if(str[i-1] == '{' || str[i-1] == ',')
			{
				jdpl_keypair* to_add = parse_keypair(str + i);
				jdpl_objadd_keypair(to_add, to_return);
			}
		}
	}
	
	return to_return;
}


jdpl_arr* jdpl_arr_fromstr(const char* str)
{
	jdpl_arr* to_return = jdpl_new_arr();
	
	size_t len = strlen(str);
	if(str[0] != '[')
	{
		return NULL;
	}
	
	size_t val_start = 0;
	size_t val_end = 0;
	//int indentation_level = 0;
	for(size_t i = 0; i < len; i++)
	{
		if(i > 0)
		{
			if(str[i-1] == '[' || str[i-1] == ',')
			{
				val_start = i;
			}
			else if(str[i] == ']' || str[i] == ',')
			{
				val_end = i-1;
			}
			
			jdpl_val* to_add = parse_value(str + val_start, val_end - val_start);
			jdpl_arradd(to_add, to_return);
		}
	}
	
	return to_return;
}

jdpl_obj* jdpl_obj_fromfile(const char* filename)
{
	jdpl_obj* to_return = jdpl_new_obj();
	
	return to_return;
}

jdpl_arr* jdpl_arr_fromfile(const char* str)
{
	jdpl_arr* to_return = jdpl_new_arr();
	
	return to_return;
}

