/*
 * battery_test.c
 *
 * Copyright (C) 2010 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: Battery characterization test application.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "bat_test.h"
#include <err.h>
#include <errno.h>
#include <pwd.h>

static int debug = 0;

static const char * log_file = "batd.log";
static char * lock_file = "batd.lock";

#define BAT_SYSFS_BUFF_SIZE 64

#define SYSFS_MC13892_BAT_PATH "/sys/class/power_supply/mc13892_bat/"
#define SYSFS_MC13892_MODULE_PATH "/sys/module/pmic_battery/parameters/"

#define log( fmt, arg...)											\
	do { 															\
		char msg[256];												\
		sprintf(msg,"[%s:%d]"fmt , __FUNCTION__,__LINE__, ## arg); 	\
		if(debug)													\
			printf("%s",msg);										\
		else														\
			file_log_message(msg);									\
	} while (0)

void file_log_message( char *message ) {
	FILE * lfd;
	lfd=fopen(log_file,"a");
	if(!lfd)
		return;
	fprintf(lfd,"%s\n",message);
	fclose(lfd);
}

void signal_handler(int sig) {
	switch(sig) {
		case SIGHUP:
			log("hangup signal catched\n");
			break;
		case SIGTERM:
			log("terminate signal catched\n");
			exit(EXIT_SUCCESS);
			break;
		default:
			log("signal %d catched\n",sig);
			break;
	}
}

static void bat_read_sysfs ( const char * sysfs_path , char * attr_name , char * attr_value )
{
	FILE * afd;
	int bread = 0;
	char path[256] = "";
	strcpy(path,sysfs_path);
	strcat(path,attr_name);
	afd = fopen(path,"r");
	if(!afd) {
		strcpy(attr_value,"");
		return;
	}

	bread = fread(attr_value,sizeof(char),BAT_SYSFS_BUFF_SIZE,afd);
	if (bread < 1 )
		strcpy(attr_value,"");
	fclose(afd);
	return;
}

static void daemonize( const char *lockfile )
{
    pid_t pid, sid, parent;
    int lfp = -1;

    /* already a daemon */
    if ( getppid() == 1 )
    	return;

    /* Create the lock file as the current user */
    if ( lockfile && lockfile[0] ) {
        lfp = open(lockfile,O_RDWR|O_CREAT,0640);
        if ( lfp < 0 ) {
            printf("unable to create lock file %s, code=%d (%s)",
                    lockfile, errno, strerror(errno) );
            exit(EXIT_FAILURE);
        }
    }

    /* Trap signals that we expect to recieve */
    signal(SIGCHLD,signal_handler);
    signal(SIGUSR1,signal_handler);
    signal(SIGALRM,signal_handler);

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        printf( "unable to fork daemon, code=%d (%s)",
                errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {

        /* Wait for confirmation from the child via SIGTERM or SIGCHLD, or
           for two seconds to elapse (SIGALRM).  pause() should not return. */
        alarm(2);
        pause();

        exit(EXIT_FAILURE);
    }

    /* At this point we are executing as the child process */
    parent = getppid();

    /* Cancel certain signals */
    signal(SIGCHLD,SIG_DFL); /* A child process dies */
    signal(SIGTSTP,SIG_IGN); /* Various TTY signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP, SIG_IGN); /* Ignore hangup signal */
    signal(SIGTERM,SIG_DFL); /* Die on SIGTERM */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        log("unable to create a new session, code %d (%s)",
                errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        log("unable to change directory to %s, code %d (%s)",
                "/", errno, strerror(errno) );
        exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    if (freopen("/dev/null", "r", stdin) == NULL)
        err(1, "reopen stdin");
    if (freopen("/dev/null", "w", stdout) == NULL)
        err(1, "reopen stdout");
    if (freopen("/dev/null", "w", stderr) == NULL)
        err(1, "reopen stderr");

    /* Tell the parent process that we are A-okay */
    kill( parent, SIGUSR1 );
}

/**
 * Prints online help
 *
 * @fd file to print on
 */
void printUsage(FILE* fd){
fprintf(fd,"Usage: battery_test -c -r -n -e -a -i -d -g [-v] [-h]\n"
  "  Battery characterization test application\n\n"
  "     -c Battery capacity in mAh\n"
  "     -r Battery hour rating (usually 20 hour rating)\n"
  "     -n Battery Peukert number\n"
  "     -e Battery charge efficiency factor\n"
  "     -a Maximum voltage by design in mV\n"
  "     -i Minimum voltage by design in mV\n"
  "     -d Battery discharge characterization parameters.\n"
  "        A comma separated list of 20 voltage values (in mV) representing"
  " 5 points capacity percentage increments.\n"
  "     -g Battery charge characterization parameters.\n"
  "        A comma separated list of 20 voltage values (in mV) representing"
  " 5 points capacity percentage increments.\n"
  "     -v Debug mode\n"
  "     -h This help\n\n");
}

static int bat_status_to_state( char * bat_status ) {
	int state = -1;

	if ( !strcmp(bat_status,"Charging") )
		state = POWER_SUPPLY_STATUS_CHARGING;
	else if( !strcmp(bat_status,"Discharging") )
		state = POWER_SUPPLY_STATUS_DISCHARGING;
	else if( !strcmp(bat_status,"Not charging") )
		state = POWER_SUPPLY_STATUS_NOT_CHARGING;
	else if( !strcmp(bat_status,"Full") )
		state = POWER_SUPPLY_STATUS_FULL;
	else if( !strcmp(bat_status,"Unknown") )
		state = POWER_SUPPLY_STATUS_UNKNOWN;
	else {
		return state;
	}
	return state;
}

static int bat_average_read( char * var )
{
	unsigned int reads[8];
	char tmp[BAT_SYSFS_BUFF_SIZE]="";
	int i;
	unsigned int val = 0;

	for( i= 0 ; i < 8 ; i++ ) {
		bat_read_sysfs(SYSFS_MC13892_BAT_PATH,var,tmp);
		reads[i] = atoi(tmp);
	}

	for( i= 0 ; i < 8 ; i++ ) {
		val = val + reads[i];
	}

	return(val/8);
}

static int bat_read_status( char * bat_status , char * bat_current , char * bat_voltage , char * bat_charging_current )
{
	int len = 0;

	int bat_charging_current_idx = 0;

	sprintf(bat_current,"%d",bat_average_read("current_now"));
	sprintf(bat_voltage,"%d",bat_average_read("voltage_now"));
	bat_read_sysfs(SYSFS_MC13892_BAT_PATH,"status",bat_status);
	// Null terminate status
	len = strcspn(bat_status,"\n");
	bat_status[len] = '\0',
	bat_read_sysfs(SYSFS_MC13892_MODULE_PATH,"main_charger_current",bat_charging_current);

	bat_charging_current_idx = atoi(bat_charging_current);
	strcpy(bat_charging_current,"");
	snprintf(bat_charging_current,BAT_SYSFS_BUFF_SIZE,"%d",bat_main_charging_current(bat_charging_current_idx));

	if( !strcmp(bat_status,"") || !strcmp(bat_current,"") || !strcmp(bat_voltage,"") || !strcmp(bat_charging_current,"") ) {
		log("Error reading sysfs attributes\n");
		return(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}

static int bat_get_percentage( char * bat_status , char * bat_voltage )
{
	int bat_percentage = 0;
	bat_percentage = bat_get_capacity(bat_status_to_state(bat_status), atoi(bat_voltage)/1000);
	return bat_percentage;
}

int main (int argc, char** argv){
	int opt;
	int retval = 0;
	int bat_rating = 0;
	int bat_capacity = 0;
	float bat_peukert = 0;
	float bat_charge_efficiency = 0;
	char * end;
	int bat_percentage = 0;
	char bat_max_voltage[BAT_SYSFS_BUFF_SIZE] = "";
	char bat_min_voltage[BAT_SYSFS_BUFF_SIZE] = "";
	char bat_status[BAT_SYSFS_BUFF_SIZE] = "";
	char bat_current[BAT_SYSFS_BUFF_SIZE] = "";
	char bat_voltage[BAT_SYSFS_BUFF_SIZE] = "";
	char bat_charging_current[BAT_SYSFS_BUFF_SIZE] = "";
	char bat_charge_parameters[256] = "";
	char bat_discharge_parameters[256] = "";

	while ( (opt=getopt(argc,argv,"c:r:n:e:d:g:a:i:vh"))!=-1 ){
		switch(opt){
			case '?':
				printUsage(stderr);
				return 1;
			case 'h':
				printUsage(stdout);
				return 0;
			case 'c':
				bat_capacity=atoi(optarg);
				break;
			case 'r':
				bat_rating=atoi(optarg);
				break;
			case 'n':
				bat_peukert=strtof(optarg,&end);
				break;
			case 'e':
				bat_charge_efficiency=strtof(optarg,&end);
				break;
			case 'd':
				strcpy(bat_discharge_parameters,optarg);
				break;
			case 'g':
				strcpy(bat_charge_parameters,optarg);
				break;
			case 'a':
				strcpy(bat_max_voltage,optarg);
				break;
			case 'i':
				strcpy(bat_min_voltage,optarg);
				break;
			case 'v':
				debug=1;
				break;
		}
	}

	log("Battery monitoring daemon started.\n");

	if( !bat_rating || !bat_capacity || !bat_peukert || !bat_charge_efficiency || !strcmp(bat_charge_parameters,"") || !strcmp(bat_discharge_parameters,"") ) {
		printf("Rating, capacity, peukert number, charge efficiency factors and charge/discharge characterization parameters for the specific battery are needed.\n");
		printUsage(stderr);
		exit(EXIT_FAILURE);
	}

	if(!debug)
		daemonize(lock_file);

	bat_parse_parameters(bat_charge_parameters,0 /* charge/discharge switch */);
	bat_parse_parameters(bat_discharge_parameters,1 /* charge/discharge switch */);

	if(debug)
		bat_dump_tables();

	while(1) {
		bat_read_status( bat_status , bat_current , bat_voltage , bat_charging_current );

		bat_percentage = bat_get_percentage( bat_status , bat_voltage );

		if ( !strncmp(bat_status,"Char",4) )
			log("Battery %s: Capacity %d%% Time to full %fh.\n",
					bat_status,bat_percentage,bat_time_to_full(atoi(bat_charging_current),
							bat_capacity,bat_charge_efficiency,bat_percentage) );
		else if( !strncmp(bat_status,"Disc\n",4) || !strncmp(bat_status,"Not ",4) )
			log("Battery %s: Capacity %d%% Time to empty %fh.\n",
					bat_status,bat_percentage,bat_time_to_empty(bat_percentage,atoi(bat_current)/1000,
							bat_capacity,bat_peukert,bat_rating) );
		else if( !strncmp(bat_status,"Full",4) )
			log("Battery %s: Capacity 100%%\n",bat_status);
		else if( !strncmp(bat_status,"Unkn",4) )
			log("Battery %s\n",bat_status);
		else {
			log("Battery status ERROR %s\n",bat_status);
			continue;
		}
		sleep(60);
	}

	log("Battery monitoring daemon finished.\n");
	return retval;
}
