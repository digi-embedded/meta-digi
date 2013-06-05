/*
 * bat_test.h
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


#ifndef BAT_TEST_H_
#define BAT_TEST_H_

typedef enum {
	POWER_SUPPLY_STATUS_UNKNOWN = 0,
	POWER_SUPPLY_STATUS_CHARGING,
	POWER_SUPPLY_STATUS_DISCHARGING,
	POWER_SUPPLY_STATUS_NOT_CHARGING,
	POWER_SUPPLY_STATUS_FULL,
}POWER_SUPPLY_STATUS;

void bat_parse_parameters( char * parms_list , unsigned int cdswitch );
void bat_dump_tables( void );
int bat_get_capacity(POWER_SUPPLY_STATUS status, unsigned int voltage);
int bat_get_voltage( POWER_SUPPLY_STATUS state , unsigned int capacity);
float bat_time_to_empty( unsigned int bat_percentage , unsigned int bat_current , unsigned int bat_capacity , float bat_peukert , unsigned int bat_rating );
float bat_time_to_full( unsigned int bat_current , unsigned int bat_capacity , float bat_charge_efficiency , unsigned int bat_percentage );
int bat_main_charging_current( unsigned int index );

#endif /* BAT_TEST_H_ */
