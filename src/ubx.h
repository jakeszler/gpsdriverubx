/****************************************************************************
 *
 *   Copyright (c) 2012-2016 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file ubx.h
 *
 * U-Blox protocol definition. Following u-blox 6/7/8 Receiver Description
 * including Prototol Specification.
 *
 * @author Thomas Gubler <thomasgubler@student.ethz.ch>
 * @author Julian Oes <julian@oes.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 *
 * @author Hannes Delago
 *   (rework, add ubx7+ compatibility)
 *
 */

#ifndef UBX_H_ //was UBX_
#define UBX_H_



#define DEFAULT_UART "/dev/ttyS3"
#define MAX_MESSAGE_BUFFER_SIZE 45
#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include <math.h>

#include <systemlib/err.h>
#include <systemlib/systemlib.h>

#include "comms.h"
#include "messages.h"

#include <stdint.h>

#include <px4_tasks.h>
#include <px4_getopt.h>
#include <px4_posix.h>
#include <errno.h>
#include <string.h>

#include <termios.h>
//#include <px4_log.h>
#include <px4_time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <mathlib/mathlib.h>
#include "sbp.h"
#include "navigation.h"
#include "tutorial_implementation.h"
//#include "tutorial_implementation.c"

#include "edc.h"



#define MAVLINK_MESSAGE_LENGTHS {9, 31, 12, 0, 14, 28, 3, 32, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 20, 2, 25, 23, 30, 101, 22, 26, 16, 14, 28, 32, 28, 28, 22, 22, 21, 6, 6, 37, 4, 4, 2, 2, 4, 2, 2, 3, 13, 12, 37, 4, 0, 0, 27, 25, 0, 0, 0, 0, 0, 72, 26, 181, 225, 42, 6, 4, 0, 11, 18, 0, 0, 37, 20, 35, 33, 3, 0, 0, 0, 22, 39, 37, 53, 51, 53, 51, 0, 28, 56, 42, 33, 81, 0, 0, 0, 0, 0, 0, 26, 32, 32, 20, 32, 62, 44, 64, 84, 9, 254, 16, 12, 36, 44, 64, 22, 6, 14, 12, 97, 2, 2, 113, 35, 6, 79, 35, 35, 22, 13, 255, 14, 18, 43, 8, 22, 14, 36, 43, 41, 32, 243, 14, 93, 0, 100, 36, 60, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 40, 63, 182, 40, 0, 0, 0, 0, 0, 0, 32, 52, 53, 6, 2, 38, 19, 254, 36, 30, 18, 18, 51, 9, 0}
#define MAVLINK_MESSAGE_CRCS {50, 124, 137, 0, 237, 217, 104, 119, 0, 0, 0, 89, 0, 0, 0, 0, 0, 0, 0, 0, 214, 159, 220, 168, 24, 23, 170, 144, 67, 115, 39, 246, 185, 104, 237, 244, 222, 212, 9, 254, 230, 28, 28, 132, 221, 232, 11, 153, 41, 39, 78, 196, 0, 0, 15, 3, 0, 0, 0, 0, 0, 167, 183, 119, 191, 118, 148, 21, 0, 243, 124, 0, 0, 38, 20, 158, 152, 143, 0, 0, 0, 106, 49, 22, 143, 140, 5, 150, 0, 231, 183, 63, 54, 47, 0, 0, 0, 0, 0, 0, 175, 102, 158, 208, 56, 93, 138, 108, 32, 185, 84, 34, 174, 124, 237, 4, 76, 128, 56, 116, 134, 237, 203, 250, 87, 203, 220, 25, 226, 46, 29, 223, 85, 6, 229, 203, 1, 195, 109, 168, 181, 47, 72, 131, 127, 0, 103, 154, 178, 200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 163, 105, 151, 35, 150, 0, 0, 0, 0, 0, 0, 90, 104, 85, 95, 130, 184, 81, 8, 204, 49, 170, 44, 83, 46, 0}

#include "mavlink_types.h"

#include <uORB/topics/vehicle_gps_position.h>




#define MAX_LEN_DEV_PATH 32


 int px4_simple_app_main(int argc, char *argv[]);




static const uint8_t mavlink_message_lengths[256] = MAVLINK_MESSAGE_LENGTHS;
static const uint8_t mavlink_message_crcs[256] = MAVLINK_MESSAGE_CRCS;

char _device[MAX_LEN_DEV_PATH];

 int _uart_fd = -1;
 bool _flow_control_enabled = false;

 bool thread_should_exit = false;		/**< daemon exit flag */
 bool thread_running = false;		/**< daemon status flag */
 int daemon_task;				/**< Handle of daemon task / thread */




int initialise_uart(void);
int enable_flow_control(bool enabled);
int deinitialize_uart(void);


//int piksi_uart(int argc, char *argv[]);
/*
 * State of the SBP message parser.
 * Must be statically allocated.
 */
sbp_state_t sbp_state;

/* SBP structs that messages from Piksi will feed. */
msg_pos_llh_t      pos_llh;
msg_baseline_ned_t baseline_ned;
msg_vel_ned_t      vel_ned;
msg_dops_t         dops;
msg_gps_time_t     gps_time;

/*
 * SBP callback nodes must be statically allocated. Each message ID / callback
 * pair must have a unique sbp_msg_callbacks_node_t associated with it.
 */
sbp_msg_callbacks_node_t pos_llh_node;
sbp_msg_callbacks_node_t baseline_ned_node;
sbp_msg_callbacks_node_t vel_ned_node;
sbp_msg_callbacks_node_t dops_node;
sbp_msg_callbacks_node_t gps_time_node;

/*
 * Callback functions to interpret SBP messages.
 * Every message ID has a callback associated with it to
 * receive and interpret the message payload.
 */

void sbp_pos_llh_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
void sbp_baseline_ned_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
void sbp_vel_ned_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
void sbp_dops_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
void sbp_gps_time_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
void sbp_setup(void);
int initialise_uart(void);




#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctime>

#include "gps_helper.h"
#include "../../definitions.h"

//#include "px4_simple_app.h"
//#include "px4_simple_app.c"
//#include "px4_simple_app.c"
// #include <px4_config.h>
// #include <px4_tasks.h>
// #include <px4_posix.h>
// #include <unistd.h>
 //#include <stdio.h>
// #include <poll.h>
// #include <string.h>
// #include <math.h>

// #include <systemlib/err.h>
// #include <systemlib/systemlib.h>

// #include "comms.h"
// #include "messages.h"

//  #include <stdint.h>

// #include <px4_tasks.h>
// #include <px4_getopt.h>
// #include <px4_posix.h>
// #include <errno.h>


//  #include <termios.h>
// #include <stdint.h>
// #include <stdbool.h>
// #include <inttypes.h>
// //#include <px4_log.h>
// #include <px4_time.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <pthread.h>
// #include <mathlib/mathlib.h>



// #include "sbp.h"
// #include "navigation.h"
// #include "tutorial_implementation.h"
// #include "tutorial_implementation.c"
// #include "edc.h"

// #include "mavlink_types.h"

#define UBX_SYNC1 0xB5
#define UBX_SYNC2 0x62

/* Message Classes */
#define UBX_CLASS_NAV		0x01
#define UBX_CLASS_INF		0x04
#define UBX_CLASS_ACK		0x05
#define UBX_CLASS_CFG		0x06
#define UBX_CLASS_MON		0x0A
#define UBX_CLASS_RTCM3	0xF5 /**< This is undocumented (?) */

/* Message IDs */
#define UBX_ID_NAV_POSLLH	0x02
#define UBX_ID_NAV_DOP		0x04
#define UBX_ID_NAV_SOL		0x06
#define UBX_ID_NAV_PVT		0x07
#define UBX_ID_NAV_VELNED	0x12
#define UBX_ID_NAV_TIMEUTC	0x21
#define UBX_ID_NAV_SVINFO	0x30
#define UBX_ID_NAV_SAT		0x35
#define UBX_ID_NAV_SVIN  	0x3B
#define UBX_ID_NAV_RELPOSNED  	0x3C
#define UBX_ID_INF_DEBUG  	0x04
#define UBX_ID_INF_ERROR  	0x00
#define UBX_ID_INF_NOTICE  	0x02
#define UBX_ID_INF_WARNING 	0x01
#define UBX_ID_ACK_NAK		0x00
#define UBX_ID_ACK_ACK		0x01
#define UBX_ID_CFG_PRT		0x00
#define UBX_ID_CFG_MSG		0x01
#define UBX_ID_CFG_RATE		0x08
#define UBX_ID_CFG_NAV5		0x24
#define UBX_ID_CFG_SBAS		0x16
#define UBX_ID_CFG_TMODE3	0x71
#define UBX_ID_MON_VER		0x04
#define UBX_ID_MON_HW		0x09
#define UBX_ID_RTCM3_1005	0x05
#define UBX_ID_RTCM3_1077	0x4D
#define UBX_ID_RTCM3_1087	0x57

/* Message Classes & IDs */
#define UBX_MSG_NAV_POSLLH	((UBX_CLASS_NAV) | UBX_ID_NAV_POSLLH << 8)
#define UBX_MSG_NAV_SOL		((UBX_CLASS_NAV) | UBX_ID_NAV_SOL << 8)
#define UBX_MSG_NAV_DOP		((UBX_CLASS_NAV) | UBX_ID_NAV_DOP << 8)
#define UBX_MSG_NAV_PVT		((UBX_CLASS_NAV) | UBX_ID_NAV_PVT << 8)
#define UBX_MSG_NAV_VELNED	((UBX_CLASS_NAV) | UBX_ID_NAV_VELNED << 8)
#define UBX_MSG_NAV_TIMEUTC	((UBX_CLASS_NAV) | UBX_ID_NAV_TIMEUTC << 8)
#define UBX_MSG_NAV_SVINFO	((UBX_CLASS_NAV) | UBX_ID_NAV_SVINFO << 8)
#define UBX_MSG_NAV_SAT	((UBX_CLASS_NAV) | UBX_ID_NAV_SAT << 8)
#define UBX_MSG_NAV_SVIN	((UBX_CLASS_NAV) | UBX_ID_NAV_SVIN << 8)
#define UBX_MSG_NAV_RELPOSNED	((UBX_CLASS_NAV) | UBX_ID_NAV_RELPOSNED << 8)
#define UBX_MSG_INF_DEBUG	((UBX_CLASS_INF) | UBX_ID_INF_DEBUG << 8)
#define UBX_MSG_INF_ERROR	((UBX_CLASS_INF) | UBX_ID_INF_ERROR << 8)
#define UBX_MSG_INF_NOTICE	((UBX_CLASS_INF) | UBX_ID_INF_NOTICE << 8)
#define UBX_MSG_INF_WARNING	((UBX_CLASS_INF) | UBX_ID_INF_WARNING << 8)
#define UBX_MSG_ACK_NAK		((UBX_CLASS_ACK) | UBX_ID_ACK_NAK << 8)
#define UBX_MSG_ACK_ACK		((UBX_CLASS_ACK) | UBX_ID_ACK_ACK << 8)
#define UBX_MSG_CFG_PRT		((UBX_CLASS_CFG) | UBX_ID_CFG_PRT << 8)
#define UBX_MSG_CFG_MSG		((UBX_CLASS_CFG) | UBX_ID_CFG_MSG << 8)
#define UBX_MSG_CFG_RATE	((UBX_CLASS_CFG) | UBX_ID_CFG_RATE << 8)
#define UBX_MSG_CFG_NAV5	((UBX_CLASS_CFG) | UBX_ID_CFG_NAV5 << 8)
#define UBX_MSG_CFG_SBAS	((UBX_CLASS_CFG) | UBX_ID_CFG_SBAS << 8)
#define UBX_MSG_CFG_TMODE3	((UBX_CLASS_CFG) | UBX_ID_CFG_TMODE3 << 8)
#define UBX_MSG_MON_HW		((UBX_CLASS_MON) | UBX_ID_MON_HW << 8)
#define UBX_MSG_MON_VER		((UBX_CLASS_MON) | UBX_ID_MON_VER << 8)
#define UBX_MSG_RTCM3_1005	((UBX_CLASS_RTCM3) | UBX_ID_RTCM3_1005 << 8)
#define UBX_MSG_RTCM3_1077	((UBX_CLASS_RTCM3) | UBX_ID_RTCM3_1077 << 8)
#define UBX_MSG_RTCM3_1087	((UBX_CLASS_RTCM3) | UBX_ID_RTCM3_1087 << 8)

/* RX NAV-PVT message content details */
/*   Bitfield "valid" masks */
#define UBX_RX_NAV_PVT_VALID_VALIDDATE		0x01	/**< validDate (Valid UTC Date) */
#define UBX_RX_NAV_PVT_VALID_VALIDTIME		0x02	/**< validTime (Valid UTC Time) */
#define UBX_RX_NAV_PVT_VALID_FULLYRESOLVED	0x04	/**< fullyResolved (1 = UTC Time of Day has been fully resolved (no seconds uncertainty)) */

/*   Bitfield "flags" masks */
#define UBX_RX_NAV_PVT_FLAGS_GNSSFIXOK		0x01	/**< gnssFixOK (A valid fix (i.e within DOP & accuracy masks)) */
#define UBX_RX_NAV_PVT_FLAGS_DIFFSOLN		0x02	/**< diffSoln (1 if differential corrections were applied) */
#define UBX_RX_NAV_PVT_FLAGS_PSMSTATE		0x1C	/**< psmState (Power Save Mode state (see Power Management)) */
#define UBX_RX_NAV_PVT_FLAGS_HEADVEHVALID	0x20	/**< headVehValid (Heading of vehicle is valid) */
#define UBX_RX_NAV_PVT_FLAGS_CARRSOLN		0xC0	/**< Carrier phase range solution (RTK mode) */

/* RX NAV-TIMEUTC message content details */
/*   Bitfield "valid" masks */
#define UBX_RX_NAV_TIMEUTC_VALID_VALIDTOW	0x01	/**< validTOW (1 = Valid Time of Week) */
#define UBX_RX_NAV_TIMEUTC_VALID_VALIDKWN	0x02	/**< validWKN (1 = Valid Week Number) */
#define UBX_RX_NAV_TIMEUTC_VALID_VALIDUTC	0x04	/**< validUTC (1 = Valid UTC Time) */
#define UBX_RX_NAV_TIMEUTC_VALID_UTCSTANDARD	0xF0	/**< utcStandard (0..15 = UTC standard identifier) */

/* TX CFG-PRT message contents */
#define UBX_TX_CFG_PRT_PORTID		0x01		/**< UART1 */
#define UBX_TX_CFG_PRT_PORTID_USB	0x03		/**< USB */
#define UBX_TX_CFG_PRT_PORTID_SPI	0x04		/**< SPI */
#define UBX_TX_CFG_PRT_MODE		0x000008D0	/**< 0b0000100011010000: 8N1 */
#define UBX_TX_CFG_PRT_MODE_SPI	0x00000100
#define UBX_TX_CFG_PRT_BAUDRATE		38400		/**< choose 38400 as GPS baudrate */
#define UBX_TX_CFG_PRT_INPROTOMASK_GPS	((1<<5) | 0x01)	/**< RTCM3 in and UBX in */
#define UBX_TX_CFG_PRT_INPROTOMASK_RTCM	(0x01)	/**< UBX in */
#define UBX_TX_CFG_PRT_OUTPROTOMASK_GPS	(0x01)			/**< UBX out */
#define UBX_TX_CFG_PRT_OUTPROTOMASK_RTCM	((1<<5) | 0x01)		/**< RTCM3 out and UBX out */

/* TX CFG-RATE message contents */
#define UBX_TX_CFG_RATE_MEASINTERVAL	200		/**< 200ms for 5Hz */
#define UBX_TX_CFG_RATE_NAVRATE		1		/**< cannot be changed */
#define UBX_TX_CFG_RATE_TIMEREF		0		/**< 0: UTC, 1: GPS time */

/* TX CFG-NAV5 message contents */
#define UBX_TX_CFG_NAV5_MASK		0x0005		/**< Only update dynamic model and fix mode */
#define UBX_TX_CFG_NAV5_FIXMODE		2		/**< 1 2D only, 2 3D only, 3 Auto 2D/3D */

/* TX CFG-SBAS message contents */
#define UBX_TX_CFG_SBAS_MODE_ENABLED	1				/**< SBAS enabled */
#define UBX_TX_CFG_SBAS_MODE_DISABLED	0				/**< SBAS disabled */
#define UBX_TX_CFG_SBAS_MODE		UBX_TX_CFG_SBAS_MODE_DISABLED	/**< SBAS enabled or disabled */

/* TX CFG-TMODE3 message contents */
#define UBX_TX_CFG_TMODE3_FLAGS     	1 	    	/**< start survey-in */
#define UBX_TX_CFG_TMODE3_SVINMINDUR    (3*60)		/**< survey-in: minimum duration [s] (higher=higher precision) */
#define UBX_TX_CFG_TMODE3_SVINACCLIMIT  (10000)	/**< survey-in: position accuracy limit 0.1[mm] */

/* RTCM3 */
#define RTCM3_PREAMBLE					0xD3
#define RTCM_INITIAL_BUFFER_LENGTH			300		/**< initial maximum message length of an RTCM message */

typedef struct {
	uint8_t			*buffer;
	uint16_t		buffer_len;
	uint16_t		pos;						///< next position in buffer
	uint16_t		message_length;					///< message length without header & CRC (both 3 bytes)
} rtcm_message_t;


/*** u-blox protocol binary message and payload definitions ***/
#pragma pack(push, 1)

/* General: Header */
typedef struct {
	uint8_t		sync1;
	uint8_t		sync2;
	uint16_t	msg;
	uint16_t	length;
} ubx_header_t;

/* General: Checksum */
typedef struct {
	uint8_t		ck_a;
	uint8_t		ck_b;
} ubx_checksum_t ;

/* Rx NAV-POSLLH */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	int32_t		lon;		/**< Longitude [1e-7 deg] */
	int32_t		lat;		/**< Latitude [1e-7 deg] */
	int32_t		height;		/**< Height above ellipsoid [mm] */
	int32_t		hMSL;		/**< Height above mean sea level [mm] */
	uint32_t	hAcc;  		/**< Horizontal accuracy estimate [mm] */
	uint32_t	vAcc;  		/**< Vertical accuracy estimate [mm] */
} ubx_payload_rx_nav_posllh_t;

/* Rx NAV-DOP */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	uint16_t	gDOP;		/**< Geometric DOP [0.01] */
	uint16_t	pDOP;		/**< Position DOP [0.01] */
	uint16_t	tDOP;		/**< Time DOP [0.01] */
	uint16_t	vDOP;		/**< Vertical DOP [0.01] */
	uint16_t	hDOP;		/**< Horizontal DOP [0.01] */
	uint16_t	nDOP;		/**< Northing DOP [0.01] */
	uint16_t	eDOP;		/**< Easting DOP [0.01] */
} ubx_payload_rx_nav_dop_t;

/* Rx NAV-SOL */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	int32_t		fTOW;		/**< Fractional part of iTOW (range: +/-500000) [ns] */
	int16_t		week;		/**< GPS week */
	uint8_t		gpsFix;		/**< GPSfix type: 0 = No fix, 1 = Dead Reckoning only, 2 = 2D fix, 3 = 3d-fix, 4 = GPS + dead reckoning, 5 = time only fix */
	uint8_t		flags;
	int32_t		ecefX;
	int32_t		ecefY;
	int32_t		ecefZ;
	uint32_t	pAcc;
	int32_t		ecefVX;
	int32_t		ecefVY;
	int32_t		ecefVZ;
	uint32_t	sAcc;
	uint16_t	pDOP;		/**< Position DOP [0.01] */
	uint8_t		reserved1;
	uint8_t		numSV;		/**< Number of SVs used in Nav Solution */
	uint32_t	reserved2;
} ubx_payload_rx_nav_sol_t;

/* Rx NAV-PVT (ubx8) */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	uint16_t	year; 		/**< Year (UTC)*/
	uint8_t		month; 		/**< Month, range 1..12 (UTC) */
	uint8_t		day; 		/**< Day of month, range 1..31 (UTC) */
	uint8_t		hour; 		/**< Hour of day, range 0..23 (UTC) */
	uint8_t		min; 		/**< Minute of hour, range 0..59 (UTC) */
	uint8_t		sec;		/**< Seconds of minute, range 0..60 (UTC) */
	uint8_t		valid; 		/**< Validity flags (see UBX_RX_NAV_PVT_VALID_...) */
	uint32_t	tAcc; 		/**< Time accuracy estimate (UTC) [ns] */
	int32_t		nano;		/**< Fraction of second (UTC) [-1e9...1e9 ns] */
	uint8_t		fixType;	/**< GNSSfix type: 0 = No fix, 1 = Dead Reckoning only, 2 = 2D fix, 3 = 3d-fix, 4 = GNSS + dead reckoning, 5 = time only fix */
	uint8_t		flags;		/**< Fix Status Flags (see UBX_RX_NAV_PVT_FLAGS_...) */
	uint8_t		reserved1;
	uint8_t		numSV;		/**< Number of SVs used in Nav Solution */
	int32_t		lon;		/**< Longitude [1e-7 deg] */
	int32_t		lat;		/**< Latitude [1e-7 deg] */
	int32_t		height;		/**< Height above ellipsoid [mm] */
	int32_t		hMSL;		/**< Height above mean sea level [mm] */
	uint32_t	hAcc;  		/**< Horizontal accuracy estimate [mm] */
	uint32_t	vAcc;  		/**< Vertical accuracy estimate [mm] */
	int32_t		velN;		/**< NED north velocity [mm/s]*/
	int32_t		velE;		/**< NED east velocity [mm/s]*/
	int32_t		velD;		/**< NED down velocity [mm/s]*/
	int32_t		gSpeed;		/**< Ground Speed (2-D) [mm/s] */
	int32_t		headMot;	/**< Heading of motion (2-D) [1e-5 deg] */
	uint32_t	sAcc;		/**< Speed accuracy estimate [mm/s] */
	uint32_t	headAcc;	/**< Heading accuracy estimate (motion and vehicle) [1e-5 deg] */
	uint16_t	pDOP;		/**< Position DOP [0.01] */
	uint16_t	reserved2;
	uint32_t	reserved3;
	int32_t		headVeh;	/**< (ubx8+ only) Heading of vehicle (2-D) [1e-5 deg] */
	uint32_t	reserved4;	/**< (ubx8+ only) */
} ubx_payload_rx_nav_pvt_t;
#define UBX_PAYLOAD_RX_NAV_PVT_SIZE_UBX7	(sizeof(ubx_payload_rx_nav_pvt_t) - 8)
#define UBX_PAYLOAD_RX_NAV_PVT_SIZE_UBX8	(sizeof(ubx_payload_rx_nav_pvt_t))

/* Rx NAV-TIMEUTC */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	uint32_t	tAcc; 		/**< Time accuracy estimate (UTC) [ns] */
	int32_t		nano;		/**< Fraction of second, range -1e9 .. 1e9 (UTC) [ns] */
	uint16_t	year; 		/**< Year, range 1999..2099 (UTC) */
	uint8_t		month; 		/**< Month, range 1..12 (UTC) */
	uint8_t		day; 		/**< Day of month, range 1..31 (UTC) */
	uint8_t		hour; 		/**< Hour of day, range 0..23 (UTC) */
	uint8_t		min; 		/**< Minute of hour, range 0..59 (UTC) */
	uint8_t		sec;		/**< Seconds of minute, range 0..60 (UTC) */
	uint8_t		valid; 		/**< Validity Flags (see UBX_RX_NAV_TIMEUTC_VALID_...) */
} ubx_payload_rx_nav_timeutc_t;

/* Rx NAV-SVINFO Part 1 */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	uint8_t		numCh; 		/**< Number of channels */
	uint8_t		globalFlags;
	uint16_t	reserved2;
} ubx_payload_rx_nav_svinfo_part1_t;

/* Rx NAV-SVINFO Part 2 (repeated) */
typedef struct {
	uint8_t		chn; 		/**< Channel number, 255 for SVs not assigned to a channel */
	uint8_t		svid; 		/**< Satellite ID */
	uint8_t		flags;
	uint8_t		quality;
	uint8_t		cno;		/**< Carrier to Noise Ratio (Signal Strength) [dbHz] */
	int8_t		elev; 		/**< Elevation [deg] */
	int16_t		azim; 		/**< Azimuth [deg] */
	int32_t		prRes; 		/**< Pseudo range residual [cm] */
} ubx_payload_rx_nav_svinfo_part2_t;

/* Rx NAV-SVIN (survey-in info) */
typedef struct {
	uint8_t     version;
	uint8_t     reserved1[3];
	uint32_t	iTOW;
	uint32_t    dur;
	int32_t     meanX;
	int32_t     meanY;
	int32_t     meanZ;
	int8_t      meanXHP;
	int8_t      meanYHP;
	int8_t      meanZHP;
	int8_t      reserved2;
	uint32_t    meanAcc;
	uint32_t    obs;
	uint8_t     valid;
	uint8_t     active;
	uint8_t     reserved3[2];
} ubx_payload_rx_nav_svin_t;

/* Rx NAV-VELNED */
typedef struct {
	uint32_t	iTOW;		/**< GPS Time of Week [ms] */
	int32_t		velN;		/**< North velocity component [cm/s]*/
	int32_t		velE;		/**< East velocity component [cm/s]*/
	int32_t		velD;		/**< Down velocity component [cm/s]*/
	uint32_t	speed;		/**< Speed (3-D) [cm/s] */
	uint32_t	gSpeed;		/**< Ground speed (2-D) [cm/s] */
	int32_t		heading;	/**< Heading of motion 2-D [1e-5 deg] */
	uint32_t	sAcc;		/**< Speed accuracy estimate [cm/s] */
	uint32_t	cAcc;		/**< Course / Heading accuracy estimate [1e-5 deg] */
} ubx_payload_rx_nav_velned_t;

/* Rx MON-HW (ubx6) */
typedef struct {
	uint32_t	pinSel;
	uint32_t	pinBank;
	uint32_t	pinDir;
	uint32_t	pinVal;
	uint16_t	noisePerMS;
	uint16_t	agcCnt;
	uint8_t		aStatus;
	uint8_t		aPower;
	uint8_t		flags;
	uint8_t		reserved1;
	uint32_t	usedMask;
	uint8_t		VP[25];
	uint8_t		jamInd;
	uint16_t	reserved3;
	uint32_t	pinIrq;
	uint32_t	pullH;
	uint32_t	pullL;
} ubx_payload_rx_mon_hw_ubx6_t;

/* Rx MON-HW (ubx7+) */
typedef struct {
	uint32_t	pinSel;
	uint32_t	pinBank;
	uint32_t	pinDir;
	uint32_t	pinVal;
	uint16_t	noisePerMS;
	uint16_t	agcCnt;
	uint8_t		aStatus;
	uint8_t		aPower;
	uint8_t		flags;
	uint8_t		reserved1;
	uint32_t	usedMask;
	uint8_t		VP[17];
	uint8_t		jamInd;
	uint16_t	reserved3;
	uint32_t	pinIrq;
	uint32_t	pullH;
	uint32_t	pullL;
} ubx_payload_rx_mon_hw_ubx7_t;

/* Rx MON-VER Part 1 */
typedef struct {
	uint8_t		swVersion[30];
	uint8_t		hwVersion[10];
} ubx_payload_rx_mon_ver_part1_t;

/* Rx MON-VER Part 2 (repeated) */
typedef struct {
	uint8_t		extension[30];
} ubx_payload_rx_mon_ver_part2_t;

/* Rx ACK-ACK */
typedef	union {
	uint16_t	msg;
	struct {
		uint8_t	clsID;
		uint8_t	msgID;
	};
} ubx_payload_rx_ack_ack_t;

/* Rx ACK-NAK */
typedef	union {
	uint16_t	msg;
	struct {
		uint8_t	clsID;
		uint8_t	msgID;
	};
} ubx_payload_rx_ack_nak_t;

/* Tx CFG-PRT */
typedef struct {
	uint8_t		portID;
	uint8_t		reserved0;
	uint16_t	txReady;
	uint32_t	mode;
	uint32_t	baudRate;
	uint16_t	inProtoMask;
	uint16_t	outProtoMask;
	uint16_t	flags;
	uint16_t	reserved5;
} ubx_payload_tx_cfg_prt_t;

/* Tx CFG-RATE */
typedef struct {
	uint16_t	measRate;	/**< Measurement Rate, GPS measurements are taken every measRate milliseconds */
	uint16_t	navRate;	/**< Navigation Rate, in number of measurement cycles. This parameter cannot be changed, and must be set to 1 */
	uint16_t	timeRef;	/**< Alignment to reference time: 0 = UTC time, 1 = GPS time */
} ubx_payload_tx_cfg_rate_t;

/* Tx CFG-NAV5 */
typedef struct {
	uint16_t	mask;
	uint8_t		dynModel;	/**< Dynamic Platform model: 0 Portable, 2 Stationary, 3 Pedestrian, 4 Automotive, 5 Sea, 6 Airborne <1g, 7 Airborne <2g, 8 Airborne <4g */
	uint8_t		fixMode;	/**< Position Fixing Mode: 1 2D only, 2 3D only, 3 Auto 2D/3D */
	int32_t		fixedAlt;
	uint32_t	fixedAltVar;
	int8_t		minElev;
	uint8_t		drLimit;
	uint16_t	pDop;
	uint16_t	tDop;
	uint16_t	pAcc;
	uint16_t	tAcc;
	uint8_t		staticHoldThresh;
	uint8_t		dgpsTimeOut;
	uint8_t		cnoThreshNumSVs;	/**< (ubx7+ only, else 0) */
	uint8_t		cnoThresh;		/**< (ubx7+ only, else 0) */
	uint16_t	reserved;
	uint16_t	staticHoldMaxDist;	/**< (ubx8+ only, else 0) */
	uint8_t		utcStandard;		/**< (ubx8+ only, else 0) */
	uint8_t		reserved3;
	uint32_t	reserved4;
} ubx_payload_tx_cfg_nav5_t;

/* tx cfg-sbas */
typedef struct {
	uint8_t		mode;
	uint8_t		usage;
	uint8_t		maxSBAS;
	uint8_t		scanmode2;
	uint32_t	scanmode1;
} ubx_payload_tx_cfg_sbas_t;

/* Tx CFG-MSG */
typedef struct {
	union {
		uint16_t	msg;
		struct {
			uint8_t	msgClass;
			uint8_t	msgID;
		};
	};
	uint8_t rate;
} ubx_payload_tx_cfg_msg_t;

/* CFG-TMODE3 ublox 8 (protocol version >= 20) */
typedef struct {
	uint8_t     version;
	uint8_t     reserved1;
	uint16_t    flags;
	int32_t     ecefXOrLat;
	int32_t     ecefYOrLon;
	int32_t     ecefZOrAlt;
	int8_t      ecefXOrLatHP;
	int8_t      ecefYOrLonHP;
	int8_t      ecefZOrAltHP;
	uint8_t     reserved2;
	uint32_t    fixedPosAcc;
	uint32_t    svinMinDur;
	uint32_t    svinAccLimit;
	uint8_t     reserved3[8];
} ubx_payload_tx_cfg_tmode3_t;

/* General message and payload buffer union */
typedef union {
	ubx_payload_rx_nav_pvt_t		payload_rx_nav_pvt;
	ubx_payload_rx_nav_posllh_t		payload_rx_nav_posllh;
	ubx_payload_rx_nav_sol_t		payload_rx_nav_sol;
	ubx_payload_rx_nav_dop_t		payload_rx_nav_dop;
	ubx_payload_rx_nav_timeutc_t		payload_rx_nav_timeutc;
	ubx_payload_rx_nav_svinfo_part1_t	payload_rx_nav_svinfo_part1;
	ubx_payload_rx_nav_svinfo_part2_t	payload_rx_nav_svinfo_part2;
	ubx_payload_rx_nav_svin_t		payload_rx_nav_svin;
	ubx_payload_rx_nav_velned_t		payload_rx_nav_velned;
	ubx_payload_rx_mon_hw_ubx6_t		payload_rx_mon_hw_ubx6;
	ubx_payload_rx_mon_hw_ubx7_t		payload_rx_mon_hw_ubx7;
	ubx_payload_rx_mon_ver_part1_t		payload_rx_mon_ver_part1;
	ubx_payload_rx_mon_ver_part2_t		payload_rx_mon_ver_part2;
	ubx_payload_rx_ack_ack_t		payload_rx_ack_ack;
	ubx_payload_rx_ack_nak_t		payload_rx_ack_nak;
	ubx_payload_tx_cfg_prt_t		payload_tx_cfg_prt;
	ubx_payload_tx_cfg_rate_t		payload_tx_cfg_rate;
	ubx_payload_tx_cfg_nav5_t		payload_tx_cfg_nav5;
	ubx_payload_tx_cfg_sbas_t		payload_tx_cfg_sbas;
	ubx_payload_tx_cfg_msg_t		payload_tx_cfg_msg;
	ubx_payload_tx_cfg_tmode3_t		payload_tx_cfg_tmode3;
} ubx_buf_t;

#pragma pack(pop)
/*** END OF u-blox protocol binary message and payload definitions ***/

/* Decoder state */
typedef enum {
	UBX_DECODE_SYNC1 = 0,
	UBX_DECODE_SYNC2,
	UBX_DECODE_CLASS,
	UBX_DECODE_ID,
	UBX_DECODE_LENGTH1,
	UBX_DECODE_LENGTH2,
	UBX_DECODE_PAYLOAD,
	UBX_DECODE_CHKSUM1,
	UBX_DECODE_CHKSUM2,

	UBX_DECODE_RTCM3
} ubx_decode_state_t;

/* Rx message state */
typedef enum {
	UBX_RXMSG_IGNORE = 0,
	UBX_RXMSG_HANDLE,
	UBX_RXMSG_DISABLE,
	UBX_RXMSG_ERROR_LENGTH
} ubx_rxmsg_state_t;

/* ACK state */
typedef enum {
	UBX_ACK_IDLE = 0,
	UBX_ACK_WAITING,
	UBX_ACK_GOT_ACK,
	UBX_ACK_GOT_NAK
} ubx_ack_state_t;



	void sbp_setup(void);
	void sbp_pos_llh_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
	void sbp_baseline_ned_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
	void sbp_vel_ned_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
	void sbp_dops_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
	void sbp_gps_time_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context);
	int initialise_uart(void);
	int enable_flow_control(bool enabled);
	int deinitialize_uart(void);
	void runpiksi(void);



int initialise_uart(void);
int enable_flow_control(bool enabled);
int deinitialize_uart(void);
void runpiksi(void);


//static char _device[MAX_LEN_DEV_PATH];

//static int _uart_fd = -1;
//static bool _flow_control_enabled = false;


 //uint16_t crc16_ccitt(const u8 *buf, u32 len, u16 crc);






class GPSDriverUBX : public GPSHelper
{
public:
	GPSDriverUBX(Interface gpsInterface, GPSCallbackPtr callback, void *callback_user,
		     struct vehicle_gps_position_s *gps_position,
		     struct satellite_info_s *satellite_info,
		     uint8_t dynamic_model = 7);

	virtual ~GPSDriverUBX();

	int receive(unsigned timeout) override;
	int configure(unsigned &baudrate, OutputMode output_mode) override;

	int restartSurveyIn() override;

	void setSurveyInSpecs(uint32_t survey_in_acc_limit, uint32_t survey_in_min_dur);




private:

	/**
	 * Parse the binary UBX packet
	 */
	int parseChar(const uint8_t b);

	/**
	 * Start payload rx
	 */
	int payloadRxInit(void);

	/**
	 * Add payload rx byte
	 */
	int payloadRxAdd(const uint8_t b);
	int payloadRxAddNavSvinfo(const uint8_t b);
	int payloadRxAddMonVer(const uint8_t b);

	/**
	 * Finish payload rx
	 */
	int payloadRxDone(void);

	/**
	 * Reset the parse state machine for a fresh start
	 */
	void decodeInit(void);

	/**
	 * While parsing add every byte (except the sync bytes) to the checksum
	 */
	void addByteToChecksum(const uint8_t);

	/**
	 * Send a message
	 * @return true on success, false on write error (errno set)
	 */
	bool sendMessage(const uint16_t msg, const uint8_t *payload, const uint16_t length);

	/**
	 * Configure message rate
	 * @return true on success, false on write error
	 */
	bool configureMessageRate(const uint16_t msg, const uint8_t rate);

	/**
	 * Calculate & add checksum for given buffer
	 */
	void calcChecksum(const uint8_t *buffer, const uint16_t length, ubx_checksum_t *checksum);

	/**
	 * Wait for message acknowledge
	 */
	int waitForAck(const uint16_t msg, const unsigned timeout, const bool report);

	/**
	 * combines the configure_message_rate & wait_for_ack calls
	 * @return true on success
	 */
	inline bool configureMessageRateAndAck(uint16_t msg, uint8_t rate, bool report_ack_error = false);

	/**
	 * Calculate FNV1 hash
	 */
	uint32_t fnv1_32_str(uint8_t *str, uint32_t hval);

	struct vehicle_gps_position_s *_gps_position {nullptr};
	struct satellite_info_s *_satellite_info {nullptr};
	uint64_t		_last_timestamp_time{0};
	bool			_configured{false};
	ubx_ack_state_t		_ack_state{UBX_ACK_IDLE};
	bool			_got_posllh{false};
	bool			_got_velned{false};
	ubx_decode_state_t	_decode_state{};
	uint16_t		_rx_msg{};
	ubx_rxmsg_state_t	_rx_state{UBX_RXMSG_IGNORE};
	uint16_t		_rx_payload_length{};
	uint16_t		_rx_payload_index{};
	uint8_t			_rx_ck_a{};
	uint8_t			_rx_ck_b{};
	gps_abstime		_disable_cmd_last{0};
	uint16_t		_ack_waiting_msg{0};
	ubx_buf_t		_buf{};
	uint32_t		_ubx_version{0};
	bool			_use_nav_pvt{false};
	OutputMode		_output_mode{OutputMode::GPS};

	rtcm_message_t	*_rtcm_message{nullptr};

	const Interface		_interface;
	uint32_t _survey_in_acc_limit;
	uint32_t _survey_in_min_dur;

	// ublox Dynamic platform model default 7: airborne with <2g acceleration
	uint8_t _dyn_model{7};
};

#endif /* UBX_H_ */
