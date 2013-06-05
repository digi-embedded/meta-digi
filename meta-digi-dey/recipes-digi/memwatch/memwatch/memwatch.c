/*
 * memwatch.c: Program to read/write from/to memory.
 *
 * Copyright (C) 2006 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define	PROGRAM		"memwatch"
#define VERSION		"v1.2"
#define MEM_DEV_FILE	"/dev/mem"

#define MAP_SIZE	4096UL
#define MAP_MASK	(MAP_SIZE - 1)

#define VERB_PRNT_ADDR	1

typedef enum {
	BYTE = 1,
	WORD = 2,
	DWORD = 4
}access_t;


/* Global variables */
static int fd = -1;
static int fd_file = -1;

/***********************************************************************
 * Function: release_resources
 * Return: Nothing.
 * Descr: Releases the used system resources.
 ***********************************************************************/
static void release_resources( void )
{
	if( fd != -1 )
		close( fd );
	if( fd_file != -1 )
		close( fd_file );
}

/***********************************************************************
 * Function: release_resources_and_exit
 * Return: Nothing.
 * Descr: Releases the used system resources and exits with exitval.
 ***********************************************************************/
static void release_resources_and_exit( int exitval )
{
	release_resources();
	exit( exitval );
}

/***********************************************************************
 * Function: show_usage_and_exit
 * Return: Nothing.
 * Descr: Shows usage information and exits with the value passed as
 *        parameter.
 ***********************************************************************/
static void show_usage_and_exit( int exitval )
{
	printf("Usage: %s [-rxbhw] -a address [-d value] [-f file] [-v level]\n"	\
	       "%s %s Copyright Digi International Inc.\n\n"				\
	       "Utility to Read/Writes physical memory\n"				\
	       "\n"									\
	       "  -r, --read        Read from memory\n"					\
	       "  -x, --write       Write to memory\n"					\
	       "  -b  --byte        Byte access\n"					\
	       "  -h  --halfw       Half word access\n"					\
	       "  -w  --word        Word access\n"					\
	       "  -l  --len         Number of bytes to read/write\n"			\
	       "  -a  --address     Start Address of physical memory\n"			\
	       "  -d  --data        Data value to write to memory address\n"		\
	       "  -f  --file        File to read/write\n"				\
	       "  -v  --verbose     Verbosity level\n"					\
	       "      --help        Print help and exit\n"				\
	       "      --version     Print version and exit\n"				\
	       "\n", PROGRAM, PROGRAM, VERSION );
	exit( exitval );
}

/***********************************************************************
 * Function: print_hex_formated
 * Return: Nothing.
 * Descr: Prints hex data formated accord to the data length.
 ***********************************************************************/
static void print_hex_formated( unsigned long val, access_t access )
{
	switch( access ) {
		case BYTE:	printf( "0x%02x ", (unsigned int)val );	break;
		case WORD:	printf( "0x%04x ", (unsigned int)val );	break;
		case DWORD:	printf( "0x%08x ", (unsigned int)val );	break;
	}
}


/***********************************************************************
 * @Function: main
 * @Return:
 * @Descr:    Main function.
 ***********************************************************************/
int main( int argc, char** argv )
{
	int read_write = 1, len = -1, pending, chunck, verbose = 0, j;
	access_t access = DWORD;
	int ret;
	unsigned long wr_val, rd_val;
	void *map_base, *virt_addr;
	off_t address = 0;
	char *filename = NULL;
	struct stat filestat;

	/*Command line vars*/
	static int version	= 0;
	static int help	= 0;

	static int opt_index, opt, optcount = 0;
	static const char *short_options = "rxbhwl:a:d:f:v:";
	static const struct option long_options[] = {
		{ "version", no_argument, &version, 1 },
		{ "help",    no_argument, &help, 1 },
		{ "read",    no_argument, NULL, 'r' },
		{ "write",   no_argument, NULL, 'x' },
		{ "byte",    no_argument, NULL, 'b' },
		{ "halfw",   no_argument, NULL, 'h' },
		{ "word",    no_argument, NULL, 'w' },
		{ "len",     required_argument, NULL, 'l' },
		{ "address", required_argument, NULL, 'a' },
		{ "data",    required_argument, NULL, 'd' },
		{ "file",    required_argument, NULL, 'f' },
		{ "verbose", required_argument, NULL, 'v' },
		{ 0, 0, 0, 0 },
	};

	for( opt_index = 0; ; ) {

		opt = getopt_long( argc, argv, short_options,
				   long_options, &opt_index );
		if( opt == EOF )
			break;

		if( 0 == opt ) {
			if( version ) {
				printf( "%s %s, compiled on %s, %s\n", PROGRAM, VERSION, __DATE__, __TIME__ );
				exit( EXIT_SUCCESS );
			}

			if( help )
				show_usage_and_exit( EXIT_SUCCESS );
		}

		switch( opt ) {
			case 'r': read_write = 1;			break;
			case 'x': read_write = 0;			break;
			case 'b': access = BYTE;			break;
			case 'h': access = WORD;			break;
			case 'w': access = DWORD;			break;
			case 'l': len = atoi( optarg );			break;
			case 'a': address = strtoul( optarg, 0, 0 );	break;	/*Start address*/
			case 'd': wr_val = strtoul( optarg, 0, 0 );	break;	/*Value to write*/
			case 'f': filename = optarg;			break;	/*file where to read/write mem info*/
			case 'v': verbose = atoi( optarg );		break;
			case '?': show_usage_and_exit( EXIT_FAILURE );	break;
		}
		optcount++;
	}

	if ( optcount == 0 )
		show_usage_and_exit( EXIT_FAILURE );

	if( len == -1 )
		len = (int)access;

	if( ( fd = open( MEM_DEV_FILE, O_RDWR | O_SYNC ) ) < 0 ) {
		fprintf( stderr, "Error opening mem file: %s\n", MEM_DEV_FILE );
		exit( EXIT_FAILURE );
	}

	if( filename != NULL ) {
		if( ( fd_file = open( filename, O_RDWR | O_CREAT ) ) < 0 ) {
			fprintf( stderr, "Error opening file: %s (%d)\n",
				filename, errno );
			release_resources_and_exit( EXIT_FAILURE );
		}

		if( !read_write ) {
			if( fstat( fd_file, &filestat ) != 0 ) {
				fprintf( stderr, "fstat error on %s (%d)\n",
					filename, errno );
 				release_resources_and_exit( EXIT_FAILURE );
			}
			len = (int)filestat.st_size;
		}
	}

	/* Check parameters consistency */
	// @TODO

	pending = len;
	while( pending ) {
		chunck = (pending > MAP_SIZE) ? MAP_SIZE : pending;

		/* Map one page maximum */
		map_base = mmap( 0, MAP_SIZE, PROT_READ | PROT_WRITE,
				 MAP_SHARED, fd, address & ~MAP_MASK );

		if( map_base == (void *) -1 ) {
			fprintf( stderr, "Error mapping memory\n" );
			release_resources_and_exit( EXIT_FAILURE );
		}

		virt_addr = map_base + (address & MAP_MASK);

		for( j = 0; j < chunck; j += access ) {
			if( read_write ) {
				switch( access ) {
					case BYTE:	rd_val = *((unsigned char *) virt_addr);	break;
					case WORD:	rd_val = *((unsigned short *) virt_addr);	break;
					case DWORD:	rd_val = *((unsigned long *) virt_addr);	break;
				}
				if( filename != NULL ) {
					if( write( fd_file, (unsigned char *)&rd_val, access ) != access ) {
						fprintf( stderr, "Error writing in %s (%d)\n",
							filename, errno );
						release_resources_and_exit( EXIT_FAILURE );
					}
				}

				if( ( verbose < VERB_PRNT_ADDR ) && ( j % 16 == 0 ) ) {
					printf( "\n0x%08lx: ", address + j );
				}
				print_hex_formated( rd_val, access );

			} else {
				if( filename != NULL ) {
					if( ( ret = read( fd_file, &wr_val, access ) ) != access ) {
						fprintf( stderr, "Error reading from %s (%d)\n",
							filename, errno );
						release_resources_and_exit( EXIT_FAILURE );
					}
				}
				switch( access ) {
					case BYTE:	*((unsigned char *) virt_addr) = wr_val;	break;
					case WORD:	*((unsigned short *) virt_addr) = wr_val;	break;
					case DWORD:	*((unsigned long *) virt_addr) = wr_val;	break;
				}
			}
			virt_addr += access;
		}

		if( munmap( map_base, MAP_SIZE ) == -1 ) {
			fprintf( stderr, "Error unmapping memory\n" );
			release_resources_and_exit( EXIT_FAILURE );
		}
		pending -= chunck;
		address += chunck;
	}

	release_resources();

	return 0;
}
