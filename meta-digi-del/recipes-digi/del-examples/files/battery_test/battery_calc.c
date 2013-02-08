/*
 * battery_calc.c
 *
 * Copyright (C) 2010 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: Simple battery characterization and time calculations.
 * Specific battery details should be used to modify these calculations.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "bat_test.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef struct {
	unsigned int voltage;
	unsigned int percent;
} POWER_CAPABILITY;

/* From MC13892 Users Guide, Table 8.2-2 */
static int main_charging_current_table[] = {0,80,240,320,400,480,560,640,720,800,880,960,1040,1200,1600,0xFFFFFFFF /* Fully Open */};

/* Characterization of the battery charge and discharge curves */
static POWER_CAPABILITY chargingTable[] = { { 0xffff, 100 }, { 0xffff, 95 }, {
		0xffff, 90 }, { 0xffff, 85 }, { 0xffff, 80 }, { 0xffff, 75 }, { 0xffff,
		70 }, { 0xffff, 65 }, { 0xffff, 60 }, { 0xffff, 55 }, { 0xffff, 50 }, {
		0xffff, 45 }, { 0xffff, 40 }, { 0xffff, 35 }, { 0xffff, 30 }, { 0xffff,
		25 }, { 0xffff, 20 }, { 0xffff, 15 }, { 0xffff, 10 }, { 0xffff, 5 }, {
		0xffff, 0 }, { 0, 0 } };

static POWER_CAPABILITY dischargingTable[] = { { 0xffff, 100 }, { 0xffff, 95 },
		{ 0xffff, 90 }, { 0xffff, 85 }, { 0xffff, 80 }, { 0xffff, 75 }, {
				0xffff, 70 }, { 0xffff, 65 }, { 0xffff, 60 }, { 0xffff, 55 }, {
				0xffff, 50 }, { 0xffff, 45 }, { 0xffff, 40 }, { 0xffff, 35 }, {
				0xffff, 30 }, { 0xffff, 25 }, { 0xffff, 20 }, { 0xffff, 15 }, {
				0xffff, 10 }, { 0xffff, 5 }, { 0xffff, 0 }, { 0, 0 } };

void bat_parse_parameters( char * parms_list , unsigned int cdswitch )
{
	POWER_CAPABILITY * pTable = cdswitch? dischargingTable:chargingTable;
	int i = 0;
	char * tmp = NULL;

	if( (tmp = strtok(parms_list,",")) )
		pTable[i++].voltage = (unsigned int)atoi( tmp );
	while( (tmp = strtok(NULL,",")) && (i < (ARRAY_SIZE(chargingTable)-1)) )
		pTable[i++].voltage = (unsigned int)atoi( tmp );
}

int bat_main_charging_current( unsigned int index )
{
	return ( main_charging_current_table[ index%sizeof(main_charging_current_table) ] );
}

static void bat_dump_table( POWER_CAPABILITY * pTable )
{
	int i;

	for (i=0 ; i < (ARRAY_SIZE(chargingTable)-1) ; i++ ) {
		printf("%d%%:%dmV\n",pTable[i].percent,pTable[i].voltage);
	}
}

void bat_dump_tables( void )
{
	POWER_CAPABILITY * pTable;

	pTable = chargingTable;
	printf("Charging table\n");
	bat_dump_table(pTable);

	pTable = dischargingTable;
	printf("Discharging table\n");
	bat_dump_table(pTable);
}

int bat_get_capacity ( POWER_SUPPLY_STATUS state , unsigned int voltage )
{
	int i;
	POWER_CAPABILITY * pTable;
	int tableSize;
	if ((state == POWER_SUPPLY_STATUS_DISCHARGING) || (state == POWER_SUPPLY_STATUS_NOT_CHARGING)) {
		pTable = dischargingTable;
		tableSize = sizeof(dischargingTable) / sizeof(dischargingTable[0]);
	} else {
		pTable = chargingTable;
		tableSize = sizeof(chargingTable) / sizeof(chargingTable[0]);
	}
	for (i = 0; i < tableSize; i++) {
		if (voltage >= pTable[i].voltage)
			return pTable[i].percent;
	}

	return 0;
}

int bat_get_voltage( POWER_SUPPLY_STATUS state , unsigned int capacity)
{
	int i;
	POWER_CAPABILITY * pTable;
	int tableSize;

	if ((state == POWER_SUPPLY_STATUS_DISCHARGING) || (state == POWER_SUPPLY_STATUS_NOT_CHARGING)) {
		pTable = dischargingTable;
		tableSize = sizeof(dischargingTable) / sizeof(dischargingTable[0]);
	} else {
		pTable = chargingTable;
		tableSize = sizeof(chargingTable) / sizeof(chargingTable[0]);
	}

	for (i = 0; i < tableSize; i++) {
		if (capacity >= pTable[i].percent)
			return pTable[i].voltage;
	}

	return 0;
}

/* Use a modified Peukert equation to use the battery hour rating and approximate the time to empty
* T = SxC/(I/(C/R))n x (R/C)
* Where:
* I = the discharge current in mA
* T = the time in hours
* C = capacity of the battery in mAh
* S = charge state (percentage of capacity)
* n = Peukert's exponent for that particular battery type
* R = the battery hour rating, i.e. 100 hour rating, 20 hour rating, 10 hour rating etc. Usually 20 hour rating.
*/
float bat_time_to_empty( unsigned int bat_percentage , unsigned int bat_current , unsigned int bat_capacity , float bat_peukert , unsigned int bat_rating )
{
	return ( (float) bat_percentage/100*(float)bat_capacity/((float)bat_current/((float)bat_capacity/(float)bat_rating))*bat_peukert * ((float)bat_rating/(float)bat_capacity) );
}

/* Use a simple equation to approximate the time to full
 *
 * T = DxnxR/I
* Where:
* I = the charge current in mA
* T = the time in hours
* D = Discharge state (100 - capacity)
* n = The charge efficiency
* C = The capacity in mAh
 */
float bat_time_to_full( unsigned int bat_charging_current , unsigned int bat_capacity , float bat_charge_efficiency , unsigned int bat_percentage )
{
	float bat_discharge_state = (100 - (float)bat_percentage)/100;
	return((float)(bat_discharge_state)*bat_charge_efficiency*(float)bat_capacity/(float)bat_charging_current);
}

