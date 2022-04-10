#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define JDPL_MEMSTEP 1024
#define JDPL_MIN_SIZE 1024

//color for text
#define __JDPL__COLOR_GREEN "\033[0;32m"
#define __JDPL__COLOR_RED   "\033[0;31m"
#define __JDPL__COLOR_RESET "\033[0m"

/***********************************************************************
 ***************FORWARD DECLARATIONS OF ALL TYPES***********************
 ***********************************************************************/

typedef struct JDPL_VALUE     jdpl_val;
typedef struct JDPL_ARRAY     jdpl_arr;
typedef struct JDPL_OBJECT    jdpl_obj;
typedef struct JDPL_KEYPAIR   jdpl_keypair;

/***********************************************************************
 ***************FORWARD DECLARATIONS OF ALL FUNCTIONS*******************
 ***********************************************************************/

//new object functions
jdpl_obj* jdpl_new_obj();
jdpl_arr* jdpl_new_arr();

//value creation functions
jdpl_val* jdpl_valnull();
jdpl_val* jdpl_vali(int);
jdpl_val* jdpl_valb(int);
jdpl_val* jdpl_valf(float);
jdpl_val* jdpl_vald(double);
jdpl_val* jdpl_vall(int64_t);
jdpl_val* jdpl_vals(const char*);

//copy functions
static size_t    __jdpl__hash_str(const char*, size_t);
static jdpl_obj* __jdpl__copy_obj(jdpl_obj*);
static jdpl_arr* __jdpl__copy_arr(jdpl_arr*);
       jdpl_val* jdpl_valobj(jdpl_obj*);
       jdpl_val* jdpl_valobj_copy(jdpl_obj*);
       jdpl_val* jdpl_valarr(jdpl_arr*);
       jdpl_val* jdpl_val_copy(jdpl_val*);
static jdpl_keypair* __jdpl__copy_keypair(jdpl_keypair*);

//memory free functions
void jdpl_free_obj(jdpl_obj*);
void jdpl_free_arr(jdpl_arr*);
void jdpl_free_val(jdpl_val*);
void jdpl_free_keypair(jdpl_keypair*);

//obj set functions
static int  __jdpl__search_obj(const char*, jdpl_obj*);
static void __jdpl__hashmap_reinsert(jdpl_obj*, jdpl_keypair*);
static void __jdpl__resize_hashmap(jdpl_obj*, size_t);
static void __jdpl__objadd_keypair(jdpl_keypair*, jdpl_obj*);
       void jdpl_objadd(const char*, jdpl_val*, jdpl_obj*);
       void jdpl_objadd_copy(const char*, jdpl_val*, jdpl_obj*);

//obj get functions
      int*      jdpl_objgetb(const char*, jdpl_obj*);
const char*     jdpl_objgets(const char*, jdpl_obj*);
      double*   jdpl_objgetnum(const char*, jdpl_obj*);
      jdpl_val* jdpl_objget(const char*, jdpl_obj*);
      jdpl_obj* jdpl_objgetobj(const char*, jdpl_obj*);
      jdpl_arr* jdpl_objgetarr(const char*, jdpl_obj*);

//arr set functions
       void      jdpl_arradd(jdpl_val*, jdpl_arr*);
       void      jdpl_arrset(jdpl_val*, unsigned int, jdpl_arr*);
       void      jdpl_arradd_copy(jdpl_val*, jdpl_arr*);
       void      jdpl_arrset_copy(jdpl_val*, unsigned int, jdpl_arr*);
static jdpl_val* __jdpl__search_arr(jdpl_arr*, unsigned int);

//array get functions
int*      jdpl_arrgetb(unsigned int, jdpl_arr*);
double*   jdpl_arrgetnum(unsigned int, jdpl_arr*);
jdpl_val* jdpl_arrget(unsigned int, jdpl_arr*);
jdpl_obj* jdpl_arrgetobj(unsigned int, jdpl_arr*);
jdpl_arr* jdpl_arrgetarr(unsigned int, jdpl_arr*);

//to string functions
       char* jdpl_val_tostr(jdpl_val*);
static char* __jdpl__keypair_tostr(jdpl_keypair*);
       char* jdpl_obj_tostr(jdpl_obj*);
       char* jdpl_arr_tostr(jdpl_arr*);
       void  jdpl_prettify(char**, unsigned int);

//parsing functions
static char*         __jdpl__read_file(const char*);
static jdpl_val*     __jdpl__parse_value(const char*, unsigned int);
       jdpl_obj*     jdpl_obj_fromstr(const char*);
       jdpl_arr*     jdpl_arr_fromstr(const char*);
       jdpl_obj*     jdpl_obj_fromfile(const char*);
       jdpl_arr*     jdpl_arr_fromfile(const char*);
static jdpl_keypair* __jdpl__parse_keypair(const char*);


typedef enum JDPL_TYPE_ENUM
{
	JDPL_TYPE_TEXT = 0, JDPL_TYPE_NUM, JDPL_TYPE_BOOL, JDPL_TYPE_OBJ, JDPL_TYPE_ARR, JDPL_TYPE_NULL, JDPL_TYPE_INVALID
}JDPL_TYPE;

typedef struct JDPL_VALUE
{
	JDPL_TYPE type;    //type of held data
	size_t data_size;  //size of held data in bytes

	//union that will hold actual data
	union DATA
	{
		double num;
		char* text;
		int boolean;
		jdpl_obj* obj;
		jdpl_arr* arr;
		int null;
	}data;
	
}jdpl_val;

typedef struct JDPL_ARRAY
{
	size_t count;     //number of items held by array
	size_t max_size;
	jdpl_val** arr;   //array of value objects
	
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
static size_t __jdpl__hash_str(const char* key, size_t size)
{
	//this is inefficient but at least shouldn't crash
	if(key == NULL)
	{
		return 0;
	}
	
	size_t hash_value = 0;
	size_t key_size = strlen(key);
	for(size_t i = 0; i < key_size; i++)
	{
		hash_value = ((hash_value * 251) + (size_t)key[i]) % size;
	}
	
	return hash_value;
}

/***********************************************************************
 *********************MEMORY ALLOCATION FUNCTIONS***********************
 ***********************************************************************/

//create new blank json object
jdpl_obj* jdpl_new_obj()
{
	jdpl_obj* to_return = (jdpl_obj*)malloc(sizeof(jdpl_obj));
	
	to_return->max_size = JDPL_MEMSTEP;
	to_return->count = 0;
	
	//initialize keypair array (hash table)
	to_return->hashmap = (jdpl_keypair**)calloc(sizeof(jdpl_keypair*) * JDPL_MIN_SIZE, 1);
	
	return to_return;
}

//create new blank json array
jdpl_arr* jdpl_new_arr()
{
	jdpl_arr* to_return = (jdpl_arr*)malloc(sizeof(jdpl_arr));
	
	to_return->max_size = JDPL_MEMSTEP;
	to_return->count = 0;
	
	//initialize array
	to_return->arr = (jdpl_val**)calloc(sizeof(jdpl_val*) * JDPL_MEMSTEP, 1);	
	return to_return;
}

//create value from integer
jdpl_val* jdpl_vali(int data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_NUM;
	to_return->data_size = sizeof(double);
	to_return->data.num = (double)data;
	return to_return;
}

//create value from 64 bit integer
jdpl_val* jdpl_vall(int64_t data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_NUM;
	to_return->data_size = sizeof(double);
	to_return->data.num = (double)data;
	
	return to_return;
}

//create value from string
jdpl_val* jdpl_vals(const char* data)
{
	if(data == NULL)
	{
		return NULL;
	}
	
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_TEXT;
	to_return->data_size = strlen(data) + 1;
	to_return->data.text = (char*)malloc(to_return->data_size);
	memcpy(to_return->data.text, data, to_return->data_size);
	
	return to_return;
}

//create value from float
jdpl_val* jdpl_valf(float data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_NUM;
	to_return->data_size = sizeof(double);
	to_return->data.num = (double)data;
	
	return to_return;
}

//create value from double
jdpl_val* jdpl_vald(double data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_NUM;
	to_return->data_size = sizeof(double);
	to_return->data.num = data;
	
	return to_return;
}

//create value from boolean
jdpl_val* jdpl_valb(int data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_BOOL;
	to_return->data_size = sizeof(int);
	to_return->data.boolean = data;

	return to_return;
}

//create null value
jdpl_val* jdpl_valnull()
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = JDPL_TYPE_NULL;
	to_return->data_size = sizeof(int);
	to_return->data.null = 1;
	
	return to_return;
}

static jdpl_keypair* __jdpl__copy_keypair(jdpl_keypair* data)
{
	if(data == NULL)
	{
		return NULL;
	}
	
	jdpl_keypair* to_return = (jdpl_keypair*)malloc(sizeof(jdpl_keypair));

	//copy key
	to_return->key = (char*)malloc(sizeof(char) * strlen(data->key) + 1);
	strcpy(to_return->key, data->key);

	//copy data
	to_return->value = jdpl_val_copy(data->value);
	
	return to_return;
}

//deep copy a jdpl_obj to a new pointer
static jdpl_obj* __jdpl__copy_obj(jdpl_obj* data)
{
	if(data == NULL)
	{
		return NULL;
	}
	
	jdpl_obj* to_return = jdpl_new_obj();
	to_return->count = data->count;
	to_return->max_size = data->max_size;

	
	//resize buffer to the required size
	to_return->hashmap = (jdpl_keypair**)realloc(to_return->hashmap, sizeof(jdpl_keypair*) * to_return->max_size);
	if(!to_return->hashmap)
	{
		fprintf(stderr, __JDPL__COLOR_RED "__jdpl__copy_obj: Failed to realloc deep copy buffer\n" __JDPL__COLOR_RESET);
		exit(1);
	}

	//individually copy keypairs
	for(unsigned int i = 0; i < data->max_size; i++)
	{
		to_return->hashmap[i] = __jdpl__copy_keypair(data->hashmap[i]);
	}

	return to_return;
}

//deep copy a jdpl_arr to a new pointer
static jdpl_arr* __jdpl__copy_arr(jdpl_arr* data)
{
	if(data == NULL)
	{
		return NULL;
	}
	
	jdpl_arr* to_return = jdpl_new_arr();
	to_return->max_size = data->max_size;
	to_return->count = data->count;

	//resize buffer to the required size
	to_return->arr = (jdpl_val**)realloc(to_return->arr, sizeof(jdpl_val*) * to_return->max_size);
	if(!to_return->arr)
	{
		fprintf(stderr, __JDPL__COLOR_RED "__jdpl__copy_arr: Failed to realloc deep copy buffer\n" __JDPL__COLOR_RESET);
		exit(1);
	}

	for(unsigned int i = 0; i < data->max_size; i++)
	{
		to_return->arr[i] = jdpl_val_copy(data->arr[i]);
	}

	return to_return;
}

//shallow copy json object
jdpl_val* jdpl_valobj(jdpl_obj* data)
{
	if(data == NULL)
	{
		return NULL;
	}
	
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_OBJ;
	to_return->data_size = sizeof(jdpl_obj*);
	to_return->data.obj = data;
	
	return to_return;
}

//deep copy json object
jdpl_val* jdpl_valobj_copy(jdpl_obj* data)
{
	if(data == NULL)
	{
		return NULL;
	}
	
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_OBJ;
	to_return->data_size = sizeof(jdpl_obj*);
	to_return->data.obj = __jdpl__copy_obj(data);
	
	return to_return;
}

//shallow copy json array
jdpl_val* jdpl_valarr(jdpl_arr* data)
{
	if(data == NULL)
	{
		return NULL;
	}
	
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_ARR;
	to_return->data_size = sizeof(jdpl_arr);
	to_return->data.arr = data;
	
	return to_return;
}

//deep copy json array
jdpl_val* jdpl_valarr_copy(jdpl_arr* data)
{
	if(data == NULL)
	{
		return NULL;
	}
	
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));

	to_return->type = JDPL_TYPE_ARR;
	to_return->data_size = sizeof(jdpl_arr);
	to_return->data.arr = __jdpl__copy_arr(data);
	
	return to_return;
}

//create a deep copy of the selected value
jdpl_val* jdpl_val_copy(jdpl_val* to_copy)
{
	//this makes copying easier and less prone to errors
	if(to_copy == NULL)
	{
		return NULL;
	}
	
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = to_copy->type;
	to_return->data_size = to_copy->data_size;

	//deep copy text
	if(to_copy->type == JDPL_TYPE_TEXT)
	{
		to_return->data.text = (char*)malloc(to_copy->data_size);
		memcpy(to_return->data.text, to_copy->data.text, to_copy->data_size);
	}

	//deep copy obj
	else if(to_copy->type == JDPL_TYPE_OBJ)
	{
		to_return->data.obj = __jdpl__copy_obj(to_copy->data.obj);
	}

	//deep copy array
	else if(to_copy->type == JDPL_TYPE_ARR)
	{
		to_return->data.arr = __jdpl__copy_arr(to_copy->data.arr);
	}

	//copy any other type (this can be handled by C)
	else
	{
		to_return->data = to_copy->data;
	}
	return to_return;
}

/***********************************************************************
 ***********************MEMORY FREE FUNCTIONS***************************
 ***********************************************************************/

//free JDPL object
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

	//free all sub-objects
	for(unsigned int i = 0; i < to_free->max_size; i++)
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
}

//free jdpl array
void jdpl_free_arr(jdpl_arr* to_free)
{
	if(to_free == NULL)
	{
		return;
	}

	//free all sub-objects
	for(unsigned int i = 0; i < to_free->count; i++)
	{
		jdpl_free_val(to_free->arr[i]);
	}

	//free the actual array
	if(to_free->arr != NULL)
	{
		free(to_free->arr);
		to_free->arr = NULL;
	}
	
	free(to_free);
}

//free jdpl value
void jdpl_free_val(jdpl_val* to_free)
{
	if(to_free == NULL)
	{
		return;
	}

	//call free on object
	if(to_free->type == JDPL_TYPE_OBJ)
	{
		jdpl_free_obj(to_free->data.obj);
		to_free->data.obj = NULL;
	}

	//call free on array
	else if(to_free->type == JDPL_TYPE_ARR)
	{
		jdpl_free_arr(to_free->data.arr);
		to_free->data.arr = NULL;
	}

	//call free on text
	else if(to_free->type == JDPL_TYPE_TEXT)
	{
		free(to_free->data.text);
		to_free->data.text = NULL;
	}

	//set value to null (in case it is used after free)
	to_free->type = JDPL_TYPE_NULL;
	to_free->data_size = sizeof(int);
	to_free->data.null = 1;
	
	free(to_free);
}

//free keypair structure
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
}


/***********************************************************************
 ********************OBJECT SET FUNCTIONS*******************************
 ***********************************************************************/

//search a JSON object for a particular key. returns NULL if not found.
static int __jdpl__search_obj(const char* key, jdpl_obj* to_search)
{
	size_t index = __jdpl__hash_str(key, to_search->max_size);
	size_t orig_val = index;
	while(index < to_search->max_size)
	{
		//new empty value is found
		if(to_search->hashmap[index] == NULL)
		{
			return index;
		}
		else
		{
			//check to make sure we're not wasting time strcmp-ing
			if(key[0] == to_search->hashmap[index]->key[0])
			{
				//occupied slot is found and will be overwritten
				if(strcmp(key, to_search->hashmap[index]->key) == 0)
				{
					return index;
				}
			}
		}
		
		index += 1;

		//wrap around if we reach the end of the array
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

//special helper function to reinsert keypairs upon array resize
static void __jdpl__hashmap_reinsert(jdpl_obj* dest, jdpl_keypair* val)
{
	unsigned int key_val = __jdpl__hash_str(val->key, dest->max_size);
	while(key_val < dest->max_size)
	{
		//new empty slot is found
		if(dest->hashmap[key_val] == NULL)
		{
			dest->hashmap[key_val] = val;
			break;
		}
		key_val++;

		//wrap around
		if(key_val == dest->max_size)
		{
			key_val = 0;
		}
	}

	//keep track of the total number of items
	dest->count += 1;
}

//function for resizing the hashmap, theoretically should work in both directions
static void __jdpl__resize_hashmap(jdpl_obj* src, size_t new_size)
{
	//this is an error condition because the items cannot fit in the new array
	if(new_size <= src->count)
	{
		fprintf(stderr, __JDPL__COLOR_RED "__jdpl__resize_hashmap: new size is smaller than current item count. Resize aborted.\n" __JDPL__COLOR_RESET);
		return;
	}

	//make sure we don't go under the minimum size for the array
	if(new_size < JDPL_MIN_SIZE)
	{
		new_size = JDPL_MIN_SIZE;
	}
	
	int arr_count = 0;
	jdpl_keypair** temp_array = (jdpl_keypair**)calloc(sizeof(jdpl_keypair*) * src->max_size, 1);
	
	//load all hashes to temporary array
	for(unsigned int i = 0; i < src->max_size; i++)
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
	for(unsigned int i = 0; i < src->max_size; i++)
	{
		src->hashmap[i] = NULL;
	}
	
	//reinsert items
	for(int i = 0; i < arr_count; i++)
	{
		__jdpl__hashmap_reinsert(src, temp_array[i]);
	}

	//free the temporary storage array
	free(temp_array);
}

//helper function to add a keypair that has been parsed to a jdpl_obj
static void __jdpl__objadd_keypair(jdpl_keypair* to_add, jdpl_obj* to_set)
{
	//resize array if it's getting too full(too full = too slow)
	if(to_set->count >= to_set->max_size - to_set->max_size / 4)
	{
		__jdpl__resize_hashmap(to_set, to_set->max_size * 1.5);
	}
	
	//search for existing keys
	int index = __jdpl__search_obj(to_add->key, to_set);
	
	//no room in object for new index (should never happen)
	if(index == -1)
	{
		fprintf(stderr, __JDPL__COLOR_RED "__jdpl__objadd_keypair: resizing error - not enough room in hashmap for new object with key %s\n" __JDPL__COLOR_RESET, to_add->key);
		return;
	}
	
	//free old value if the index is filled
	if(to_set->hashmap[index] != NULL)
	{
		jdpl_free_keypair(to_set->hashmap[index]);
		to_set->hashmap[index] = NULL;
	}
	
	//write new value to index
	to_set->hashmap[index] = to_add;
	to_set->count += 1;
	
}

//add a new keypair to object via shallow copy value
void jdpl_objadd(const char* key, jdpl_val* val, jdpl_obj* to_set)
{	
	if(to_set == NULL || key == NULL || val == NULL)
	{
		return;
	}

	//check for a recursive addition
	if(val->type == JDPL_TYPE_OBJ)
	{
		if(val->data.obj == to_set)
		{
			return;
		}
	}
	
	//resize array if it's getting too full(too full = too slow)
	if(to_set->count >= to_set->max_size - to_set->max_size / 4)
	{
		__jdpl__resize_hashmap(to_set, to_set->max_size * 1.5);
	}
	
	int index = __jdpl__search_obj(key, to_set);
	
	//no room in object for new index (should never happen)
	if(index == -1)
	{
		fprintf(stderr, __JDPL__COLOR_RED "jdpl_objadd: resizing error - not enough room in hashmap for new object with key %s\n" __JDPL__COLOR_RESET, key);
		return;
	}
	
	//initialize new keypair
	jdpl_keypair* to_write = (jdpl_keypair*)calloc(sizeof(jdpl_keypair), 1);
	
	//deep copy key
	to_write->key = (char*)malloc(sizeof(char) * strlen(key) + 1);
	strcpy(to_write->key, key);
	
	//shallow copy value
	to_write->value = val;
	
	//free the old index (if it's filled)
	if(to_set->hashmap[index] != NULL)
	{
		jdpl_free_keypair(to_set->hashmap[index]);
		to_set->hashmap[index] = NULL;
	}
	
	//write new value to index
	to_set->hashmap[index] = to_write;
	to_set->count += 1;
}

//add a new keypair to object via deep copy value
void jdpl_objadd_copy(const char* key, jdpl_val* val, jdpl_obj* to_set)
{
	if(to_set == NULL || key == NULL || val == NULL)
	{
		return;
	}

	//check for a recursive addition
	if(val->type == JDPL_TYPE_OBJ)
	{
		if(val->data.obj == to_set)
		{
			return;
		}
	}
	
    //resize array if it's getting too full(too full = too slow)
	if(to_set->count >= to_set->max_size - to_set->max_size / 4)
	{
		__jdpl__resize_hashmap(to_set, to_set->max_size * 1.5);
	}
	
	int index = __jdpl__search_obj(key, to_set);
	
	//no room in object for new index (should never happen)
	if(index == -1)
	{
		fprintf(stderr, __JDPL__COLOR_RED "jdpl_objadd_copy: resizing error - not enough room in hashmap for new object with key %s\n" __JDPL__COLOR_RESET, key);
		return;
	}
	
	//initialize new keypair
	jdpl_keypair* to_write = (jdpl_keypair*)calloc(sizeof(jdpl_keypair), 1);
	
	//deep copy key
	to_write->key = (char*)malloc(sizeof(char) * strlen(key) + 1);
	strcpy(to_write->key, key);
	
	//deep copy value
	to_write->value = jdpl_val_copy(val);
	
	//free the old index (if it's filled)
	if(to_set->hashmap[index] != NULL)
	{
		jdpl_free_keypair(to_set->hashmap[index]);
		to_set->hashmap[index] = NULL;
	}
	
	//write new value to index
	to_set->hashmap[index] = to_write;
	to_set->count += 1;
}

/***********************************************************************
 *********************OBJECT GET FUNCTIONS*******************************
 ***********************************************************************/

//generic search for jdpl_obj
jdpl_val* jdpl_objget(const char* key, jdpl_obj* to_search)
{
	if(key == NULL || to_search == NULL)
	{
		return NULL;
	}
	
	int index = __jdpl__search_obj(key, to_search);
	if(index < 0)
	{
		return NULL;
	}
	
	return to_search->hashmap[index]->value;
}

//search for text(string)
const char* jdpl_objgets(const char* key, jdpl_obj* to_search)
{
	if(key == NULL || to_search == NULL)
	{
		return NULL;
	}
	
	int index = __jdpl__search_obj(key, to_search);
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

//search for number (always double)
double* jdpl_objgetnum(const char* key, jdpl_obj* to_search)
{
	if(key == NULL || to_search == NULL)
	{
		return NULL;
	}
	
	int index = __jdpl__search_obj(key, to_search);
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

//search for bool(boolean) 1 true 0 false -1 error
int* jdpl_objgetb(const char* key, jdpl_obj* to_search)
{
	if(key == NULL || to_search == NULL)
	{
		return NULL;
	}
	
	int index = __jdpl__search_obj(key, to_search);
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
	
	return &to_return->data.boolean;
}

//search for obj(jdpl_obj)
jdpl_obj* jdpl_objgetobj(const char* key, jdpl_obj* to_search)
{
	if(key == NULL || to_search == NULL)
	{
		return NULL;
	}
	
	int index = __jdpl__search_obj(key, to_search);
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

//search for arr(jdpl_arr)
jdpl_arr* jdpl_objgetarr(const char* key, jdpl_obj* to_search)
{
	if(key == NULL || to_search == NULL)
	{
		return NULL;
	}
	
	int index = __jdpl__search_obj(key, to_search);
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
static jdpl_val* __jdpl__search_arr(jdpl_arr* to_search, unsigned int index)
{
	if(index < to_search->count)
	{
		return to_search->arr[index];
	}
	
	return NULL;
}

//add a value to the jdpl array (shallow copy)
void jdpl_arradd(jdpl_val* val, jdpl_arr* to_set)
{
	if(val == NULL || to_set == NULL)
	{
		return;
	}

	//check for a recursive addition
	if(val->type == JDPL_TYPE_ARR)
	{
		if(val->data.arr == to_set)
		{
			return;
		}
	}
	
	//expand key and value arrays if necessary
	if(to_set->count == to_set->max_size)
	{
		jdpl_val** val_temp = (jdpl_val**)realloc(to_set->arr, sizeof(jdpl_val*) * (to_set->max_size + JDPL_MEMSTEP));
		
		//error out if realloc is unsuccessful
		if(!val_temp)
		{
			fprintf(stderr, __JDPL__COLOR_RED "jdpl_arradd: Memory allocation error. Could not realloc array of size: %lu.\n" __JDPL__COLOR_RESET, (unsigned long)to_set->max_size);
			exit(1);
		}
		
		to_set->max_size += JDPL_MEMSTEP;
		to_set->arr = val_temp;
	}
	
	//assign new value
	to_set->arr[to_set->count] = val;
	to_set->count++;
}

//add a value to the jdpl array (deep copy)
void jdpl_arradd_copy(jdpl_val* val, jdpl_arr* to_set)
{
	if(val == NULL || to_set == NULL)
	{
		return;
	}

	//check for a recursive addition
	if(val->type == JDPL_TYPE_ARR)
	{
		if(val->data.arr == to_set)
		{
			return;
		}
	}
	
	//expand key and value arrays if necessary
	if(to_set->count == to_set->max_size)
	{
		jdpl_val** val_temp = (jdpl_val**)realloc(to_set->arr, sizeof(jdpl_val*) * (to_set->max_size + JDPL_MEMSTEP));
		
		//error out if realloc is unsuccessful
		if(val_temp == NULL)
		{
			fprintf(stderr, __JDPL__COLOR_RED "jdpl_arradd_copy: Memory allocation error. Could not realloc array of size: %lu.\n" __JDPL__COLOR_RESET, (unsigned long)to_set->max_size);
			exit(1);
		}
		
		to_set->max_size += JDPL_MEMSTEP;
		to_set->arr = val_temp;
	}
	
	//assign new value (deep copy)
	to_set->arr[to_set->count] = jdpl_val_copy(val);
	to_set->count++;
}

//overwrite value at index with new value (shallow copy)
void jdpl_arrset(jdpl_val* val, unsigned int index, jdpl_arr* to_set)
{
	if(val == NULL || to_set == NULL)
	{
		return;
	}

	//check for a recursive addition
	if(val->type == JDPL_TYPE_ARR)
	{
		if(val->data.arr == to_set)
		{
			return;
		}
	}
	
	//index is out of bounds
	if(to_set->count < index)
	{
		return;
	}
	
	jdpl_free_val(to_set->arr[index]);
	to_set->arr[index] = val;
}

//overwrite value at index with new value (deep copy)
void jdpl_arrset_copy(jdpl_val* val, unsigned int index, jdpl_arr* to_set)
{
	if(val == NULL || to_set == NULL)
	{
		return;
	}

	//check for a recursive addition
	if(val->type == JDPL_TYPE_ARR)
	{
		if(val->data.arr == to_set)
		{
			return;
		}
	}
	
	//index is out of bounds
	if(to_set->count < index)
	{
		return;
	}
	
	jdpl_free_val(to_set->arr[index]);
	to_set->arr[index] = jdpl_val_copy(val);
}

/***********************************************************************
 **********************ARRAY GET FUNCTIONS*******************************
 ***********************************************************************/

//generic search, gets value at index
jdpl_val* jdpl_arrget(unsigned int index, jdpl_arr* to_search)
{
	if(to_search == NULL)
	{
		return NULL;
	}
	
	return __jdpl__search_arr(to_search, index);
}

//return value at index as a double(number)
double* jdpl_arrgetnum(unsigned int index, jdpl_arr* to_search)
{
	if(to_search == NULL)
	{
		return NULL;
	}
	
	jdpl_val* to_return = __jdpl__search_arr(to_search, index);
	
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

//return value at index as an int(boolean)
int* jdpl_arrgetb(unsigned int index, jdpl_arr* to_search)
{
	if(to_search == NULL)
	{
		return NULL;
	}
		
	jdpl_val* to_return = __jdpl__search_arr(to_search, index);
	
	if(to_return == NULL)
	{
		return NULL;
	}
	
	if(to_return->type != JDPL_TYPE_BOOL)
	{
		return NULL;
	}
	
	return &to_return->data.boolean;
}

//return value at index as an obj reference(jdpl_obj*)
jdpl_obj* jdpl_arrgetobj(unsigned int index, jdpl_arr* to_search)
{
	if(to_search == NULL)
	{
		return NULL;
	}
	
	jdpl_val* to_return = __jdpl__search_arr(to_search, index);
	
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

//return value at index as an arr reference(jdpl_arr*)
jdpl_arr* jdpl_arrgetarr(unsigned int index, jdpl_arr* to_search)
{
	if(to_search == NULL)
	{
		return NULL;
	}
	
	jdpl_val* to_return = __jdpl__search_arr(to_search, index);
	
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
		//return boolean (true/false)
		case JDPL_TYPE_BOOL:
		{
			//maximum number of characters should be 6 ("false")
			to_return = (char*)malloc(6);
			if(to_convert->data.boolean)
			{
				strcpy(to_return, "true");
			}
			else
			{
				strcpy(to_return, "false");
			}
			break;
		}

		//return number(double)
		case JDPL_TYPE_NUM:
		{
			//maximum number of digits should be about 24 (according to random people on the internet)
			//POSSIBLE BUG - Is the real maximum 24 characters?
			to_return = (char*)malloc(24);

			//convert number to text using sprintf
			double num = to_convert->data.num;
			if(num - (int64_t)num == 0)
			{
				sprintf(to_return, "%ld", (int64_t)num);
			}
			else
			{
				sprintf(to_return, "%f", num);
			}
			break;
		}

		//format text appropriately, surrounded by quotation marks
		case JDPL_TYPE_TEXT:
		{
			to_return = (char*)malloc(to_convert->data_size + 2);
			to_return[0] = '"';
			memcpy(to_return + 1, to_convert->data.text, to_convert->data_size);
			to_return[to_convert->data_size] = '"';
			to_return[to_convert->data_size + 1] = '\0';
			break;
		}

		//use obj_tostr() to convert object
		case JDPL_TYPE_OBJ:
		{
			to_return = jdpl_obj_tostr(to_convert->data.obj);
			break;
		}

		//use arr_tostr() to convert array
		case JDPL_TYPE_ARR:
		{
			to_return = jdpl_arr_tostr(to_convert->data.arr);
			break;
		}

		//return the string "null"
		//POSSIBLE BUG - should this be null-terminated? am I assuming this is null-terminated?
		case JDPL_TYPE_NULL:
		{
			to_return = (char*)malloc(5);
			strcpy(to_return, "null");
			break;
		}

		//return null if the value is invalid
		case JDPL_TYPE_INVALID:
			break;
	}
	
	return to_return;
}

//convert keypair to string
static char* __jdpl__keypair_tostr(jdpl_keypair* to_convert)
{
	char* value_str = jdpl_val_tostr(to_convert->value);
	size_t key_size = strlen(to_convert->key);
	size_t value_size = strlen(value_str);
	int str_size = key_size + value_size + 4;
	
	char* to_return = (char*)malloc(str_size);
	
	to_return[0] = '"';
	memcpy(to_return + 1, to_convert->key, key_size);
	to_return[key_size + 1] = '"';
	to_return[key_size + 2] = ':';
	//to_return[key_size + 3] = ' '; <- this should be done in jdpl_prettify
	memcpy(to_return+key_size+3, value_str, value_size);
	to_return[str_size - 1] = '\0';
	
	free(value_str);
	return to_return;
}


//convert object to flattened string {obj}
char* jdpl_obj_tostr(jdpl_obj* to_convert)
{
	if(to_convert == NULL)
	{
		return NULL;
	}
	
	int str_size = 3;  //minimum size for a string (""\0)
	int str_index = 1; //current index in the string
	unsigned int read_objects = 0; //number of objects read (should this be here?)
	
	char* to_return = (char*)calloc(str_size, 1);
	to_return[0] = '{';
	
	//go through each keypair in object
	for(unsigned int i = 0; i < to_convert->max_size; i++)
	{
		jdpl_keypair* keypair = to_convert->hashmap[i];
		if(keypair == NULL)
		{
			continue;
		}

		//should we add an extra character for ,? (no on last keypair)
		int expand_val = 1;
		if(read_objects == to_convert->count - 1)
		{
			expand_val -= 1;
		}
		
		char* keypair_str = __jdpl__keypair_tostr(keypair); //raw keypair string
		size_t chunk_len = strlen(keypair_str); //size of keypair string
		int starting_index = str_index;
		str_size += chunk_len + expand_val;

		//resize string that we will be returning
		char* temp = (char*)realloc(to_return, str_size);
		if(temp == NULL)
		{
			fprintf(stderr, __JDPL__COLOR_RED "jdpl_obj_tostr: Memory allocation error. Could not realloc array of size: %d.\n" __JDPL__COLOR_RESET, str_size);
			exit(1);
		}
		to_return = temp;

		//copy data
		memcpy(to_return + starting_index, keypair_str, chunk_len);
		str_index += chunk_len;

		//add a , if necessary
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
	if(to_convert == NULL)
	{
		return NULL;
	}
	
	int str_size = 3;
	int str_index = 1;
	char* to_return = (char*)calloc(str_size, 1);
	
	to_return[0] = '[';
	for(unsigned int i = 0; i < to_convert->count; i++)
	{
		//should we add a ,? (no on last value)
		int expand_val = 1;
		if(i == to_convert->count - 1)
		{
			expand_val -= 1;
		}
		
		//convert value to string
		char* val_str = jdpl_val_tostr(to_convert->arr[i]);
		size_t val_size = strlen(val_str);
		str_size += val_size + expand_val;

		//expand return string to hold value
		char* temp = (char*)realloc(to_return, str_size);
		if(temp == NULL)
		{
			fprintf(stderr, __JDPL__COLOR_RED "jdpl_obj_tostr: Memory allocation error. Could not realloc array of size: %d.\n" __JDPL__COLOR_RESET, str_size);
			exit(1);
		}
		to_return = temp;
		
		//add new data
		memcpy(to_return + str_index, val_str, val_size);
		str_index += val_size;

		//add a , if required)
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

//TODO - insert space after ':' character

//takes condensed input and prettifies it to human-readable json
//this is arcane magic, god help me if I ever need to touch it again
void jdpl_prettify(char** to_prettify, unsigned int indent_size)
{
	if(to_prettify == NULL)
	{
		return;
	}
	
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
				
				for(unsigned int x = 0; x < indent_level * indent_size; x++)
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
				
				for(unsigned int x = 0; x < indent_level * indent_size; x++)
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
				
				for(unsigned int x = 0; x < indent_level * indent_size; x++)
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
				
				for(unsigned int x = 0; x < indent_level * indent_size; x++)
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
				
				for(unsigned int x = 0; x < indent_level * indent_size; x++)
				{
					str[index] = ' ';
					index++;
				}
			}
			else if(src_str[i] == ':')
			{
				str[index] = ':';
				str[index+1] = ' ';
				index += 2;
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

		//resize string
		if(index >= str_max * 0.75)
		{
			char* temp = (char*)realloc(str, str_max * 1.5);
			if(!temp)
			{
				fprintf(stderr, __JDPL__COLOR_RED "jdpl_prettify: failed to resize output string.\n" __JDPL__COLOR_RESET);
				exit(1);
			}
			str = temp;
			str_max = str_max * 1.5;
		}
	}

	//final resize of string
	char* temp = (char*)realloc(str, index+1);
	if(!temp)
	{
		fprintf(stderr, __JDPL__COLOR_RED "jdpl_prettify: failed to resize output string.\n" __JDPL__COLOR_RESET);
		exit(1);
	}
	
	str = temp;
	str[index] = '\0';
	
	free(*to_prettify);
	*to_prettify = str;
}

/*************************************************************************
 ****************************PARSING FUNCTIONS****************************
 *************************************************************************/

//parse string and convert to jdpl_val
static jdpl_val* __jdpl__parse_value(const char* str, unsigned int len)
{	
	//flags for finding out which data type we have and content length
	int is_str    = -1; //flag and also length counter
	int is_obj    = -1; //flag and also offset value
	int obj_end   = -1;
	int is_arr    = -1; //flag and also offset value
	int arr_end   = -1;
	int is_number = -1; //flag and also length counter
	
	//flags for counting whitespace preceeding content
	unsigned int count_spaces = 1;
	unsigned int whitespace_counter = 0;
	
	//determine type and leading whitespace
	for(size_t i = 0; i < len; i++)
	{
		const char* cur = (str+i);
		if(!isspace(*cur))
		{
			count_spaces = 0;
		}
		
		if(isspace(*cur) && count_spaces)
		{
			whitespace_counter++;
			continue;
		}
		
		//data is str
		if(*cur == '"')
		{
			is_str = 2;
			i++;
			
			//determine length of string
			while( i < len && str[i] != '"')
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
			int obj_indent = 0;
			int arr_indent = 0;
			int in_quote = 0;
			do
			{
				if(str[i] == '"' && str[i-1] != '\\')
				{
					in_quote = !in_quote;
				}

				if(!in_quote)
				{
					if(str[i] == '{')
					{
						obj_indent++;
					}
					else if(str[i] == '[')
					{
						arr_indent++;
					}

					else if(str[i] == '}')
					{
						obj_indent--;
					}
					else if(str[i] == ']')
					{
						arr_indent--;
					}
				}
				i++;
			}while(((obj_indent + arr_indent) != 0) && (i < len));
			
			if((obj_indent + arr_indent) != 0)
			{
				return NULL;
			}
			obj_end = i;
			
			break;
		}
		//data is json array
		else if(*cur == '[')
		{
			is_arr = i;
			int obj_indent = 0;
			int arr_indent = 0;
			int in_quote = 0;
			do
			{
				if(str[i] == '"' && str[i-1] != '\\')
				{
					in_quote = !in_quote;
				}

				if(!in_quote)
				{
					if(str[i] == '{')
					{
						obj_indent++;
					}
					else if(str[i] == '[')
					{
						arr_indent++;
					}

					else if(str[i] == '}')
					{
						obj_indent--;
					}
					else if(str[i] == ']')
					{
						arr_indent--;
					}
				}
				i++;
			}while(((obj_indent + arr_indent) != 0) && (i < len));
			
			if((obj_indent + arr_indent) != 0)
			{
				return NULL;
			}
			arr_end = i;
			break;
		}
		//data is number
		else if(isdigit(*cur) || *cur == '.')
		{
			count_spaces = 0;
			is_number++;
		}

		//value hits no digits and no quotes
		else if(*cur == ',')
		{
			break;
		}
	}
	
	if(is_str > 1)
	{
		//validate value
		for(size_t i = is_str + whitespace_counter; i < len; i++)
		{
			if(!isspace(str[i]))
			{
				if(str[i] != ',' && str[i] != '}' && str[i] != ']')
				{
					return NULL;
				}
				else
				{
					break;
				}
			}
		}
		
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
	else if(is_number > -1)
	{
		//this is to make up for is_number starting at -1
		is_number += 1;
		
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
	else if(is_obj > -1)
	{
		//validate value
		for(size_t i = obj_end; i < len; i++)
		{
			if(!isspace(str[i]))
			{
				if(str[i] != ',' && str[i] != '}' && str[i] != ']')
				{
					return NULL;
				}
				else
				{
					break;
				}
			}
		}
		return jdpl_valobj(jdpl_obj_fromstr(str+is_obj));
	}
	else if(is_arr > -1)
	{
		//validate value
		for(size_t i = arr_end; i < len; i++)
		{
			if(!isspace(str[i]))
			{
				if(str[i] != ',' && str[i] != '}' && str[i] != ']')
				{
					return NULL;
				}
				else
				{
					break;
				}
			}
		}
		return jdpl_valarr(jdpl_arr_fromstr(str+is_arr));
	}
	
	//parse for boolean or null values
	else
	{
		if(len < 5 + whitespace_counter)
		{
			return NULL;
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

	fprintf(stderr, __JDPL__COLOR_RED "__jdpl__parse_value: Something has gone horribly wrong, this case should never be hit\n" __JDPL__COLOR_RESET);
	//error case where value cannot be parsed correctly. This should lead to an empty array/object
	return NULL;
}

//parse text to extract a keypair object
static jdpl_keypair* __jdpl__parse_keypair(const char* str)
{
	jdpl_keypair* to_return = (jdpl_keypair*)malloc(sizeof(jdpl_keypair));
	to_return->key = NULL;
	to_return->value = NULL;
	
	//flags for various objects
	size_t len = strlen(str);
	int in_quote = 0;
	
	//key location
	int key_start = 0;
	int key_end = 0;

	int data_start = 0;

	//parse key
	for(size_t i = 0; i < len; i++)
	{
		//lead up to key
		if(!in_quote && isspace(str[i]))
		{
			continue;
		}

		//main key text
		else if(in_quote)
		{
			key_end++;

			if(str[i] == '"')
			{
				key_end--;
				break;
			}
		}

		//start of key
		else if(str[i] == '"')
		{
			key_start = i;
			key_end = i;
			in_quote = 1;
		}

		//invalid keypair
		else
		{
			jdpl_free_keypair(to_return);
			return NULL;
		}
	}

	//validate keypair up to :
	for(size_t i = key_end + 2; i < len; i++)
	{
		if(isspace(str[i]))
		{
			continue;
		}
		else if(str[i] == ':')
		{
			data_start = i+1;
			break;
		}
		//invalid keypair
		else
		{
			jdpl_free_keypair(to_return);
			return NULL;
		}
	}
	
	//make copy of key
	unsigned int key_len = key_end - key_start;
	to_return->key = (char*)malloc(key_len + 1);
	to_return->key[key_len] = '\0';
	memcpy(to_return->key, str + key_start + 1, key_len);
	
	//parse value
	to_return->value = __jdpl__parse_value(str+data_start, len - data_start);

	if(!to_return->value)
	{
		jdpl_free_keypair(to_return);
		return NULL;
	}
	
	return to_return;
}

//TODO - Needs work, not parsing correctly. whole thing needs to be rechecked
//parse text to create an object
jdpl_obj* jdpl_obj_fromstr(const char* str)
{	
	jdpl_obj* to_return = jdpl_new_obj();
	int in_nested_obj = 0;
	int in_nested_arr = 0;
	int in_quote = 0;

	if(!str)
	{
		return to_return;
	}
	
	size_t len = strlen(str);

	//special case for empty string
	if(len < 1)
	{
		return to_return;
	}
	
	if(str[0] != '{')
	{
		return to_return;
	}

	//extract all keypairs
	for(size_t i = 0; i < len; i++)
	{
		if(i > 0)
		{	
			if((str[i-1] == ',' || i == 1) && (in_nested_obj == 0 && in_nested_arr == 0) && !in_quote)
			{
				jdpl_keypair* to_add = __jdpl__parse_keypair(str + i);
				if(to_add)
				{
					__jdpl__objadd_keypair(to_add, to_return);
				}
				//invalid input
				else
				{
					jdpl_free_obj(to_return);
					to_return = jdpl_new_obj();
					return to_return;
				}
			}

			//check for quotes
			//this must run after keypair checking or the first keypair will be missed
			if(str[i] == '"' && str[i-1] != '\\')
			{
				in_quote = !in_quote;
			}

			if(!in_quote)
			{
				if(str[i] == '{')
				{
					in_nested_obj++;
				}
				else if(str[i] == '}')
				{
					in_nested_obj--;
				}
				else if(str[i] == '[')
				{
					in_nested_arr++;
				}
				else if(str[i] == ']')
				{
					in_nested_arr--;
				}	
			}

		}
	}

	//This check seems to not actually help verify the validity of the json data, so it has been omitted
	/*
	in_nested_obj++;
	if(in_nested_obj != 0 || in_nested_arr != 0)
	{
		printf("%d\n", in_nested_obj);
		jdpl_free_obj(to_return);
		to_return = jdpl_new_obj();
		return to_return;
	}
	*/
	
	return to_return;
}

//parse text to create an array
jdpl_arr* jdpl_arr_fromstr(const char* str)
{	
	jdpl_arr* to_return = jdpl_new_arr();

	if(str == NULL)
	{
		return to_return;
	}
	
	size_t len = strlen(str);
	
    //special case for empty string
	if(len < 1)
	{
		return to_return;
	}
	
	if(str[0] != '[')
	{
		return to_return;
	}
	
	size_t val_start = 0;
	int val_captured = 0;
	int in_quote = 0;
	int in_nested_obj = 0;
	int in_nested_arr = 0;
	//int indentation_level = 0;
	for(size_t i = 0; i < len; i++)
	{	
		if(i > 0)
		{
			//extract values
			if((str[i-1] == ',' || i == 1) && (in_nested_obj == 0 && in_nested_arr == 0) && !in_quote)
			{
				val_start = i;
				val_captured = 0;
			}

			//convert values
			if(!val_captured)
			{	
				jdpl_val* to_add = __jdpl__parse_value(str + val_start, len - val_start);
				if(to_add)
				{
					jdpl_arradd(to_add, to_return);
				}
				
				//invalid input (return [])
				else
				{
					jdpl_free_arr(to_return);
					to_return = jdpl_new_arr();
					return to_return;
				}
				
				//reset counters
				val_captured = 1;
			}

			//check for quotes
			//this must be run after parsing otherwise the first entry will be missed
			if(str[i] == '"' && str[i-1] != '\\')
			{
				in_quote = !in_quote;
			}

			if(!in_quote)
			{
				if(str[i] == '{')
				{
					in_nested_obj++;
				}
				else if(str[i] == '}')
				{
					in_nested_obj--;
				}
				else if(str[i] == '[')
				{
					in_nested_arr++;
				}
				else if(str[i] == ']')
				{
					in_nested_arr--;
				}	
			}
		}
	}

	//This check seems to not actually help verify the validity of the json data, so it has been omitted
	/*
	in_nested_arr++;
	if(in_nested_obj != 0 || in_nested_arr != 0)
	{
		jdpl_free_arr(to_return);
		to_return = jdpl_new_arr();
		return to_return;
	}
	*/
	
	return to_return;
}

static char* __jdpl__read_file(const char* filename)
{
	FILE* input = fopen(filename, "r");

	//if file can't be opened return {}
	if(!input)
	{
		fprintf(stderr, __JDPL__COLOR_RED "__jdpl__read_file: Failed to open file %s.\n" __JDPL__COLOR_RESET, filename);
		return NULL;
	}
	
	//create string to hold file contents
	char* str = (char*)calloc(sizeof(char) * 8192 + 1, 1);
	size_t current_end = 8192;

	//read file to string
	while(fread(str, 1, 8192, input) > 0)
	{
		char* stemp = (char*)realloc(str, current_end + 1 + 8192);

		if(!stemp)
		{
			fprintf(stderr, __JDPL__COLOR_RED "__jdpl__read_file: File buffer resize failed.\n" __JDPL__COLOR_RESET);
			exit(1);
		}
		else
		{
			str = stemp;

			//initialize new buffer area
			memset(str + current_end + 1, 0, 8192);
			current_end += 8192;
		}
	}
	fclose(input);

	return str;
}

//convert a file to a json object
jdpl_obj* jdpl_obj_fromfile(const char* filename)
{
	if(!filename)
	{
		return jdpl_new_obj();
	}

	char* str = __jdpl__read_file(filename);
	
	jdpl_obj* to_return = jdpl_obj_fromstr(str);
	free(str);
	return to_return;
}

//convert a file to a json array
jdpl_arr* jdpl_arr_fromfile(const char* filename)
{
	if(!filename)
	{
		return jdpl_new_arr();
	}

	char* str = __jdpl__read_file(filename);
	
	jdpl_arr* to_return = jdpl_arr_fromstr(str);
	free(str);
	return to_return;
}

