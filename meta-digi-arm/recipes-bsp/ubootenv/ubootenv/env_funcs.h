/*
 * env_funcs.h
 *
 * Copyright (C) 2006-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: Data types and prototypes for parsing the NVRAM environment
 *
 */

#ifndef ENV_FUNCS_H
#define ENV_FUNCS_H

#define ENV_MAX_VAR_NAME_LEN	50
#define ENV_MAX_VAR_VAL_LEN	256

char *get_var_value(const char *from, const char *var_name, char sep);
char *get_next_env_string(char *from, char *till);
char *get_var_addr(char *from, char *till, char *var_name);
char *get_end_mark(char *from, char *till);
int get_var_name(const char *from, char *var_name, char sep);
int add_var(char *from, char *till, char *var_str);
int remove_var(char *from, char *till, char *var_name);

#endif
