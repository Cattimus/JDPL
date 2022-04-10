#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "JDPL.h"

static char* print_specifiers[] = {"%c", "%d", "%e", "%f", "%g", "%hi", "%hu", "%i", "%ld", "%lf", "%o", "%p", "%s", "%u", "%x", "%n", "%%", "%a"};
static char* unicode_characters[] = {"😀", "🙂", "🙃", "😗", "🤐", "😌", "😟", "😯", "🤬", "💩", "😺", "❤", "💣", "👇", "🧠", "🧔", "🧘", "Ó", "ɂ"};

static char* generate_value(int* data_type);
static char* generate_keypair(int* depth);
char* generate_obj(int* depth);
char* generate_arr(int* depth);
char* generate_json();

static char* generate_value(int* data_type)
{
	int type = 0;

	//select a random type for the data(does not include objects)
	if(!data_type || *data_type == 0)
	{
		type = (rand() / (RAND_MAX / 4)) + 1;
	}
	//generate object or arr
	else if(*data_type < 0)
	{
		type = (rand() / (RAND_MAX / 6)) + 1;
	}
	//select a predetermined type for the data
	else
	{
		if(*data_type > 0 && *data_type < 7)
		{
			type = *data_type;
		}
	}

	switch(type)
	{
	case 1:
	{
		int text_size = rand() % 50;
		text_size += 3;
		char* value = (char*)malloc(sizeof(char) * text_size);
		memset(value, 0, sizeof(char) * text_size);

		value[0] = '"';

		//fill array with random characters
		for(int i = 1; i < text_size - 2; i++)
		{
			//random character value
			value[i] = (rand() / (RAND_MAX / ('~' - ' '))) + ' ';

			//invalid values
			if(value[i] == '"' || value[i] == '\\')
			{
				if(rand() % 2 == 0)
				{
					value[i]++;
				}
				else
				{
					value[i]--;
				}
			}

			//random chance to insert a formatting character
			if((rand() < (RAND_MAX / 1024)))
			{
				char* print_specifier = print_specifiers[rand() / (RAND_MAX / 18)];
				int print_specifier_len = strlen(print_specifier);

				if((text_size - (print_specifier_len + 1 + i)) > 1)
				{
					strcat(value, print_specifier);
					i += print_specifier_len;
				}
			}

			//random chance to insert unicode character
			else if((rand() < (RAND_MAX / 512)))
			{
				char* special_char = unicode_characters[rand() / (RAND_MAX / 19)];
				int char_len = strlen(special_char);

				if((text_size - (char_len + 1 + i)) > 1)
				{
					strcat(value, special_char);
					i += char_len;
				}
			}
			
		}

		value[text_size - 2] = '"';
		value[text_size - 1] = '\0';

		return value;
	}

	case 2:
	{
		int text_size = (rand() / (RAND_MAX / 18)) + 2;
		int has_decimal = 0;
		char* value = (char*)malloc(sizeof(char) * text_size);
		memset(value, 0, sizeof(char) * text_size);

		//fill array with random numbers
		for(int i = 0; i < text_size - 1; i++)
		{
			//random number value
			value[i] = ((rand() / (RAND_MAX / 10)) + '0');

			//randomly insert a decimal
			//this can generate invalid inputs on purpose
			//possible generation of . at the beginning or end of the number
			if(!has_decimal)
			{
				if(rand() < (RAND_MAX / 100))
				{
					value[i] = '.';
					has_decimal = 1;
				}
			}
		}

		value[text_size - 1] = '\0';
		return value;
	}

	case 3:
	{
		int condition = rand() % 2;

		if(condition)
		{
			char* value = (char*)malloc(sizeof(char) * 5);
			strcpy(value, "true");
			return value;
		}
		else
		{
			char* value = (char*)malloc(sizeof(char) * 6);
			strcpy(value, "false");
			return value;
		}
	}

	case 4:
	{
		char* value = (char*)malloc(sizeof(char) * 5);
		strcpy(value, "null");
		return value;
	}

	//jdpl obj (only manually generated)
	case 5:
	{
		*data_type += 1;
		return generate_obj(data_type);
	}

	//jdpl arr (only manually generated)
	case 6:
	{
		*data_type += 1;
		return generate_arr(data_type);
	}
		
	}

	//fail into null
	char* value = (char*)malloc(sizeof(char) * 5);
	strcpy(value, "null");
	return value;
}

static char* generate_keypair(int* depth)
{
	int key_type = 1;
	char* key = generate_value(&key_type);
	char* value = generate_value(depth);

	int keypair_len = strlen(key) + strlen(value) + 2;
	char* keypair = (char*)malloc(sizeof(char) * keypair_len);
	memset(keypair, 0, keypair_len);

	//random chance to reverse key and value
	if((rand() < (RAND_MAX / 10000)))
	{
		strcpy(keypair, value);
		strcat(keypair, ":");
		strcat(keypair, key);
	}

	//key and value in proper order
	else
	{
		strcpy(keypair, key);
		strcat(keypair, ":");
		strcat(keypair, value);
	}

	free(key);
	free(value);
	return keypair;
}

char* generate_obj(int* depth)
{
	int num_elements = (rand() / (RAND_MAX / 15)) + 1;
	int current_size = sizeof(char) * 2;
	char* to_return = (char*)malloc(sizeof(char) * 2);
	
	int max_depth = *depth;
	
	to_return[0] = '{';
	to_return[1] = '\0';

	for(int i = 0; i < num_elements; i++)
	{
		char* keypair = generate_keypair(depth);
		size_t keypair_len = strlen(keypair);

		char* temp = (char*)realloc(to_return, sizeof(char) * current_size + keypair_len + 1);
		if(!temp)
		{
			fprintf(stderr, "generate_obj: unable to reallocate buffer\n");
			exit(1);
		}
		
		memset(temp + current_size, 0, keypair_len + 1);
		to_return = temp;
		current_size += keypair_len + 1;
		
		if(i < (num_elements - 1))
		{	
			strcat(to_return, keypair);
			strcat(to_return, ",");
		}
		else
		{
			strcat(to_return, keypair);
			strcat(to_return, "}");
		}

		//reset depth
		*depth = max_depth;
		
		free(keypair);
	}

	return to_return;
}


char* generate_arr(int* depth)
{
	int num_elements = (rand() / (RAND_MAX / 15)) + 1;
	int current_size = sizeof(char) * 2;
	char* to_return = (char*)malloc(sizeof(char) * 2);
	int max_depth = *depth;
	
	to_return[0] = '[';
	to_return[1] = '\0';

	for(int i = 0; i < num_elements; i++)
	{
		char* value = generate_value(depth);
		size_t value_len = strlen(value);

		char* temp = (char*)realloc(to_return, sizeof(char) * current_size + value_len + 1);
		if(!temp)
		{
			fprintf(stderr, "generate_json: unable to reallocate buffer\n");
			exit(1);
		}
		
		memset(temp + current_size, 0, value_len + 1);
		to_return = temp;
		current_size += value_len + 1;
		
		if(i < (num_elements - 1))
		{	
			strcat(to_return, value);
			strcat(to_return, ",");
		}
		else
		{
			strcat(to_return, value);
			strcat(to_return, "]");
		}

		//reset depth
		*depth = max_depth;

		free(value);
	}

	return to_return;
}

char* generate_json()
{
	int max_depth = (rand() / (RAND_MAX / 5)) * -1;
	char* to_return = generate_obj(&max_depth);
	return to_return;
}

//read file to string
char* file_to_str(char* filename)
{
	//open file
	FILE* input = fopen(filename, "r");
	if(!input)
	{
		return NULL;
	}

	//allocate buffer
	char* str = (char*)malloc(sizeof(char) * 8192 + 1);
	memset(str, 0, 8192 + 1);
	size_t current_size = 8192;

	//read and expand buffer until string is read
	while(fread(str, 1, 8192, input) > 0)
	{
		char* temp = (char*)realloc(str, sizeof(char) * (current_size + 8192 + 1));
		if(!temp)
		{
			fprintf(stderr, "file_to_str: Unable to resize string.\n");
			exit(1);
		}

		str = temp;
		memset(str + current_size, 0, 8192 + 1);
		current_size += 8192;
	}
	
	return str;
}
