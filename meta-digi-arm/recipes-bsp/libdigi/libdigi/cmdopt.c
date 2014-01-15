/***********************************************************************
 *
 *  Copyright 2001,2002 by FS-Forth Systeme GmbH.
 *  All rights reserved.
 *
 *  $Id: cmdopt.c,v 1.4 2007-01-23 15:13:27 mpietrek Exp $
 *  @Author: Markus Pietrek
 *  @Descr: Provides some helper functions to access the command line
 *          options. The results are stored in global variables.
 *          This file is not thread safe.
 *
 ********************************************************************** */
/*********************************************************************** *
 *  @History:
 *     2002/10/21 : Purified for splint. Changed to new variable
 *                  naming convention.
 *
 *********************************************************************** */

#include <assert.h>
#include <ctype.h>		// toupper
#include <getopt.h>             // getopt
#include <stdio.h>
#include <stdlib.h>             // atoi
#include <string.h>		// strlen

#include "cmdopt.h"
#include "log.h"
#include "misc_helper.h"

#define BUFFER_SIZE 1024
#define MAX_ENTRIES 32

/*@observer@*/
void (*fnCmdOptExtendedUsage) (char bCmdLine) = NULL;
const char* szCmdOptVersion = "$Revision: 1.4 $"; // if not
						  // overwritten take
						  // at least the
						  // revision of cmdopt

static CmdOptEntry cmdOptEntries[ MAX_ENTRIES ]; // to avoid malloc we
						 // use a static buffer
/*@noreturn@*/
static void usageAndExit( int argc,
                          char* argv[],
                          const CmdOptEntry entries[],
                          const char* szDescr,
                          char cWrongNumber );
static void updateVar( const CmdOptEntry entries[],
                       int nOptIndex,
                       const char* szStr );


/***********************************************************************
 * @Function: cmdOptParse
 * @Return: only if command line could be parsed otherwise ends
 *          the application. Result is only set if COT_MORE is specified
 *          and points to the first unprocessed entry
 * @Descr: reads the command line and sets the variables. Displays a usage
 *         if values are wrong.
 ***********************************************************************/

int cmdOptParse( int argc,
                 char* argv[],
                 const CmdOptEntry entries[],
                 const char* szDescr )
{
        char szOptLine[ BUFFER_SIZE ];
        struct option axLongOptions[ MAX_ENTRIES ];
        int nOptIndex = 0;
        int nSize = 0;
        int nSizeLong = 0;
        signed char cOptChar;
	int nOptIndexStatic = 0;
	int nOptLongIndex = 0;
	char cPrintVersion = 0;
	char cPrintHelp = 0;

	CmdOptEntry coePrintVersion =
	    {COT_BOOL, -1, &cPrintVersion, "version", "print version and exit" };
	CmdOptEntry coePrintHelp =
	    {COT_BOOL, 'h', &cPrintHelp, "help", "print help" };
        CmdOptEntry coeLogLevel =
	    {COT_INT,  'l', &logLevel, "log-level", "log level for messages" };

        CLEAR( axLongOptions );

	// add global default command line arguments
	cmdOptEntries[ nOptIndexStatic++ ] = coePrintVersion;
	cmdOptEntries[ nOptIndexStatic++ ] = coePrintHelp;
	cmdOptEntries[ nOptIndexStatic++ ] = coeLogLevel;

	// copy default command line entries
	// will break when all entries are copied to global area
	// or too much arguments
        while( 1 )
	{
	    if( nOptIndexStatic == MAX_ENTRIES )
	    {
		logMsg( LOG_ERR, "cmdOptParse: too many command line options" );
		exit( EXIT_FAILURE );
	    }

	    cmdOptEntries[ nOptIndexStatic ] = entries[ nOptIndex ];
	    if( entries[ nOptIndex ].type == COT_NONE )
		// no more entries
		break;

	    nOptIndexStatic++;
	    nOptIndex++;
	}

        // create parameter list for getopt

	nOptIndex = 0;
        szOptLine[ nOptIndex ] = 0;

        while( cmdOptEntries[ nSize ].type != COT_NONE )
        {
            if( ( cmdOptEntries[ nSize ].type != COT_MORE ) &&
                ( cmdOptEntries[ nSize ].type != COT_MORE_OPT ) ) {
                    axLongOptions[ nSizeLong ].name    = cmdOptEntries[ nSize ].szLabelStr;
                    axLongOptions[ nSizeLong ].has_arg =
                            ((cmdOptEntries[ nSize ].type != COT_BOOL ) ? required_argument : no_argument);
                    axLongOptions[ nSizeLong ].flag    = NULL;
                    axLongOptions[ nSizeLong ].val     = -2 - nSize;
                    nSizeLong++;
            }

            if( NULL != cmdOptEntries[ nSize ].pbPresent )
                    *cmdOptEntries[ nSize ].pbPresent = 0;
            if( !cmdOptEntries[ nSize ].cOptChar )
	    {
                // parameter is not mandatory
                nSize++;
                continue;
            }

            if( nOptIndex >= BUFFER_SIZE - 2 )
            {
                logMsg( LOG_ERR,
		      "cmdOptParse: too long command line" );
                exit( EXIT_FAILURE );
            }

            if( -1 != cmdOptEntries[ nSize ].cOptChar ) {
                    szOptLine[ nOptIndex++ ] = cmdOptEntries[ nSize ].cOptChar;
                    if( cmdOptEntries[ nSize ].type != COT_BOOL )
                            szOptLine[ nOptIndex++ ] = ':';
            }

            szOptLine[ nOptIndex ] = 0;

            nSize++;
        }

        // parse optional command line options

        while( ( cOptChar = getopt_long( argc, argv, szOptLine, axLongOptions, &nOptLongIndex ) ) != -1 )
	{
            char cFound = 0;

            if( cOptChar < 0 ) {
                    cFound = 1;
                    updateVar( cmdOptEntries, -2 - cOptChar, optarg );
            } else {
                    for( nOptIndex = 0; nOptIndex < nSize; nOptIndex++ )
                    {
                            if( cmdOptEntries[ nOptIndex ].cOptChar == cOptChar )
                            {
                                    // optional arguments only
                                    cFound = 1;
                                    updateVar( cmdOptEntries, nOptIndex, optarg );
                            }
                    }
            }

            if( !cFound )
                usageAndExit( argc, argv, cmdOptEntries, szDescr, 0 );
        }

	if( cPrintVersion )
	{
	    fprintf( stdout, "%s %s\n",
		     argv[ 0 ],
		     szCmdOptVersion );
	    exit( EXIT_SUCCESS );
	}

	if( cPrintHelp )
	    usageAndExit( argc, argv, cmdOptEntries, szDescr, 0 );

        // parse mandatory command line options

        nOptIndex = 0;
        for( nOptIndex = 0; nOptIndex < nSize; nOptIndex++ )
            if( !cmdOptEntries[ nOptIndex ].cOptChar )
            {
                if( cmdOptEntries[ nOptIndex ].type == COT_MORE_OPT )
		    // don't parse the following arguments as they
		    // can't be covered by this lib yet
                    return optind;

                if( !( argc - optind ) )
                    usageAndExit( argc, argv, cmdOptEntries, szDescr, 1 );

                if( cmdOptEntries[ nOptIndex ].type == COT_MORE )
		    // don't parse the following arguments as they
		    // can't be covered by this lib yet
                    return optind;

                updateVar( cmdOptEntries, nOptIndex, argv[ optind++ ] );
            }

        if( argc - optind )
            // COT_MORE is aborted before
            usageAndExit( argc, argv, cmdOptEntries, szDescr, 1 );

        return 0;
}

void cmdOptUsageAndExit(
        int argc,
        char* argv[],
        const CmdOptEntry entries[],
        const char* szDescr )
{
        usageAndExit( argc, argv, entries, szDescr, 0 );
}


/***********************************************************************
 * @Function: usageAndExit
 * @Return: never
 * @Descr: displays usage and exits with EXIT_FAILURE. If wrongNumber is 1,
 *         a message "too many arguments is displayed".
 ***********************************************************************/

static void usageAndExit( /*@unused@*/ int argc,
                          char* argv[],
                          const CmdOptEntry entries[],
                          const char* szDescr,
                          char cWrongNumber )
{
        int nOptIndex = 0;
        size_t maxStrLen = 0;

        fprintf( stdout, "Usage: %s ", argv[ 0 ] );

        // print command line arguments
        while( entries[ nOptIndex ].type != COT_NONE )
        {
            if( entries[ nOptIndex ].szLabelStr != NULL )
            {
                // determine max len of labelStr for formatted output
                size_t strLen = strlen( entries[ nOptIndex ].szLabelStr );

                if( strLen > maxStrLen )
                    maxStrLen = strLen;
            }

            switch( entries[ nOptIndex ].type )
            {
                case COT_BOOL:
                case COT_INT:
                case COT_STRING:
                    fprintf( stdout, "[--%s] ", entries[ nOptIndex ].szLabelStr );
                    break;
                case COT_MORE:
                case COT_MORE_OPT:
                    fprintf( stdout,
			     "%s ",
			     entries[ nOptIndex ].szLabelStr );
                    break;
                case COT_NONE:
                    // will never happen but satisfies compiler
                    break;
            }
            nOptIndex++;
        }
        if( NULL != fnCmdOptExtendedUsage )
                fnCmdOptExtendedUsage( 1 );

        fprintf( stdout, "\n" );

        // explain command line options
        nOptIndex = 0;
        while( entries[ nOptIndex ].type != COT_NONE )
        {
            fprintf( stdout, "   " );
            fprintf( stdout, " %-*s",
                     (int) maxStrLen,
                    entries[ nOptIndex ].szLabelStr );

            if( entries[ nOptIndex ].cOptChar > 0)
                    fprintf( stdout, " [-%c]", entries[ nOptIndex ].cOptChar );
            else
                    fprintf( stdout, "     " );
	    /*@+matchanyintegral*/ // maxStrLen is size_t. We shall
				   // unrestrict for this case the
				   // behaviour of splint because the
				   // compiler should do some
				   // checking, too
	    /*@-matchanyintegral*/

            if( entries[ nOptIndex ].szHelpStr != NULL )
                fprintf( stdout, " : %s", entries[ nOptIndex ].szHelpStr );

            fprintf( stdout, "\n" );
            nOptIndex++;
        }

        if( NULL != fnCmdOptExtendedUsage )
                fnCmdOptExtendedUsage( 0 );

        fprintf( stdout, "\n%s\n", szDescr );

        // explain failure
        if( cWrongNumber )
            fprintf( stderr, "\n*** Wrong # arguments ***\n" );

        exit( EXIT_FAILURE );
}


/***********************************************************************
 * @Function: updateVar
 * @Return: nothing
 * @Descr: sets the variable in entries[ index ] to the value in str
 *         and does conversion if necessary
 ***********************************************************************/

static void updateVar( const CmdOptEntry entries[],
                       int nOptIndex,
                       const char* szStr )
{
        switch( entries[ nOptIndex ].type )
        {
            case COT_BOOL:
                *((char*) entries[ nOptIndex ].vValuePtr) = 1;
                break;
            case COT_INT:
		if( (int) strlen( szStr ) > 2 &&
		    ((szStr[ 0 ] == '0' && (toupper( szStr[ 1 ] ) == 'X' ))))
		{
		    sscanf( &szStr[ 2 ],
			    "%x",
			    ((int*) entries[ nOptIndex ].vValuePtr ) );
		    break;
		}
		*((int*) entries[ nOptIndex ].vValuePtr) = atoi( szStr );
                break;
            case COT_STRING:
                *((const char**) entries[ nOptIndex ].vValuePtr) = szStr;
                break;
            case COT_MORE:
            case COT_MORE_OPT:
            case COT_NONE:
                // will never happen but satisfies compiler
                break;
        }

        if( NULL != entries[ nOptIndex ].pbPresent )
                *entries[ nOptIndex ].pbPresent = 1;
}

