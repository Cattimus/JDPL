#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define JDPL_MEMSTEP 1024
#define JDPL_MIN_SIZE 1024

typedef enum JDPL_TYPE_ENUM
{
	JDPL_TYPE_TEXT, JDPL_TYPE_NUM, JDPL_TYPE_BOOL, JDPL_TYPE_OBJ, JDPL_TYPE_ARR, JDPL_TYPE_NULL, JDPL_TYPE_INVALID
}JDPL_TYPE;

typedef struct JDPL_VALUE
{
	JDPL_TYPE type; //type of held data
	size_t data_size;  //size of held data in bytes
	void* data;     //pointer to data
	
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
	for(int i = 0; i < strlen(key); i++)
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
	to_return->data = malloc(to_return->data_size);
	double data_copy = (double)data;
	memcpy(to_return->data, &data_copy, sizeof(double));
	
	return to_return;
}

jdpl_val* jdpl_vall(int64_t data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = JDPL_TYPE_NUM;
	
	to_return->data_size = sizeof(double);
	to_return->data = malloc(to_return->data_size);
	double data_copy = (double)data;
	memcpy(to_return->data, &data_copy, sizeof(double));
	
	return to_return;
}

jdpl_val* jdpl_vals(const char* data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = JDPL_TYPE_TEXT;
	
	to_return->data_size = strlen(data) + 1;
	to_return->data = malloc(to_return->data_size);
	memcpy(to_return->data, data, to_return->data_size);
	
	return to_return;
}

jdpl_val* jdpl_valf(float data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = JDPL_TYPE_NUM;
	
	to_return->data_size = sizeof(double);
	to_return->data = malloc(to_return->data_size);
	double data_copy = (double)data;
	memcpy(to_return->data, &data_copy, sizeof(double));
	
	return to_return;
}

jdpl_val* jdpl_vald(double data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = JDPL_TYPE_NUM;
	
	to_return->data_size = sizeof(double);
	to_return->data = malloc(to_return->data_size);
	memcpy(to_return->data, &data, sizeof(double));
	
	return to_return;
}

jdpl_val* jdpl_valb(int data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = JDPL_TYPE_BOOL;
	
	char bool_data[6];
	
	if(data)
	{
		strcpy(bool_data, "true");
	}
	else
	{
		strcpy(bool_data, "false");
	}
	
	to_return->data_size = sizeof(char) * 6;
	to_return->data = malloc(to_return->data_size);
	memcpy(to_return->data, bool_data, sizeof(char) * 6);
	
	return to_return;
}

//TODO - jdpl_valobj_copy() jdpl_valarr_copy() (deep copy variants)
jdpl_val* jdpl_valobj(jdpl_obj* data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = JDPL_TYPE_OBJ;
	
	to_return->data_size = sizeof(jdpl_obj*);
	to_return->data = data;
	
	return to_return;
}

jdpl_val* jdpl_valarr(jdpl_arr* data)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	to_return->type = JDPL_TYPE_ARR;
	
	to_return->data_size = sizeof(jdpl_arr);
	to_return->data = data;
	
	return to_return;
}

//create a deep copy of the selected value
jdpl_val* jdpl_val_copy(jdpl_val* to_copy)
{
	jdpl_val* to_return = (jdpl_val*)malloc(sizeof(jdpl_val));
	
	to_return->type = to_copy->type;
	to_return->data_size = to_copy->data_size;
	
	to_return->data = malloc(to_copy->data_size);
	memcpy(to_return->data, to_copy->data, to_copy->data_size);
	
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

/***********************************************************************
 ********************OBJECT SET FUNCTIONS*******************************
 ***********************************************************************/

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
	
	return (const char*)to_return->data;
}

//decimal(double)
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
	
	return (double*)to_return->data;
}

//bool(boolean) 1 true 0 false -1 error
int jdpl_objgetb(const char* key, jdpl_obj* to_search)
{
	int index = jdpl_search_obj(key, to_search);
	if(index < 0)
	{
		return -1;
	}
	jdpl_val* to_return = to_search->hashmap[index]->value;
	
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
	
	return (jdpl_obj*)to_return->data;
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
		{
			to_return = (char*)malloc(6);
			memcpy(to_return, to_convert->data, 6);
			break;
		}
			
		case JDPL_TYPE_NUM:
		{
			to_return = (char*)malloc(24);
			double num = *(double*)to_convert->data;
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
			memcpy(to_return + 1, to_convert->data, to_convert->data_size);
			to_return[to_convert->data_size] = '"';
			to_return[to_convert->data_size + 1] = '\0';
			break;
		}
			
		case JDPL_TYPE_OBJ:
		{
			to_return = jdpl_obj_tostr((jdpl_obj*)to_convert->data);
			break;
		}
			
		case JDPL_TYPE_ARR:
		{
			to_return = jdpl_arr_tostr((jdpl_arr*)to_convert->data);
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
	int key_size = strlen(to_convert->key);
	int value_size = strlen(value_str);
	int str_size = key_size + value_size + 4;
	
	char* to_return = (char*)malloc(str_size);
	
	to_return[0] = '"';
	memcpy(to_return + 1, to_convert->key, key_size);
	to_return[key_size + 1] = '"';
	to_return[key_size + 2] = ':';
	memcpy(to_return+key_size+3, value_str, value_size);
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
		int chunk_len = strlen(keypair_str);
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
		int val_size = strlen(val_str);
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
//TODO - output to string instead of printing
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
		return to_search->arr[index];
	}
	
	return NULL;
}

/***********************************************************************
 *********************ARRAY SET FUNCTIONS*******************************
 ***********************************************************************/

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
 *********************ARRAY GET FUNCTIONS*******************************
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
	
	return (double*)to_return->data;
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
	
	return (int*)to_return->data;
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
	
	return (jdpl_obj*)to_return->data;
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
	
	return (jdpl_arr*)to_return->data;
}
