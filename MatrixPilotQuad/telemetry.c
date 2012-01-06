// This file is part of the MatrixPilotQuad firmware.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.

#include "../libDCM/libDCM.h"

// Used for serial debug output
#include "stdio.h"


char debug_buffer[128] ;
int db_index = 0 ;
boolean hasWrittenHeader = 0 ;
int header_line = 0 ;

extern int theta[3] , roll_control , pitch_control , yaw_control , accelEarth[3] , accel_feedback ;
extern int commanded_roll, commanded_pitch, commanded_yaw, pwManual[] ;
extern int roll_error , pitch_error ;
extern union longww roll_error_integral, pitch_error_integral ;

volatile int trap_flags __attribute__ ((persistent));
volatile long trap_source __attribute__ ((persistent));
volatile int osc_fail_count __attribute__ ((persistent));


// Prepare a line of serial output and start it sending
void send_debug_line( void )
{
	db_index = 0 ;
	
	if (!hasWrittenHeader)
	{
		header_line ++ ;
		switch ( header_line ) {
		case 1:
			sprintf(debug_buffer, "\r\n") ;
			break ;
		case 2:
			sprintf(debug_buffer, "ROLL_KP = %5f, PITCH_KP = %5f\r\n" ,
				ROLL_KP ,
				PITCH_KP  ) ;
			break ;	
		case 3:
			sprintf(debug_buffer, "ROLL_KI = %5f, PITCH_KI = %5f\r\n" ,
				ROLL_KI ,
				PITCH_KI  ) ;
			break ;
		case 4:
			sprintf(debug_buffer, "ROLL_KD = %5f, PITCH_KD = %5f, YAW_KD = %5f\r\n" ,
				ROLL_KD ,
				PITCH_KD ,
				YAW_KD ) ;
			break ;
		case 5:
			sprintf(debug_buffer, "ROLL_KDD = %5f, PITCH_KDD = %5f\r\nACCEL_K = %5f\r\n" ,
				ROLL_KDD ,
				PITCH_KDD ,
				ACCEL_K ) ;
			break ;
		case 6:
			sprintf(debug_buffer, "r6 , r7 , w0 , w1 , w2 , rfb , pfb , yfb , rerr, rerrI , perr, perrI , rcmd , pcmd, ycmd, thr , acc2 , accfb\r\n" ) ;
			hasWrittenHeader = 1 ;			
			break ;
		default:
			hasWrittenHeader = 1 ;
			break ;
		}
	}
	else
	{
		sprintf(debug_buffer, "%i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i\r\n" ,
			rmat[6] , rmat[7] , 
			theta[0] , theta[1] , theta[2] , 
			roll_control , pitch_control, yaw_control ,
			roll_error , roll_error_integral._.W1 , pitch_error , pitch_error_integral._.W1 ,
			commanded_roll , commanded_pitch , commanded_yaw , pwManual[THROTTLE_INPUT_CHANNEL] ,
			accelEarth[2] , accel_feedback ) ;
	}
	
	udb_serial_start_sending_data() ;
	
	return ;
}

/*
extern int gplane[] ;
// Prepare a line of serial output and start it sending
void send_debug_line( void )
{
	db_index = 0 ;
	
	if (!hasWrittenHeader)
	{
		sprintf(debug_buffer, "w0 , w1 , w2 , a0 , a1 , a2\r\n") ;
		hasWrittenHeader = 1 ;
	}
	else
	{
		sprintf(debug_buffer, "%i , %i , %i , %i , %i , %i\r\n" ,
		omegagyro[0] , 	omegagyro[1] , omegagyro[2] ,
		gplane[0] , gplane[1] , gplane[2]
		 ) ;
	}
	
	udb_serial_start_sending_data() ;
	
	return ;
}
*/

// Return one character at a time, as requested.
// Requests will stop after we send back a -1 end-of-data marker.
int udb_serial_callback_get_byte_to_send(void)
{
	unsigned char c = debug_buffer[ db_index++ ] ;
	
	if (c == 0) return -1 ;
	
	return c ;
}


// Don't respond to serial input
void udb_serial_callback_received_byte(char rxchar)
{
	// Do nothing
	return ;
}


