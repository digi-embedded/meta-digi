/*
 * env_funcs.c
 *
 * Copyright (C) 2006-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: Function prototypes for all the flavors of the NVRAM tool
 *
 */

#include <stdio.h>
#include <string.h>

#include "env_funcs.h"

/*
 * Function:    get_var_value
 * Return:      NULL on failure/ pointer where var value starts on succes
 * Description: Checks if the variable name is contained on the string and
 *              returns a pointer where the variable value starts.
 */
char *get_var_value(const char *from, const char *var_name, char sep)
{
	char *separator;
	char *start;

	if (((separator = strchr(from, sep)) != NULL) &&
	    ((start = strstr(from, var_name)) != NULL)) {
		if (start < separator)
			return (++separator);
	}

	return NULL;
}

/*
 * Function:    get_var_name
 * Return:      1 on success, 0 otherwise
 * Description: Retrieves the var name from a var string
 */
int get_var_name(const char *from, char *var_name, char sep)
{
	char *separator;

	if ((separator = strchr(from, sep)) != NULL) {
		while (from < separator)
			*var_name++ = *from++;
		*var_name = 0;
		return 1;
	}
	return 0;
}

/*
 * Function:    get_next_env_string
 * Return:      the string to the next environment variable in UBOOT or
 *              NULL on failure
 * Description: returns the addr of the next string in a data structure
 *              <string>\0<string>\0\0
 *              It does an offset calculation to check for overflow.
 */
char *get_next_env_string(char *from, char *till)
{
	if (*from == 0)
		return NULL;

	while (from < till && *from)
		from++;

	if (from == till && *from)
		return NULL;	// Indicate string to long

	return (++from);
}

/*
 * Function:    get_var_addr
 * Return:      the pointer to the address string or NULL if not found.
 * Description: Returns the addr of the variable in a data structure
 *              <string>\0<string>\0\0
 *              It does an offset calculation to check for overflow.
 */
char *get_var_addr(char *from, char *till, char *var_name)
{
	char *data = from;
	char *var_addr;
	char var_name_temp[ENV_MAX_VAR_NAME_LEN + 1];

	sprintf(var_name_temp, "%s%s", var_name, "=");

	do {
		if (*data && data < till) {
			if ((var_addr = strstr(data, var_name_temp)) != NULL
			    && (var_addr == data))
				return var_addr < till ? var_addr : NULL;
		}
	} while ((data = get_next_env_string(data, till)) != NULL);

	return NULL;
}

/*
 * Function:    get_end_mark
 * Description:
 */
char *get_end_mark(char *from, char *till)
{
	while (from < till) {
		while (*from)
			from++;
		if (from >= till)
			return NULL;
		if (*(++from) == 0)
			return from;
	}
	return NULL;
}

/*
 * Function:    remove_var
 * Return:      1 on success, 0 otherwise
 * Description: Remove environment variables
 */
int remove_var(char *from, char *till, char *var_name)
{
	char *var_addr;
	char *env_end;
	char *var_end;

	/* Check if variable already exists */
	if ((var_addr = get_var_addr(from, till, var_name)) != NULL) {
		if ((env_end = get_end_mark(var_addr, till)) != NULL) {
			if ((var_end = get_next_env_string(var_addr, till)) != NULL) {
				while (var_end <= env_end)
					*var_addr++ = *var_end++;
				while (var_addr <= env_end)
					*var_addr++ = 0;	/* Just to have a clean environment :-) */
				return 1;
			}
		}
	}

	return 0;
}

/*
 * Function:    add_var
 * Return:      1 on success, 0 otherwise
 * Description: Add a new environment variable
 */
int add_var(char *from, char *till, char *var_str)
{
	char *var_addr;
	char var_name[ENV_MAX_VAR_NAME_LEN];

	if (get_var_name(var_str, var_name, '=')) {
		/* Check if variable already exists */
		if ((var_addr = get_var_addr(from, till, var_name)) != NULL) {
			/* @TODO: remove more?? could be that it were there more than once?? */
			if (!remove_var(var_addr, till, var_name))
				return 0;
		}
		/* Append the variable to the end */
		if ((var_addr = get_end_mark(from, till)) != NULL) {
			/* Check if environment is empty. If yes, start from the beginning */
			if (var_addr == (from + 1))
				var_addr--;
			while (var_addr < till && *var_str)
				*var_addr++ = *var_str++;
			*var_addr++ = 0;
			*var_addr = 0;
			return 1;
		} else {
			fprintf(stderr, "Unable to find environment end\n");
		}
	}

	return 0;
}
