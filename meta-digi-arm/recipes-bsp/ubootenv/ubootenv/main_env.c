/*
 * main_env.c
 *
 * Copyright (C) 2006-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: main() and user code to manage u-boot and linux environment
 *
 */

#define VERSION                 "1.8" "-g"GIT_SHA1

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <mtd/mtd-user.h>       /* MEMERASE */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>          /* ioctl */
#include <unistd.h>

#include <libdigi/crc32.h>

#include <nvram.h>              /* Nv* */

#include "env_funcs.h"
#include "environment.h"

#define APP_UBOOTENV    	"ubootenv"
#define APP_PRODINFOENV		"prodinfoenv"

static char *fileadd = NULL;
static char *printlist = NULL;
static char *setstring = NULL;
static char *eraselist = NULL;
static char *cmdname = NULL;
static int dump = 0, clean = 0;
static nv_os_type_e envType;

static const char *envTypes[] = {
	[NVOS_NONE] = "None",
	[NVOS_UBOOT] = "U-Boot",
	[NVOS_PROD_INFO] = "Product Info",
};

static const size_t envDefaultSizes[] = {
	[NVOS_NONE]		= 0,
	[NVOS_UBOOT]		= 8192,	/* Default value is CONFIG_ENV_SIZE, normally set by U-Boot */
	[NVOS_PROD_INFO]	= PROD_INFO_DATA_SIZE,	/* Defined in nvram.h but shouldn't be except uboot needs it */
};

static const char* excludeVars[][2] = {
	{"ethaddr",     "ethaddr1" },
	{"wlanaddr",    "ethaddr2" },
	{"eth1addr",    "ethaddr3" },
	{"ipaddr",      "ip1"      },
	{"ipaddr_wlan", "ip2"      },
	{"ipaddr1",     "ip3"      },
	{"netmask",     "netmask1" },
	{"netmask_wlan","netmask2" },
	{"netmask1",    "netmask3" },
	{"serverip",    "server"   },
	{"gatewayip",   "gateway"  },
	{"dnsip",       "dns1"     },
	{"dnsip2",      "dns2"     },
	{"dhcp",        "dhcp1"    },
	{"dhcp_wlan",   "dhcp2"    },
	{"dhcp1",       "dhcp3"    },
};

static const char *env_os_type_to_string(void)
{
	return envTypes[envType];
}

static int env_onetime_writable(void)
{
	if (NVOS_PROD_INFO == envType)
		return 1;

	return 0;
}

static int env_is_empty(env_t * env, unsigned int envlen)
{
	char *from = (char *)&env->data;
	char *till = (char *)&env->data + envlen;
	char *end;

	end = get_end_mark(from, till);
	/* Check if environment is empty. If yes, start from the beginning */
	if (end == (from + 1))
		return 1;

	return 0;
}

static int env_have_to_use_nvram(const char *varname)
{
	int j;

	if (envType == NVOS_UBOOT) {
		for (j = 0; j < ARRAY_SIZE(excludeVars); j++) {
			if (strcmp(varname, excludeVars[j][0]) == 0)
				return j;
		}
	}
	return -1;
}

static void show_usage(void)
{
	fprintf(stdout, "Usage: %s [options]\n"
		"%s %s Copyright Digi International Inc.\n\n"
		"Prints or updates the %s environment\n"
		"\n"
		"  -d, --dump                        Prints the values of all the environment\n"
		"  -p, --print 'var_name_list'       Prints the value of the list of variables\n"
		"                                    The list has to be simple quoted ('')\n"
		"  -s, --set  'var_name=var_value'   Sets var_value in the variable var_name\n"
		"                                    The string has to be simple quoted ('') to allow\n"
		"                                    spaces\n"
		"  -e  --erase 'var_name_list'       Removes the list of variables (simple quoted)\n"
		"  -c  --clean                       Removes all variables\n"
		"  -a  --fileadd file_name           Adds variables from file_name. To init the full\n"
		"                                    environment from file use -c -a simultaneously\n"
		"  -h  --help                        Displays usage information\n\n",
		cmdname, cmdname, VERSION, env_os_type_to_string());

	if (env_onetime_writable()) {
		fprintf(stdout, "       WARNING: Variables can only be set the first time\n"
				"                and become read-only afterwards.\n");
	}
}

static void show_usage_and_exit(int exit_code)
{
	show_usage();
	exit(exit_code);
}

static void process_options(int argc, char *argv[])
{
	int opt_index, opt, optcount = 0;
	static const char *short_options = "?hdcp:s:e:a:";
	static const struct option long_options[] = {
		{"dump",    no_argument,       NULL, 'd'},
		{"help",    no_argument,       NULL, 'h'},
		{"erase",   required_argument, NULL, 'e'},
		{"clean",   no_argument,       NULL, 'c'},
		{"fileadd", required_argument, NULL, 'a'},
		{"print",   required_argument, NULL, 'p'},
		{"set",     required_argument, NULL, 's'},
		{0, 0, 0, 0},
	};

	for (opt_index = 0;;) {

		opt = getopt_long(argc, argv, short_options, long_options, &opt_index);
		if (opt == EOF)
			break;

		switch (opt) {
		case 'd':
			dump = 1;
			break;
		case 'p':
			printlist = optarg;
			break;
		case 's':
			setstring = optarg;
			break;
		case 'a':
			fileadd = optarg;
			break;
		case 'e':
			eraselist = optarg;
			break;
		case 'c':
			clean = 1;
			break;
		case 'h':
		case '?':
			show_usage_and_exit(EXIT_SUCCESS);
			break;
		}
		optcount++;
	}

	if (optcount == 0)
		show_usage_and_exit(EXIT_FAILURE);

	/* Check options */
	if (dump && (printlist != NULL)) {
		fprintf(stderr, "--dump and --print can't be used simultaneously\n");
		show_usage_and_exit(EXIT_FAILURE);
	}
	if (clean && (eraselist != NULL)) {
		fprintf(stderr, "--clean and --erase can't be used simultaneously\n");
		show_usage_and_exit(EXIT_FAILURE);
	}
}

static int env_add_var(const char *varstring, env_t * env, unsigned int envlen)
{
	char *nvramCmd[3] = { "set", "network", NULL };
	char tmpstr[50];

	char *varval;
	int j;

	if (varstring == NULL)
		return -EINVAL;

	if (!get_var_name(varstring, tmpstr, VAR_ASIGN))
		return -EINVAL;

	/* Check if is a special variable */
	if ((j = env_have_to_use_nvram(tmpstr)) != -1) {
		if ((varval = get_var_value(varstring, excludeVars[j][0], VAR_ASIGN)) != NULL) {
			/* Define command to be used by nvram */
			sprintf(tmpstr, "%s=%s", excludeVars[j][1], varval);
			nvramCmd[2] = tmpstr;
			if (!NvCmdLine(3, (const char **)nvramCmd)) {
				return -EINVAL;
			}
		}
	} else {
		if (!add_var(env->data, env->data + envlen, (char *)varstring)) {
			return -EINVAL;
		}
	}

	return 0;
}

static int env_add_vars_from_file(char *filename, env_t * env, unsigned int envlen)
{
	FILE *fp;
	char line[ENV_MAX_VAR_NAME_LEN + ENV_MAX_VAR_VAL_LEN + 3];
	int ret = EXIT_SUCCESS;

	if ((fp = fopen(filename, "r")) == NULL)
		return -errno;

	while (!feof(fp)) {
		if (fgets(line, sizeof(line), fp)) {
			/* TODO should we remove comments starting with # ?? */
			/* Remove '\n' */
			if (line[strlen(line) - 1] == '\n')
				line[strlen(line) - 1] = 0;
			if (env_add_var((const char *)line, env, envlen)) {
				fprintf(stderr, "Unable to add environment variable %s\n",
					line);
				ret = -EINVAL;
				goto out;
			}
		}
	}

 out:
	if (fp)
		fclose(fp);
	return ret;
}

static void env_remove_varlist(const char *varlist, env_t * env, unsigned int envlen)
{
	char *var;

	var = strtok((char *)varlist, " ");

	while (var != NULL) {
		if (!remove_var(env->data, env->data + envlen, var))
			fprintf(stderr, "Unable to remove environment variable %s\n", var);
		var = strtok(NULL, " ");
	}
}

static int env_validate(env_t * env, int datalen, int verbose)
{
	unsigned long new_crc;

	/* Check stored crc with data */
	new_crc = crc32(0, (const unsigned char *)env->data, datalen);

	if ((unsigned int)env->crc != new_crc) {
		if ( verbose ) {
			fprintf(stderr, "CRC failure: got 0x%08x expected 0x%08x\n",
				(unsigned int)env->crc, (unsigned int)new_crc);
		}
		return 1;
	}
	return 0;
}

static void env_printenv_nvram_vars(char *varname)
{
	struct nv_critical *crit;
	nv_param_ip_t *ip_params;
	int index;
	int oneloop = 0;

	if (NvCriticalGet(&crit)) {

		ip_params = &crit->s.p.xIP;
		for (index = 3; index < ARRAY_SIZE(excludeVars); index++) {
			if (varname != NULL) {
				if ((index = env_have_to_use_nvram(varname)) == -1)
					break;
				oneloop = 1;
			}

			switch (index) {
			case 3:
				fprintf(stdout, "ipaddr=%s\n",
					NvToStringIP(ip_params->axDevice[0].uiIP));
				break;
			case 4:
				fprintf(stdout, "ipaddr_wlan=%s\n",
					NvToStringIP(ip_params->axDevice[1].uiIP));
				break;
			case 5:
				fprintf(stdout, "ipaddr1=%s\n",
					NvToStringIP(crit->s.p.eth1dev.uiIP));
				break;
			case 6:
				fprintf(stdout, "netmask=%s\n",
					NvToStringIP(ip_params->axDevice[0].uiNetMask));
				break;
			case 7:
				fprintf(stdout, "netmask_wlan=%s\n",
					NvToStringIP(ip_params->axDevice[1].uiNetMask));
				break;
			case 8:
				fprintf(stdout, "netmask1=%s\n",
					NvToStringIP(crit->s.p.eth1dev.uiNetMask));
				break;
			case 9:
				fprintf(stdout, "serverip=%s\n",
					NvToStringIP(ip_params->uiIPServer));
				break;
			case 10:
				fprintf(stdout, "gatewayip=%s\n",
					NvToStringIP(ip_params->uiIPGateway));
				break;
			case 11:
				fprintf(stdout, "dnsip=%s\n",
					NvToStringIP(ip_params->auiIPDNS[0]));
				break;
			case 12:
				fprintf(stdout, "dnsip2=%s\n",
					NvToStringIP(ip_params->auiIPDNS[1]));
				break;
			case 13:
				fprintf(stdout, "dhcp=%s\n",
					(ip_params->axDevice[0].flags.bDHCP ? "on" : "off"));
				break;
			case 14:
				fprintf(stdout, "dhcp_wlan=%s\n",
					(ip_params->axDevice[1].flags.bDHCP ? "on" : "off"));
				break;
			case 15:
				fprintf(stdout, "dhcp1=%s\n",
					(crit->s.p.eth1dev.flags.bDHCP ? "on" : "off"));
				break;
			}
			if (oneloop)
				break;
		}
	}
}

static void env_printenv(int dump, char *varlist, env_t * env, unsigned int envlen)
{
	char *var;
	char *data = env->data;
	char *varinenv;

	if (dump) {
		/* print the full environment  */
		while (data != NULL && *data) {
			fprintf(stdout, "%s\n", data);
			data = get_next_env_string(data, env->data + envlen);
		}
		if (envType == NVOS_UBOOT)
			env_printenv_nvram_vars(NULL);
		return;
	} else {
		var = strtok((char *)varlist, " ");

		while (var != NULL) {

			if (envType == NVOS_UBOOT)
				env_printenv_nvram_vars(var);
			if ((varinenv =
			     get_var_addr(env->data, env->data + envlen, var)) != NULL) {
				fprintf(stdout, "%s\n", varinenv);
			}
			var = strtok(NULL, " ");
		}
	}
}

int main(int argc, char *argv[])
{
	unsigned int envlen = 0;
	env_t *env = NULL;
	nv_param_os_cfg_t xCfg;
	size_t iSize;
	int save_env = 0;
	int ret = EXIT_SUCCESS;
	int found;
	char *cmdstart;

	cmdname = argv[0];

	cmdname = *argv;

	if ((cmdstart = strrchr(cmdname, '/')) != NULL) {
		cmdname = cmdstart + 1;
	}

	if (strcmp(cmdname, APP_UBOOTENV) == 0)
		envType = NVOS_UBOOT;
	else if (strcmp(cmdname, APP_PRODINFOENV) == 0)
		envType = NVOS_PROD_INFO;
	else {
		fprintf(stderr, "This application can be invoked as:\n"
			"%s\n%s\n\n", APP_UBOOTENV, APP_PRODINFOENV);
		return EXIT_FAILURE;
	}

	/* read and process command line */
	process_options(argc, argv);

	if (!NvInit(NVR_AUTO)) {
		fprintf(stderr, "Unable to initialize nvram\n");
		return EXIT_FAILURE;
	}

	found = NvOSCfgFind(&xCfg, envType);

	if (!found && clean) {
		/* Add missing section to nvram if --clean specified */

		if (envDefaultSizes[envType] != 0 ) {
			if (!NvOSCfgAdd(envType, envDefaultSizes[envType])) {
				fprintf(stderr, "Unable to add missing %s environment to flash\n",
					env_os_type_to_string());
				return EXIT_FAILURE;
			}

			/* Try again to find our section */
			found = NvOSCfgFind(&xCfg, envType);
		}
	}

	if (!found) {
		fprintf(stderr, "Unable to detect %s environment in flash\n",
			env_os_type_to_string());
		return EXIT_FAILURE;
	}

	env = malloc(xCfg.uiSize);
	if (NULL == env) {
		perror("malloc");
		return EXIT_FAILURE;
	}

	memset(env, 0, xCfg.uiSize);
	/* Adjust envlen for just for data, crc32 value not included */
	envlen = xCfg.uiSize - sizeof(unsigned long);

	if (!NvOSCfgGet(envType, (void *)env, xCfg.uiSize, &iSize)) {
		/* This can't fail; we either found it, created it, or exited. */
		/* But just to be safe... */
		fprintf(stderr, "Unable to get %s environment from flash\n",
				env_os_type_to_string());
		ret = EXIT_FAILURE;
		goto free_and_ret;
	}

	/* Check if env is one-time writable, is valid, and was already written */
	if (env_onetime_writable() && !env_validate(env, envlen, 0) &&
		!env_is_empty(env, envlen)) {
		if ( clean || fileadd || eraselist || setstring ) {
			fprintf(stderr, "Environment is one-time writable only\n");
			ret = EXIT_FAILURE;
			goto free_and_ret;
		}
	}

	if (clean) {
		memset(env, 0, xCfg.uiSize);
		save_env = 1;
	} else {
		if (env_validate(env, envlen, 1)) {
			fprintf(stderr, "Invalid %s environment found\n",
				env_os_type_to_string());
			ret = EXIT_FAILURE;
			goto free_and_ret;
		}
	}

	if (fileadd != NULL) {
		if (env_add_vars_from_file(fileadd, env, envlen) < 0) {
			fprintf(stderr, "Unable to add %s environment from file %s\n",
				env_os_type_to_string(), fileadd);
			ret = EXIT_FAILURE;
			goto free_and_ret;
		}
		save_env = 1;
	}

	if (eraselist != NULL) {
		env_remove_varlist(eraselist, env, envlen);
		save_env = 1;
	}

	if (setstring != NULL) {
		if (env_add_var(setstring, env, envlen)) {
			fprintf(stderr, "Unable to add environment variable %s\n", setstring);
			ret = EXIT_FAILURE;
			goto free_and_ret;
		}
		save_env = 1;
	}

	if (dump) {
		env_printenv(1, NULL, env, envlen);
	} else if (printlist != NULL) {
		env_printenv(0, printlist, env, envlen);
	}

	if (save_env) {
		/* Compute new crc32 value just in case we are going to update the value in flash */
		env->crc = crc32(0, (const unsigned char *)env->data, envlen);

		if (!NvOSCfgSet(envType, env, xCfg.uiSize)) {
			fprintf(stderr, "Unable to set %s environment to store in flash\n",
				env_os_type_to_string());
			ret = EXIT_FAILURE;
		}
		if (!NvSave()) {
			fprintf(stderr, "Unable to save %s environment in flash\n",
				env_os_type_to_string());
			ret = EXIT_FAILURE;
		}
	}

 free_and_ret:
	free(env);
	return ret;
}

