/****************************************************************************
 *
 *   Copyright (c) 2012-2015 PX4 Development Team. All rights reserved.
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
 * @file ubx.cpp
 *
 * U-Blox protocol implementation. Following u-blox 6/7/8 Receiver Description
 * including Prototol Specification.
 *
 * @author Thomas Gubler <thomasgubler@student.ethz.ch>
 * @author Julian Oes <joes@student.ethz.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 * @author Beat Kueng <beat-kueng@gmx.net>
 *
 * @author Hannes Delago
 *   (rework, add ubx7+ compatibility)
 *
 * @see https://www2.u-blox.com/images/downloads/Product_Docs/u-blox6-GPS-GLONASS-QZSS-V14_ReceiverDescriptionProtocolSpec_Public_(GPS.G6-SW-12013).pdf
 * @see https://www.u-blox.com/sites/default/files/products/documents/u-bloxM8_ReceiverDescrProtSpec_%28UBX-13003221%29_Public.pdf
 */
//#define DEFAULT_UART "/dev/ttyS3"
//#define MAX_MESSAGE_BUFFER_SIZE 45

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctime>
 //#include "edc.h"

//#include <stdint.h>
#include "ubx.h"
//#include "px4_simple_app.h"
//#include "px4_simple_app.c"
// #define MAVLINK_MESSAGE_LENGTHS {9, 31, 12, 0, 14, 28, 3, 32, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 20, 2, 25, 23, 30, 101, 22, 26, 16, 14, 28, 32, 28, 28, 22, 22, 21, 6, 6, 37, 4, 4, 2, 2, 4, 2, 2, 3, 13, 12, 37, 4, 0, 0, 27, 25, 0, 0, 0, 0, 0, 72, 26, 181, 225, 42, 6, 4, 0, 11, 18, 0, 0, 37, 20, 35, 33, 3, 0, 0, 0, 22, 39, 37, 53, 51, 53, 51, 0, 28, 56, 42, 33, 81, 0, 0, 0, 0, 0, 0, 26, 32, 32, 20, 32, 62, 44, 64, 84, 9, 254, 16, 12, 36, 44, 64, 22, 6, 14, 12, 97, 2, 2, 113, 35, 6, 79, 35, 35, 22, 13, 255, 14, 18, 43, 8, 22, 14, 36, 43, 41, 32, 243, 14, 93, 0, 100, 36, 60, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 40, 63, 182, 40, 0, 0, 0, 0, 0, 0, 32, 52, 53, 6, 2, 38, 19, 254, 36, 30, 18, 18, 51, 9, 0}
// #define MAVLINK_MESSAGE_CRCS {50, 124, 137, 0, 237, 217, 104, 119, 0, 0, 0, 89, 0, 0, 0, 0, 0, 0, 0, 0, 214, 159, 220, 168, 24, 23, 170, 144, 67, 115, 39, 246, 185, 104, 237, 244, 222, 212, 9, 254, 230, 28, 28, 132, 221, 232, 11, 153, 41, 39, 78, 196, 0, 0, 15, 3, 0, 0, 0, 0, 0, 167, 183, 119, 191, 118, 148, 21, 0, 243, 124, 0, 0, 38, 20, 158, 152, 143, 0, 0, 0, 106, 49, 22, 143, 140, 5, 150, 0, 231, 183, 63, 54, 47, 0, 0, 0, 0, 0, 0, 175, 102, 158, 208, 56, 93, 138, 108, 32, 185, 84, 34, 174, 124, 237, 4, 76, 128, 56, 116, 134, 237, 203, 250, 87, 203, 220, 25, 226, 46, 29, 223, 85, 6, 229, 203, 1, 195, 109, 168, 181, 47, 72, 131, 127, 0, 103, 154, 178, 200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 163, 105, 151, 35, 150, 0, 0, 0, 0, 0, 0, 90, 104, 85, 95, 130, 184, 81, 8, 204, 49, 170, 44, 83, 46, 0}


 //#define MAX_LEN_DEV_PATH 32



// static const uint8_t mavlink_message_lengths[256] = MAVLINK_MESSAGE_LENGTHS;
// static const uint8_t mavlink_message_crcs[256] = MAVLINK_MESSAGE_CRCS;

 // static char _device[MAX_LEN_DEV_PATH];

 // static int _uart_fd = -1;
 // static bool _flow_control_enabled = false;





// /*
//  * State of the SBP message parser.
//  * Must be statically allocated.
//  */
// sbp_state_t sbp_state;

// /* SBP structs that messages from Piksi will feed. */
// msg_pos_llh_t      pos_llh;
// msg_baseline_ned_t baseline_ned;
// msg_vel_ned_t      vel_ned;
// msg_dops_t         dops;
// msg_gps_time_t     gps_time;

// /*
//  * SBP callback nodes must be statically allocated. Each message ID / callback
//  * pair must have a unique sbp_msg_callbacks_node_t associated with it.
//  */
// sbp_msg_callbacks_node_t pos_llh_node;
// sbp_msg_callbacks_node_t baseline_ned_node;
// sbp_msg_callbacks_node_t vel_ned_node;
// sbp_msg_callbacks_node_t dops_node;
// sbp_msg_callbacks_node_t gps_time_node;

// void sbp_pos_llh_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context)
// {
//   pos_llh = *(msg_pos_llh_t *)msg;
// }
// void sbp_baseline_ned_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context)
// {
//   baseline_ned = *(msg_baseline_ned_t *)msg;
// }
// void sbp_vel_ned_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context)
// {
//   vel_ned = *(msg_vel_ned_t *)msg;
// }
// void sbp_dops_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context)
// {
//   dops = *(msg_dops_t *)msg;
// }
// void sbp_gps_time_callback(uint16_t sender_id, uint8_t len, uint8_t msg[], void *context)
// {
//   gps_time = *(msg_gps_time_t *)msg;
// }
/*
 * Set up SwiftNav Binary Protocol (SBP) nodes; the sbp_process function will
 * search through these to find the callback for a particular message ID.
 *
 * Example: sbp_pos_llh_callback is registered with sbp_state, and is associated
 * with both a unique sbp_msg_callbacks_node_t and the message ID SBP_POS_LLH.
 * When a valid SBP message with the ID SBP_POS_LLH comes through the UART, written
 * to the FIFO, and then parsed by sbp_process, sbp_pos_llh_callback is called
 * with the data carried by that message.
 */



// void sbp_setup(void)
// {
//   /* SBP parser state must be initialized before sbp_process is called. */
//   sbp_state_init(&sbp_state);

//   /* Register a node and callback, and associate them with a specific message ID. */
//   sbp_register_callback(&sbp_state, SBP_MSG_GPS_TIME, &sbp_gps_time_callback,
//                         NULL, &gps_time_node);
//   sbp_register_callback(&sbp_state, SBP_MSG_POS_LLH, &sbp_pos_llh_callback,
//                         NULL, &pos_llh_node);
//   sbp_register_callback(&sbp_state, SBP_MSG_BASELINE_NED, &sbp_baseline_ned_callback,
//                         NULL, &baseline_ned_node);
//   sbp_register_callback(&sbp_state, SBP_MSG_VEL_NED, &sbp_vel_ned_callback,
//                         NULL, &vel_ned_node);
//   sbp_register_callback(&sbp_state, SBP_MSG_DOPS, &sbp_dops_callback,
//                         NULL, &dops_node);

// }
 //uint16_t crc16_ccitt(const u8 *buf, u32 len, u16 crc);


#define UBX_CONFIG_TIMEOUT	200		// ms, timeout for waiting ACK
#define UBX_PACKET_TIMEOUT	2		// ms, if now data during this delay assume that full update received
#define DISABLE_MSG_INTERVAL	1000000		// us, try to disable message with this interval

#define MIN(X,Y)	((X) < (Y) ? (X) : (Y))
#define SWAP16(X)	((((X) >>  8) & 0x00ff) | (((X) << 8) & 0xff00))

#define FNV1_32_INIT	((uint32_t)0x811c9dc5)	// init value for FNV1 hash algorithm
#define FNV1_32_PRIME	((uint32_t)0x01000193)	// magic prime for FNV1 hash algorithm


/**** Trace macros, disable for production builds */
#define UBX_TRACE_PARSER(...)	{/*GPS_INFO(__VA_ARGS__);*/}	/* decoding progress in parse_char() */
#define UBX_TRACE_RXMSG(...)		{/*GPS_INFO(__VA_ARGS__);*/}	/* Rx msgs in payload_rx_done() */
#define UBX_TRACE_SVINFO(...)	{/*GPS_INFO(__VA_ARGS__);*/}	/* NAV-SVINFO processing (debug use only, will cause rx buffer overflows) */

/**** Warning macros, disable to save memory */
#define UBX_WARN(...)		{GPS_WARN(__VA_ARGS__);}
#define UBX_DEBUG(...)		{/*GPS_WARN(__VA_ARGS__);*/}




GPSDriverUBX::GPSDriverUBX(Interface gpsInterface, GPSCallbackPtr callback, void *callback_user,
			   struct vehicle_gps_position_s *gps_position,
			   struct satellite_info_s *satellite_info,
			   uint8_t dynamic_model)
	: GPSHelper(callback, callback_user)
	, _gps_position(gps_position)
	, _satellite_info(satellite_info)
	//, _interface(gpsInterface)
	, _survey_in_acc_limit(UBX_TX_CFG_TMODE3_SVINACCLIMIT)
	, _survey_in_min_dur(UBX_TX_CFG_TMODE3_SVINMINDUR)
	//, _dyn_model(dynamic_model)
{
	decodeInit();
}

GPSDriverUBX::~GPSDriverUBX()
{
	if (_rtcm_message) {
		delete[](_rtcm_message->buffer);
		delete (_rtcm_message);
	}
}
// int initialise_uart()
// {
// 	// open uart
// 	_uart_fd = px4_open("/dev/ttyS3", O_RDWR | O_NOCTTY);
// 	int termios_state = -1;

// 	if (_uart_fd < 0) {
// 		PX4_ERR("failed to open uart device!");
// 		return -1;
// 	}
// 	PX4_INFO("here0");
// 	// set baud rate
// 	int speed = 115200;
// 	printf("%d\n", speed);
// 	struct termios uart_config;
// 	tcgetattr(_uart_fd, &uart_config);
// 	// clear ONLCR flag (which appends a CR for every LF)
// 	//uart_config.c_oflag &= ~ONLCR;

// 	/* Set baud rate */
// 	if (cfsetispeed(&uart_config, speed) < 0 || cfsetospeed(&uart_config, speed) < 0) {
// 		warnx("ERR SET BAUD %s: %d\n", _device, termios_state);
// 		//::close(_uart_fd);
// 		return -1;
// 	}

// 	if ((termios_state = tcsetattr(_uart_fd, TCSANOW, &uart_config)) < 0) {
// 		PX4_WARN("ERR SET CONF %s\n", _device);
// 		px4_close(_uart_fd);
// 		return -1;
// 	}

// 	/* setup output flow control */
// 	if (enable_flow_control(false)) {
// 		PX4_WARN("hardware flow disable failed");
// 	}

// 	return _uart_fd;
// }

// int enable_flow_control(bool enabled)
// {
// 	struct termios uart_config;

// 	int ret = tcgetattr(_uart_fd, &uart_config);

// 	if (enabled) {
// 		uart_config.c_cflag |= CRTSCTS;

// 	} else {
// 		uart_config.c_cflag &= ~CRTSCTS;
// 	}

// 	ret = tcsetattr(_uart_fd, TCSANOW, &uart_config);

// 	if (!ret) {
// 		_flow_control_enabled = enabled;
// 	}

// 	return ret;
// }

// int deinitialize_uart()
// {
// 	return close(_uart_fd);
// }

int GPSDriverUBX::initialise_uart()
{
	// open uart
	_uart_fd = open("/dev/ttyS3", O_RDWR | O_NOCTTY);
	int termios_state = -1;

	if (_uart_fd < 0) {
		PX4_ERR("failed to open uart device! error no: %d", _uart_fd);
		return -1;
	}
PX4_INFO("here0");
	// set baud rate
	int speed = 115200;
	printf("%d\n", speed);
	struct termios uart_config;
	tcgetattr(_uart_fd, &uart_config);
	// clear ONLCR flag (which appends a CR for every LF)
	//uart_config.c_oflag &= ~ONLCR;

	/* Set baud rate */
	if (cfsetispeed(&uart_config, speed) < 0 || cfsetospeed(&uart_config, speed) < 0) {
		warnx("ERR SET BAUD %s: %d\n", _device2, termios_state);
		//::close(_uart_fd);
		return -1;
	}

	if ((termios_state = tcsetattr(_uart_fd, TCSANOW, &uart_config)) < 0) {
		PX4_WARN("ERR SET CONF %s\n", _device2);
		px4_close(_uart_fd);
		return -1;
	}

	/* setup output flow control */
	if (enable_flow_control(false)) {
		PX4_WARN("hardware flow disable failed");
	}

	return _uart_fd;
}

int GPSDriverUBX::enable_flow_control(bool enabled)
{
	struct termios uart_config;

	int ret = tcgetattr(_uart_fd, &uart_config);

	if (enabled) {
		uart_config.c_cflag |= CRTSCTS;

	} else {
		uart_config.c_cflag &= ~CRTSCTS;
	}

	ret = tcsetattr(_uart_fd, TCSANOW, &uart_config);

	if (!ret) {
		_flow_control_enabled = enabled;
	}

	return ret;
}

int GPSDriverUBX::deinitialize_uart()
{
	return close(_uart_fd);
}

void runpiksi()
{
		// struct vehicle_gps_position_s pos;
		// memset(&pos, 0, sizeof(pos));
		// orb_advert_t _gps_pub = orb_advertise(ORB_ID(vehicle_gps_position), &pos);

		// initialise_uart();
		// px4_pollfd_struct_t fds[1];
		// fds[0].fd = _uart_fd;
		// fds[0].events = POLLIN;
							// sbp_setup();

						 //  	u16 crc;
						 //  	const u8 *test_data = (uint8_t*)"123456789"; //this is just checking the crc
						 //  	crc = crc16_ccitt(test_data, 0, 22);
						 //  	PX4_INFO("crc %d",crc);
						 //  	PX4_INFO("herecrcrc");
	//piksi_uart(0,nullptr);
}

int
GPSDriverUBX::configure(unsigned &baudrate, OutputMode output_mode)
{
	_configured = false;
	_output_mode = output_mode;
	/* try different baudrates */
	//const unsigned baudrates[] = {9600, 38400, 19200, 57600, 115200, 230400};

	// unsigned baud_i;
	// ubx_payload_tx_cfg_prt_t cfg_prt[2];
	// uint16_t out_proto_mask = output_mode == OutputMode::GPS ?
	// 			  UBX_TX_CFG_PRT_OUTPROTOMASK_GPS :
	// 			  UBX_TX_CFG_PRT_OUTPROTOMASK_RTCM;
	// uint16_t in_proto_mask = output_mode == OutputMode::GPS ?
	// 			 UBX_TX_CFG_PRT_INPROTOMASK_GPS :
	// 			 UBX_TX_CFG_PRT_INPROTOMASK_RTCM;
	//FIXME: RTCM3 output needs at least protocol version 20. The protocol version can be checked via the version
	//output:
	//WARN  VER ext "                  PROTVER=20.00"
	//However this is a string and it is not well documented, that PROTVER is always contained. Maybe there is a
	//better way to check the protocol version?
		//decodeInit();
		//char serial_buf[1];
		//mavlink_status_t serial_status = {};

		runpiksi();
		receive(0);
		//decodeInit();


	// if (_interface == Interface::UART) {
	// 	for (baud_i = 0; baud_i < sizeof(baudrates) / sizeof(baudrates[0]); baud_i++) {
	// 		baudrate = baudrates[baud_i];
	// 		setBaudrate(baudrate);

	// 		/* flush input and wait for at least 20 ms silence */
	// 		decodeInit();
	// 		receive(20);
	// 		decodeInit();

	// 		/* Send a CFG-PRT message to set the UBX protocol for in and out
	// 		 * and leave the baudrate as it is, we just want an ACK-ACK for this */
	// 		memset(cfg_prt, 0, 2 * sizeof(ubx_payload_tx_cfg_prt_t));
	// 		cfg_prt[0].portID		= UBX_TX_CFG_PRT_PORTID;
	// 		cfg_prt[0].mode		= UBX_TX_CFG_PRT_MODE;
	// 		cfg_prt[0].baudRate	= baudrate;
	// 		cfg_prt[0].inProtoMask	= in_proto_mask;
	// 		cfg_prt[0].outProtoMask	= out_proto_mask;
	// 		cfg_prt[1].portID		= UBX_TX_CFG_PRT_PORTID_USB;
	// 		cfg_prt[1].mode		= UBX_TX_CFG_PRT_MODE;
	// 		cfg_prt[1].baudRate	= baudrate;
	// 		cfg_prt[1].inProtoMask	= in_proto_mask;
	// 		cfg_prt[1].outProtoMask	= out_proto_mask;

	// 		if (!sendMessage(UBX_MSG_CFG_PRT, (uint8_t *)cfg_prt, 2 * sizeof(ubx_payload_tx_cfg_prt_t))) {
	// 			continue;
	// 		}

	// 		if (waitForAck(UBX_MSG_CFG_PRT, UBX_CONFIG_TIMEOUT, false) < 0) {
	// 			/* try next baudrate */
	// 			continue;
	// 		}

	// 		 //Send a CFG-PRT message again, this time change the baudrate 
	// 		memset(cfg_prt, 0, 2 * sizeof(ubx_payload_tx_cfg_prt_t));
	// 		cfg_prt[0].portID		= UBX_TX_CFG_PRT_PORTID;
	// 		cfg_prt[0].mode		= UBX_TX_CFG_PRT_MODE;
	// 		cfg_prt[0].baudRate	= UBX_TX_CFG_PRT_BAUDRATE;
	// 		cfg_prt[0].inProtoMask	= in_proto_mask;
	// 		cfg_prt[0].outProtoMask	= out_proto_mask;
	// 		cfg_prt[1].portID		= UBX_TX_CFG_PRT_PORTID_USB;
	// 		cfg_prt[1].mode		= UBX_TX_CFG_PRT_MODE;
	// 		cfg_prt[1].baudRate	= UBX_TX_CFG_PRT_BAUDRATE;
	// 		cfg_prt[1].inProtoMask	= in_proto_mask;
	// 		cfg_prt[1].outProtoMask	= out_proto_mask;

	// 		if (!sendMessage(UBX_MSG_CFG_PRT, (uint8_t *)cfg_prt, 2 * sizeof(ubx_payload_tx_cfg_prt_t))) {
	// 			continue;
	// 		}

	// 		/* no ACK is expected here, but read the buffer anyway in case we actually get an ACK */
	// 		waitForAck(UBX_MSG_CFG_PRT, UBX_CONFIG_TIMEOUT, false);

	// 		if (UBX_TX_CFG_PRT_BAUDRATE != baudrate) {
	// 			setBaudrate(UBX_TX_CFG_PRT_BAUDRATE);
	// 			baudrate = UBX_TX_CFG_PRT_BAUDRATE;
	// 		}

	// 		/* at this point we have correct baudrate on both ends */
	// 		break;
	// 	}

	// 	if (baud_i >= sizeof(baudrates) / sizeof(baudrates[0])) {
	// 		return -1;	// connection and/or baudrate detection failed
	// 	}

	// } 
	// else if (_interface == Interface::SPI) {
	// 	memset(cfg_prt, 0, 2 * sizeof(ubx_payload_tx_cfg_prt_t));
	// 	cfg_prt[0].portID		= UBX_TX_CFG_PRT_PORTID_SPI;
	// 	cfg_prt[0].mode			= UBX_TX_CFG_PRT_MODE_SPI;
	// 	cfg_prt[0].inProtoMask	= in_proto_mask;
	// 	cfg_prt[0].outProtoMask	= out_proto_mask;

	// 	if (!sendMessage(UBX_MSG_CFG_PRT, (uint8_t *)cfg_prt, sizeof(ubx_payload_tx_cfg_prt_t))) {
	// 		return -1;
	// 	}

	// 	/* no ACK is expected here, but read the buffer anyway in case we actually get an ACK */
	// 	waitForAck(UBX_MSG_CFG_PRT, UBX_CONFIG_TIMEOUT, false);

	// } else {
	// 	return -1;
	// }

// 	/* Send a CFG-RATE message to define update rate */
// 	memset(&_buf.payload_tx_cfg_rate, 0, sizeof(_buf.payload_tx_cfg_rate));
// 	_buf.payload_tx_cfg_rate.measRate	= UBX_TX_CFG_RATE_MEASINTERVAL;
// 	_buf.payload_tx_cfg_rate.navRate	= UBX_TX_CFG_RATE_NAVRATE;
// 	_buf.payload_tx_cfg_rate.timeRef	= UBX_TX_CFG_RATE_TIMEREF;

// 	if (!sendMessage(UBX_MSG_CFG_RATE, (uint8_t *)&_buf, sizeof(_buf.payload_tx_cfg_rate))) {
// 		return -1;
// 	}

// 	if (waitForAck(UBX_MSG_CFG_RATE, UBX_CONFIG_TIMEOUT, true) < 0) {
// 		return -1;
// 	}

// 	if (output_mode != OutputMode::GPS) {
// 		// RTCM mode force stationary dynamic model
// 		_dyn_model = 2;
// 	}

// 	/* send a NAV5 message to set the options for the internal filter */
// 	memset(&_buf.payload_tx_cfg_nav5, 0, sizeof(_buf.payload_tx_cfg_nav5));
// 	_buf.payload_tx_cfg_nav5.mask		= UBX_TX_CFG_NAV5_MASK;
// 	_buf.payload_tx_cfg_nav5.dynModel	= _dyn_model;
// 	_buf.payload_tx_cfg_nav5.fixMode	= UBX_TX_CFG_NAV5_FIXMODE;

// 	if (!sendMessage(UBX_MSG_CFG_NAV5, (uint8_t *)&_buf, sizeof(_buf.payload_tx_cfg_nav5))) {
// 		return -1;
// 	}

// 	if (waitForAck(UBX_MSG_CFG_NAV5, UBX_CONFIG_TIMEOUT, true) < 0) {
// 		return -1;
// 	}

// #ifdef UBX_CONFIGURE_SBAS
// 	/* send a SBAS message to set the SBAS options */
// 	memset(&_buf.payload_tx_cfg_sbas, 0, sizeof(_buf.payload_tx_cfg_sbas));
// 	_buf.payload_tx_cfg_sbas.mode		= UBX_TX_CFG_SBAS_MODE;

// 	if (!sendMessage(UBX_MSG_CFG_SBAS, (uint8_t *)&_buf, sizeof(_buf.payload_tx_cfg_sbas))) {
// 		return -1;
// 	}

// 	if (waitForAck(UBX_MSG_CFG_SBAS, UBX_CONFIG_TIMEOUT, true) < 0) {
// 		return -1;
// 	}

// #endif

// 	/* configure message rates */
// 	/* the last argument is divisor for measurement rate (set by CFG RATE), i.e. 1 means 5Hz */

// 	/* try to set rate for NAV-PVT */
// 	/* (implemented for ubx7+ modules only, use NAV-SOL, NAV-POSLLH, NAV-VELNED and NAV-TIMEUTC for ubx6) */
// 	if (!configureMessageRate(UBX_MSG_NAV_PVT, 1)) {
// 		return -1;
// 	}

// 	if (waitForAck(UBX_MSG_CFG_MSG, UBX_CONFIG_TIMEOUT, true) < 0) {
// 		_use_nav_pvt = false;

// 	} else {
// 		_use_nav_pvt = true;
// 	}

// 	UBX_DEBUG("%susing NAV-PVT", _use_nav_pvt ? "" : "not ");

// 	if (!_use_nav_pvt) {
// 		if (!configureMessageRateAndAck(UBX_MSG_NAV_TIMEUTC, 5, true)) {
// 			return -1;
// 		}

// 		if (!configureMessageRateAndAck(UBX_MSG_NAV_POSLLH, 1, true)) {
// 			return -1;
// 		}

// 		if (!configureMessageRateAndAck(UBX_MSG_NAV_SOL, 1, true)) {
// 			return -1;
// 		}

// 		if (!configureMessageRateAndAck(UBX_MSG_NAV_VELNED, 1, true)) {
// 			return -1;
// 		}
// 	}

// 	if (!configureMessageRateAndAck(UBX_MSG_NAV_DOP, 1, true)) {
// 		return -1;
// 	}

// 	if (!configureMessageRateAndAck(UBX_MSG_NAV_SVINFO, (_satellite_info != nullptr) ? 5 : 0, true)) {
// 		return -1;
// 	}

// 	if (!configureMessageRateAndAck(UBX_MSG_MON_HW, 1, true)) {
// 		return -1;
// 	}

// 	/* request module version information by sending an empty MON-VER message */
// 	if (!sendMessage(UBX_MSG_MON_VER, nullptr, 0)) {
// 		return -1;
// 	}

// 	if (output_mode == OutputMode::RTCM) {
// 		if (restartSurveyIn() < 0) {
// 			return -1;
// 		}
// 	}

	_configured = true;
	return 0;
}

int GPSDriverUBX::restartSurveyIn()
{


	return 0;
}

int	// -1 = NAK, error or timeout, 0 = ACK
GPSDriverUBX::waitForAck(const uint16_t msg, const unsigned timeout, const bool report)
{
	return 0;
}

int	// -1 = error, 0 = no message handled, 1 = message handled, 2 = sat info message handled
GPSDriverUBX::receive(unsigned timeout)
{
	//uint8_t buf[GPS_READ_BUFFER_SIZE];

	/* timeout additional to poll */
	//gps_abstime time_started = gps_absolute_time();

	//int handled = 0;

	while (true) {
		int ret =payloadRxDone();
		return ret;
		// bool ready_to_return = _configured ? (_got_posllh && _got_velned) : handled;

		// /* Wait for only UBX_PACKET_TIMEOUT if something already received. */
		// int ret = read(buf, sizeof(buf), ready_to_return ? UBX_PACKET_TIMEOUT : timeout);

		// if (ret < 0) {
		// 	/* something went wrong when polling or reading */
		// 	UBX_WARN("ubx poll_or_read err");
		// 	PX4_INFO("ubx poll or read err");
		// 	return -1;

		// } else if (ret == 0) {
		// 	/* return success if ready */
		// 	if (ready_to_return) {
		// 		_got_posllh = false;
		// 		_got_velned = false;
		// 		return handled;
		// 	}

		// } else {
		// 	//UBX_DEBUG("read %d bytes", ret);

		// 	/* pass received bytes to the packet decoder */
		// 	for (int i = 0; i < ret; i++) {
		// 		handled |= parseChar(buf[i]);
		// 		//UBX_DEBUG("parsed %d: 0x%x", i, buf[i]);
		// 	}

		// 	if (_interface == Interface::SPI) {
		// 		if (buf[ret - 1] == 0xff) {
		// 			if (ready_to_return) {
		// 				_got_posllh = false;
		// 				_got_velned = false;
		// 				return handled;
		// 			}
		// 		}
		// 	}
		// }

		// /* abort after timeout if no useful packets received */
		// if (time_started + timeout * 1000 < gps_absolute_time()) {
		// 	UBX_DEBUG("timed out, returning");
		// 	return -1;
		// }
	}
}

int	// 0 = decoding, 1 = message handled, 2 = sat info message handled
GPSDriverUBX::parseChar(const uint8_t b)
{
	int ret = 0;
	//PX4_INFO("%d",_decode_state);
	switch (_decode_state) {

	/* Expecting Sync1 */
	case UBX_DECODE_SYNC1:
		if (b == UBX_SYNC1) {	// Sync1 found --> expecting Sync2
			UBX_TRACE_PARSER("A");
			_decode_state = UBX_DECODE_SYNC2;

		} else if (b == RTCM3_PREAMBLE && _rtcm_message) {
			UBX_TRACE_PARSER("RTCM");
			_decode_state = UBX_DECODE_RTCM3;
			_rtcm_message->buffer[_rtcm_message->pos++] = b;
		}

		break;

	/* Expecting Sync2 */
	case UBX_DECODE_SYNC2:
		if (b == UBX_SYNC2) {	// Sync2 found --> expecting Class
			UBX_TRACE_PARSER("B");
			_decode_state = UBX_DECODE_CLASS;

		} else {		// Sync1 not followed by Sync2: reset parser
			decodeInit();
		}

		break;

	/* Expecting Class */
	case UBX_DECODE_CLASS:
		UBX_TRACE_PARSER("C");
		addByteToChecksum(b);  // checksum is calculated for everything except Sync and Checksum bytes
		_rx_msg = b;
		_decode_state = UBX_DECODE_ID;
		break;

	/* Expecting ID */
	case UBX_DECODE_ID:
		UBX_TRACE_PARSER("D");
		addByteToChecksum(b);
		_rx_msg |= b << 8;
		_decode_state = UBX_DECODE_LENGTH1;
		break;

	/* Expecting first length byte */
	case UBX_DECODE_LENGTH1:
		UBX_TRACE_PARSER("E");
		addByteToChecksum(b);
		_rx_payload_length = b;
		_decode_state = UBX_DECODE_LENGTH2;
		break;

	/* Expecting second length byte */
	case UBX_DECODE_LENGTH2:{
		UBX_TRACE_PARSER("F");
		addByteToChecksum(b);
		_rx_payload_length |= b << 8;	// calculate payload size

		if (payloadRxInit() != 0) {	// start payload reception
			// payload will not be handled, discard message
			decodeInit();

		} else {
			_decode_state = (_rx_payload_length > 0) ? UBX_DECODE_PAYLOAD : UBX_DECODE_CHKSUM1;
		}
		}
		break;

	/* Expecting payload */
	case UBX_DECODE_PAYLOAD:
		UBX_TRACE_PARSER(".");
		addByteToChecksum(b);

		switch (_rx_msg) {
		case UBX_MSG_NAV_SVINFO:
			ret = payloadRxAddNavSvinfo(b);	// add a NAV-SVINFO payload byte
			break;

		case UBX_MSG_MON_VER:
			ret = payloadRxAddMonVer(b);	// add a MON-VER payload byte
			break;

		default:
			ret = payloadRxAdd(b);		// add a payload byte
			break;
		}

		if (ret < 0) {
			// payload not handled, discard message
			decodeInit();

		} else if (ret > 0) {
			// payload complete, expecting checksum
			_decode_state = UBX_DECODE_CHKSUM1;

		} else {
			// expecting more payload, stay in state UBX_DECODE_PAYLOAD
		}

		ret = 0;
		break;

	/* Expecting first checksum byte */
	case UBX_DECODE_CHKSUM1:
		if (_rx_ck_a != b) {
			UBX_WARN("ubx checksum err");
			decodeInit();

		} else {
			_decode_state = UBX_DECODE_CHKSUM2;
		}

		break;

	/* Expecting second checksum byte */
	case UBX_DECODE_CHKSUM2:
		if (_rx_ck_b != b) {
			UBX_WARN("ubx checksum err");

		} else {
			ret = payloadRxDone();	// finish payload processing
		}

		decodeInit();
		break;

	case UBX_DECODE_RTCM3:
		_rtcm_message->buffer[_rtcm_message->pos++] = b;

		if (_rtcm_message->pos == 3) {
			_rtcm_message->message_length = (((uint16_t)_rtcm_message->buffer[1] & 3) << 8) | (_rtcm_message->buffer[2]);
			UBX_DEBUG("got RTCM message with length %i", (int)_rtcm_message->message_length);

			if (_rtcm_message->message_length + 6 > _rtcm_message->buffer_len) {
				uint16_t new_buffer_len = _rtcm_message->message_length + 6;
				uint8_t *new_buffer = new uint8_t[new_buffer_len];
				memcpy(new_buffer, _rtcm_message->buffer, 3);
				delete[](_rtcm_message->buffer);
				_rtcm_message->buffer = new_buffer;
				_rtcm_message->buffer_len = new_buffer_len;
			}
		}

		if (_rtcm_message->message_length + 6 == _rtcm_message->pos) {

			gotRTCMMessage(_rtcm_message->buffer, _rtcm_message->pos);
			decodeInit();
		}

		break;

	default:
		break;
	}
	//PX4_INFO("ret:=====%d",ret);
	return ret;
}

/**
 * Start payload rx
 */
int	// -1 = abort, 0 = continue
GPSDriverUBX::payloadRxInit()
{
	int ret = 0;

	_rx_state = UBX_RXMSG_HANDLE;	// handle by default

	switch (_rx_msg) {
	case UBX_MSG_NAV_PVT:
		if ((_rx_payload_length != UBX_PAYLOAD_RX_NAV_PVT_SIZE_UBX7)		/* u-blox 7 msg format */
		    && (_rx_payload_length != UBX_PAYLOAD_RX_NAV_PVT_SIZE_UBX8)) {	/* u-blox 8+ msg format */
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (!_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if not _configured

		} else if (!_use_nav_pvt) {
			_rx_state = UBX_RXMSG_DISABLE;        // disable if not using NAV-PVT
		}

		break;

	case UBX_MSG_INF_DEBUG:
	case UBX_MSG_INF_ERROR:
	case UBX_MSG_INF_NOTICE:
	case UBX_MSG_INF_WARNING:
		if (_rx_payload_length >= sizeof(ubx_buf_t)) {
			_rx_payload_length = sizeof(ubx_buf_t) - 1; //avoid buffer overflow
		}

		break;

	case UBX_MSG_NAV_POSLLH:
		if (_rx_payload_length != sizeof(ubx_payload_rx_nav_posllh_t)) {
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (!_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if not _configured

		} else if (_use_nav_pvt) {
			_rx_state = UBX_RXMSG_DISABLE;        // disable if using NAV-PVT instead
		}

		break;

	case UBX_MSG_NAV_SOL:
		if (_rx_payload_length != sizeof(ubx_payload_rx_nav_sol_t)) {
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (!_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if not _configured

		} else if (_use_nav_pvt) {
			_rx_state = UBX_RXMSG_DISABLE;        // disable if using NAV-PVT instead
		}

		break;

	case UBX_MSG_NAV_DOP:
		if (_rx_payload_length != sizeof(ubx_payload_rx_nav_dop_t)) {
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (!_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if not _configured

		}

		break;

	case UBX_MSG_NAV_TIMEUTC:
		if (_rx_payload_length != sizeof(ubx_payload_rx_nav_timeutc_t)) {
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (!_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if not _configured

		} else if (_use_nav_pvt) {
			_rx_state = UBX_RXMSG_DISABLE;        // disable if using NAV-PVT instead
		}

		break;

	case UBX_MSG_NAV_SVINFO:
		if (_satellite_info == nullptr) {
			_rx_state = UBX_RXMSG_DISABLE;        // disable if sat info not requested

		} else if (!_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if not _configured

		} else {
			memset(_satellite_info, 0, sizeof(*_satellite_info));        // initialize sat info
		}

		break;

	case UBX_MSG_NAV_SVIN:
		if (_rx_payload_length != sizeof(ubx_payload_rx_nav_svin_t)) {
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (!_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if not _configured

		}

		break;

	case UBX_MSG_NAV_VELNED:
		if (_rx_payload_length != sizeof(ubx_payload_rx_nav_velned_t)) {
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (!_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if not _configured

		} else if (_use_nav_pvt) {
			_rx_state = UBX_RXMSG_DISABLE;        // disable if using NAV-PVT instead
		}

		break;

	case UBX_MSG_MON_VER:
		break;		// unconditionally handle this message

	case UBX_MSG_MON_HW:
		if ((_rx_payload_length != sizeof(ubx_payload_rx_mon_hw_ubx6_t))	/* u-blox 6 msg format */
		    && (_rx_payload_length != sizeof(ubx_payload_rx_mon_hw_ubx7_t))) {	/* u-blox 7+ msg format */
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (!_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if not _configured
		}

		break;

	case UBX_MSG_ACK_ACK:
		if (_rx_payload_length != sizeof(ubx_payload_rx_ack_ack_t)) {
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if _configured
		}

		break;

	case UBX_MSG_ACK_NAK:
		if (_rx_payload_length != sizeof(ubx_payload_rx_ack_nak_t)) {
			_rx_state = UBX_RXMSG_ERROR_LENGTH;

		} else if (_configured) {
			_rx_state = UBX_RXMSG_IGNORE;        // ignore if _configured
		}

		break;

	default:
		_rx_state = UBX_RXMSG_DISABLE;	// disable all other messages
		break;
	}

	switch (_rx_state) {
	case UBX_RXMSG_HANDLE:	// handle message
	case UBX_RXMSG_IGNORE:	// ignore message but don't report error
		ret = 0;
		break;

	case UBX_RXMSG_DISABLE:	// disable unexpected messages
		UBX_DEBUG("ubx msg 0x%04x len %u unexpected", SWAP16((unsigned)_rx_msg), (unsigned)_rx_payload_length);

		{
			gps_abstime t = gps_absolute_time();

			if (t > _disable_cmd_last + DISABLE_MSG_INTERVAL) {
				/* don't attempt for every message to disable, some might not be disabled */
				_disable_cmd_last = t;
				UBX_DEBUG("ubx disabling msg 0x%04x", SWAP16((unsigned)_rx_msg));

				if (!configureMessageRate(_rx_msg, 0)) {
					ret = -1;
				}
			}
		}

		ret = -1;	// return error, abort handling this message
		break;

	case UBX_RXMSG_ERROR_LENGTH:	// error: invalid length
		UBX_WARN("ubx msg 0x%04x invalid len %u", SWAP16((unsigned)_rx_msg), (unsigned)_rx_payload_length);
		ret = -1;	// return error, abort handling this message
		break;

	default:	// invalid message state
		UBX_WARN("ubx internal err1");
		ret = -1;	// return error, abort handling this message
		break;
	}

	return ret;
}

/**
 * Add payload rx byte
 */
int	// -1 = error, 0 = ok, 1 = payload completed
GPSDriverUBX::payloadRxAdd(const uint8_t b)
{
	int ret = 0;
	uint8_t *p_buf = (uint8_t *)&_buf;

	p_buf[_rx_payload_index] = b;

	if (++_rx_payload_index >= _rx_payload_length) {
		ret = 1;	// payload received completely
	}

	return ret;
}

/**
 * Add NAV-SVINFO payload rx byte
 */
int	// -1 = error, 0 = ok, 1 = payload completed
GPSDriverUBX::payloadRxAddNavSvinfo(const uint8_t b)
{
	int ret = 0;
	uint8_t *p_buf = (uint8_t *)&_buf;

	if (_rx_payload_index < sizeof(ubx_payload_rx_nav_svinfo_part1_t)) {
		// Fill Part 1 buffer
		p_buf[_rx_payload_index] = b;

	} else {
		if (_rx_payload_index == sizeof(ubx_payload_rx_nav_svinfo_part1_t)) {
			// Part 1 complete: decode Part 1 buffer
			_satellite_info->count = MIN(_buf.payload_rx_nav_svinfo_part1.numCh, satellite_info_s::SAT_INFO_MAX_SATELLITES);
			UBX_TRACE_SVINFO("SVINFO len %u  numCh %u", (unsigned)_rx_payload_length,
					 (unsigned)_buf.payload_rx_nav_svinfo_part1.numCh);
		}

		if (_rx_payload_index < sizeof(ubx_payload_rx_nav_svinfo_part1_t) + _satellite_info->count * sizeof(
			    ubx_payload_rx_nav_svinfo_part2_t)) {
			// Still room in _satellite_info: fill Part 2 buffer
			unsigned buf_index = (_rx_payload_index - sizeof(ubx_payload_rx_nav_svinfo_part1_t)) % sizeof(
						     ubx_payload_rx_nav_svinfo_part2_t);
			p_buf[buf_index] = b;

			if (buf_index == sizeof(ubx_payload_rx_nav_svinfo_part2_t) - 1) {
				// Part 2 complete: decode Part 2 buffer
				unsigned sat_index = (_rx_payload_index - sizeof(ubx_payload_rx_nav_svinfo_part1_t)) / sizeof(
							     ubx_payload_rx_nav_svinfo_part2_t);
				_satellite_info->used[sat_index]	= (uint8_t)(_buf.payload_rx_nav_svinfo_part2.flags & 0x01);
				_satellite_info->snr[sat_index]		= (uint8_t)(_buf.payload_rx_nav_svinfo_part2.cno);
				_satellite_info->elevation[sat_index]	= (uint8_t)(_buf.payload_rx_nav_svinfo_part2.elev);
				_satellite_info->azimuth[sat_index]	= (uint8_t)((float)_buf.payload_rx_nav_svinfo_part2.azim * 255.0f / 360.0f);
				_satellite_info->svid[sat_index]	= (uint8_t)(_buf.payload_rx_nav_svinfo_part2.svid);
				UBX_TRACE_SVINFO("SVINFO #%02u  used %u  snr %3u  elevation %3u  azimuth %3u  svid %3u",
						 (unsigned)sat_index + 1,
						 (unsigned)_satellite_info->used[sat_index],
						 (unsigned)_satellite_info->snr[sat_index],
						 (unsigned)_satellite_info->elevation[sat_index],
						 (unsigned)_satellite_info->azimuth[sat_index],
						 (unsigned)_satellite_info->svid[sat_index]
						);
			}
		}
	}

	if (++_rx_payload_index >= _rx_payload_length) {
		ret = 1;	// payload received completely
	}

	return ret;
}

/**
 * Add MON-VER payload rx byte
 */
int	// -1 = error, 0 = ok, 1 = payload completed
GPSDriverUBX::payloadRxAddMonVer(const uint8_t b)
{
	int ret = 0;
	uint8_t *p_buf = (uint8_t *)&_buf;

	if (_rx_payload_index < sizeof(ubx_payload_rx_mon_ver_part1_t)) {
		// Fill Part 1 buffer
		p_buf[_rx_payload_index] = b;

	} else {
		if (_rx_payload_index == sizeof(ubx_payload_rx_mon_ver_part1_t)) {
			// Part 1 complete: decode Part 1 buffer and calculate hash for SW&HW version strings
			_ubx_version = fnv1_32_str(_buf.payload_rx_mon_ver_part1.swVersion, FNV1_32_INIT);
			_ubx_version = fnv1_32_str(_buf.payload_rx_mon_ver_part1.hwVersion, _ubx_version);
			UBX_DEBUG("VER hash 0x%08x", _ubx_version);
			UBX_DEBUG("VER hw  \"%10s\"", _buf.payload_rx_mon_ver_part1.hwVersion);
			UBX_DEBUG("VER sw  \"%30s\"", _buf.payload_rx_mon_ver_part1.swVersion);
		}

		// fill Part 2 buffer
		unsigned buf_index = (_rx_payload_index - sizeof(ubx_payload_rx_mon_ver_part1_t)) % sizeof(
					     ubx_payload_rx_mon_ver_part2_t);
		p_buf[buf_index] = b;

		if (buf_index == sizeof(ubx_payload_rx_mon_ver_part2_t) - 1) {
			// Part 2 complete: decode Part 2 buffer
			UBX_DEBUG("VER ext \" %30s\"", _buf.payload_rx_mon_ver_part2.extension);
		}
	}

	if (++_rx_payload_index >= _rx_payload_length) {
		ret = 1;	// payload received completely
	}

	return ret;
}

/**
 * Finish payload rx
 */
int	// 0 = no message handled, 1 = message handled, 2 = sat info message handled
GPSDriverUBX::payloadRxDone()
{
	int ret = 0;
	//PX4_INFO("here");
	struct vehicle_gps_position_s pos2;

	char serial_buf[1];
	if(flag ==1)
	{
		initialise_uart();
		sbp_setup();
		
		//struct vehicle_gps_position_s pos;
		memset(&pos2, 0, sizeof(pos2));
		flag = 0;
	//uint8_t serial_buf[GPS_READ_BUFFER_SIZE];
	 //int len = read2(serial_buf, sizeof(serial_buf),20);
	}
	sbp_process(&sbp_state, &fifo_read);
	int len = ::read(_uart_fd, serial_buf, sizeof(serial_buf));
	//PX4_INFO("read amount: %d",len);
		if (len > 0) {
		//mavlink_message_t msg;
		for (int i = 0; i < len; i++) {	
			 int test = fifo_write(serial_buf[i]);
			 if(test == 0){
			 	PX4_INFO("fifo full: %d \n",test );
			 	}
			}
		}
	
	//_gps_position->satellites_used	= 11;//_buf.payload_rx_nav_pvt.numSV;
				//int32_t latest = pos2.lat;//423736160;
				//int32_t longtest =  -711097340;
				_gps_position->fix_type = 5;
				//_gps_position->lat		= latest;//_buf.payload_rx_nav_pvt.lat;
				//_gps_position->lon		= longtest;//_buf.payload_rx_nav_pvt.lon;
				//_gps_position->alt		= 10.0;//_buf.payload_rx_nav_pvt.hMSL;
				// _satellite_info->count = 11;
				// _satellite_info->used[0]	= 10;
				_satellite_info->snr[0]		= 1;
				_satellite_info->elevation[0]	= 5;
				_satellite_info->azimuth[0]	= 255.0f / 360.0f;
				_satellite_info->svid[0]	= 1;
				_gps_position->eph		= 9999;
				_gps_position->epv		= 9999;
				_gps_position->timestamp = gps_absolute_time();

						_gps_position->timestamp = gps_absolute_time();
		_last_timestamp_time = _gps_position->timestamp;

		_rate_count_vel++;
		_rate_count_lat_lon++;

		_got_posllh = true;
		_got_velned = true;
		//PX4_INFO("ret1");

			DO_EVERY(10,
		pos2.lat = pos_llh.lat* 1E7; 
		_gps_position->lat = pos2.lat;
		pos2.lon = pos_llh.lon* 1E7; 
		_gps_position->lon	= pos2.lon ;
		_gps_position->satellites_used	= pos_llh.n_sats;
		_gps_position->alt = pos_llh.height* 1E7;
		_gps_position->eph = (dops.hdop/100);
		_gps_position->epv	= (dops.vdop/100);

		_gps_position->vel_n_m_s	= (int)vel_ned.n;
 		_gps_position->vel_e_m_s	= (int)vel_ned.e;
 		_gps_position->vel_d_m_s	= (int)vel_ned.d;


		_satellite_info->count = pos_llh.n_sats;
		_satellite_info->used[0]= pos_llh.n_sats;
		//PX4_INFO("%d", pos2.lat);
		);

		ret = 1;
	if (ret > 0) {
		//PX4_INFO("ret >0");
		_gps_position->timestamp_time_relative = (int32_t)(_last_timestamp_time - _gps_position->timestamp);
	}

	return ret;

// 		_gps_position->fix_type = 6;
// 		_gps_position->lat		= latest;//_buf.payload_rx_nav_pvt.lat;
// 		_gps_position->lon		= longtest;//_buf.payload_rx_nav_pvt.lon;
// 		_gps_position->alt		= 5.0;//_buf.payload_rx_nav_pvt.hMSL;
// 		_gps_position->alt_ellipsoid	= _buf.payload_rx_nav_pvt.height;

// 		_gps_position->eph		= (float)_buf.payload_rx_nav_pvt.hAcc * 1e-3f;
// 		_gps_position->epv		= (float)_buf.payload_rx_nav_pvt.vAcc * 1e-3f;
// 		_gps_position->s_variance_m_s	= (float)_buf.payload_rx_nav_pvt.sAcc * 1e-3f;

// 		_gps_position->vel_m_s		= (float)_buf.payload_rx_nav_pvt.gSpeed * 1e-3f;

// 		_gps_position->vel_n_m_s	= (float)_buf.payload_rx_nav_pvt.velN * 1e-3f;
// 		_gps_position->vel_e_m_s	= (float)_buf.payload_rx_nav_pvt.velE * 1e-3f;
// 		_gps_position->vel_d_m_s	= (float)_buf.payload_rx_nav_pvt.velD * 1e-3f;

// 		_gps_position->cog_rad		= (float)_buf.payload_rx_nav_pvt.headMot * M_DEG_TO_RAD_F * 1e-5f;
// 		_gps_position->c_variance_rad	= (float)_buf.payload_rx_nav_pvt.headAcc * M_DEG_TO_RAD_F * 1e-5f;

// 		//Check if time and date fix flags are good
// 		if ((_buf.payload_rx_nav_pvt.valid & UBX_RX_NAV_PVT_VALID_VALIDDATE)
// 		    && (_buf.payload_rx_nav_pvt.valid & UBX_RX_NAV_PVT_VALID_VALIDTIME)
// 		    && (_buf.payload_rx_nav_pvt.valid & UBX_RX_NAV_PVT_VALID_FULLYRESOLVED)) {
// 			/* convert to unix timestamp */
// 			struct tm timeinfo;
// 			memset(&timeinfo, 0, sizeof(timeinfo));
// 			timeinfo.tm_year	= _buf.payload_rx_nav_pvt.year - 1900;
// 			timeinfo.tm_mon		= _buf.payload_rx_nav_pvt.month - 1;
// 			timeinfo.tm_mday	= _buf.payload_rx_nav_pvt.day;
// 			timeinfo.tm_hour	= _buf.payload_rx_nav_pvt.hour;
// 			timeinfo.tm_min		= _buf.payload_rx_nav_pvt.min;
// 			timeinfo.tm_sec		= _buf.payload_rx_nav_pvt.sec;

// 		_gps_position->lat	= _buf.payload_rx_nav_posllh.lat;
// 		_gps_position->lon	= _buf.payload_rx_nav_posllh.lon;
// 		_gps_position->alt	= _buf.payload_rx_nav_posllh.hMSL;
// 		_gps_position->eph	= (float)_buf.payload_rx_nav_posllh.hAcc * 1e-3f; // from mm to m
// 		_gps_position->epv	= (float)_buf.payload_rx_nav_posllh.vAcc * 1e-3f; // from mm to m
// 		_gps_position->alt_ellipsoid = _buf.payload_rx_nav_posllh.height;

// 		_gps_position->timestamp = gps_absolute_time();


// 			struct tm timeinfo;
// 			memset(&timeinfo, 0, sizeof(tm));
// 			timeinfo.tm_year	= _buf.payload_rx_nav_timeutc.year - 1900;
// 			timeinfo.tm_mon		= _buf.payload_rx_nav_timeutc.month - 1;
// 			timeinfo.tm_mday	= _buf.payload_rx_nav_timeutc.day;
// 			timeinfo.tm_hour	= _buf.payload_rx_nav_timeutc.hour;
// 			timeinfo.tm_min		= _buf.payload_rx_nav_timeutc.min;
// 			timeinfo.tm_sec		= _buf.payload_rx_nav_timeutc.sec;
// 			timeinfo.tm_isdst	= 0;


// 				_gps_position->time_utc_usec = static_cast<uint64_t>(epoch) * 1000000ULL;
// 				_gps_position->time_utc_usec += _buf.payload_rx_nav_timeutc.nano / 1000;

// 			} else {
// 				_gps_position->time_utc_usec = 0;
// 			}

// #else
// 			_gps_position->time_utc_usec = 0;
// #endif
// 		}

// 		_last_timestamp_time = gps_absolute_time();



// 	case UBX_MSG_NAV_SVIN:
// 		UBX_TRACE_RXMSG("Rx NAV-SVIN");
// 			PX4_INFO("Rx NAV-SVIN");
// 		{
// 			ubx_payload_rx_nav_svin_t &svin = _buf.payload_rx_nav_svin;

// 			UBX_DEBUG("Survey-in status: %is cur accuracy: %imm nr obs: %i valid: %i active: %i",
// 				  svin.dur, svin.meanAcc / 10, svin.obs, (int)svin.valid, (int)svin.active);

// 			SurveyInStatus status;
// 			memset(&status, 0, sizeof(status));
// 			status.duration = svin.dur;
// 			status.mean_accuracy = svin.meanAcc / 10;
// 			status.flags = (svin.valid & 1) | ((svin.active & 1) << 1);
// 			surveyInStatus(status);

// 			if (svin.valid == 1 && svin.active == 0) {
// 				/* We now switch to 1 Hz update rate, which is enough for RTCM output.
// 				 * For the survey-in, we still want 5 Hz, because this speeds up the process */
// 				memset(&_buf.payload_tx_cfg_rate, 0, sizeof(_buf.payload_tx_cfg_rate));
// 				_buf.payload_tx_cfg_rate.measRate	= 1000;
// 				_buf.payload_tx_cfg_rate.navRate	= UBX_TX_CFG_RATE_NAVRATE;
// 				_buf.payload_tx_cfg_rate.timeRef	= UBX_TX_CFG_RATE_TIMEREF;



// 		_gps_position->vel_m_s		= (float)_buf.payload_rx_nav_velned.speed * 1e-2f;
// 		_gps_position->vel_n_m_s	= (float)_buf.payload_rx_nav_velned.velN * 1e-2f; /* NED NORTH velocity */
// 		_gps_position->vel_e_m_s	= (float)_buf.payload_rx_nav_velned.velE * 1e-2f; /* NED EAST velocity */
// 		_gps_position->vel_d_m_s	= (float)_buf.payload_rx_nav_velned.velD * 1e-2f; /* NED DOWN velocity */
// 		_gps_position->cog_rad		= (float)_buf.payload_rx_nav_velned.heading * M_DEG_TO_RAD_F * 1e-5f;
// 		_gps_position->c_variance_rad	= (float)_buf.payload_rx_nav_velned.cAcc * M_DEG_TO_RAD_F * 1e-5f;
// 		_gps_position->vel_ned_valid	= true;

// 		_rate_count_vel++;
// 		_got_velned = true;



// 		case sizeof(ubx_payload_rx_mon_hw_ubx7_t):	/* u-blox 7+ msg format */
// 			_gps_position->noise_per_ms		= _buf.payload_rx_mon_hw_ubx7.noisePerMS;
// 			_gps_position->jamming_indicator	= _buf.payload_rx_mon_hw_ubx7.jamInd;



}

void
GPSDriverUBX::decodeInit()
{
	_decode_state = UBX_DECODE_SYNC1;
	_rx_ck_a = 0;
	_rx_ck_b = 0;
	_rx_payload_length = 0;
	_rx_payload_index = 0;

	if (_output_mode == OutputMode::RTCM) {
		if (!_rtcm_message) {
			_rtcm_message = new rtcm_message_t;
			_rtcm_message->buffer = new uint8_t[RTCM_INITIAL_BUFFER_LENGTH];
			_rtcm_message->buffer_len = RTCM_INITIAL_BUFFER_LENGTH;
		}

		_rtcm_message->pos = 0;
		_rtcm_message->message_length = _rtcm_message->buffer_len;
	}
}

void
GPSDriverUBX::addByteToChecksum(const uint8_t b)
{
	_rx_ck_a = _rx_ck_a + b;
	_rx_ck_b = _rx_ck_b + _rx_ck_a;
}

void
GPSDriverUBX::calcChecksum(const uint8_t *buffer, const uint16_t length, ubx_checksum_t *checksum)
{
	for (uint16_t i = 0; i < length; i++) {
		checksum->ck_a = checksum->ck_a + buffer[i];
		checksum->ck_b = checksum->ck_b + checksum->ck_a;
	}
}

bool
GPSDriverUBX::configureMessageRate(const uint16_t msg, const uint8_t rate)
{
	ubx_payload_tx_cfg_msg_t cfg_msg;	// don't use _buf (allow interleaved operation)
	memset(&cfg_msg, 0, sizeof(cfg_msg));

	cfg_msg.msg	= msg;
	cfg_msg.rate	= rate;

	return sendMessage(UBX_MSG_CFG_MSG, (uint8_t *)&cfg_msg, sizeof(cfg_msg));
}

bool
GPSDriverUBX::configureMessageRateAndAck(uint16_t msg, uint8_t rate, bool report_ack_error)
{
	if (!configureMessageRate(msg, rate)) {
		return false;
	}

	return waitForAck(UBX_MSG_CFG_MSG, UBX_CONFIG_TIMEOUT, report_ack_error) >= 0;
}

bool
GPSDriverUBX::sendMessage(const uint16_t msg, const uint8_t *payload, const uint16_t length)
{
	ubx_header_t   header = {UBX_SYNC1, UBX_SYNC2, 0, 0};
	ubx_checksum_t checksum = {0, 0};

	// Populate header
	header.msg	= msg;
	header.length	= length;

	// Calculate checksum
	calcChecksum(((uint8_t *)&header) + 2, sizeof(header) - 2, &checksum); // skip 2 sync bytes

	if (payload != nullptr) {
		calcChecksum(payload, length, &checksum);
	}

	// Send message
	if (write((void *)&header, sizeof(header)) != sizeof(header)) {
		return false;
	}

	if (payload && write((void *)payload, length) != length) {
		return false;
	}

	if (write((void *)&checksum, sizeof(checksum)) != sizeof(checksum)) {
		return false;
	}

	return true;
}

uint32_t
GPSDriverUBX::fnv1_32_str(uint8_t *str, uint32_t hval)
{
	uint8_t *s = str;

	/*
	 * FNV-1 hash each octet in the buffer
	 */
	while (*s) {

		/* multiply by the 32 bit FNV magic prime mod 2^32 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= FNV1_32_PRIME;
#else
		hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
#endif

		/* xor the bottom with the current octet */
		hval ^= (uint32_t) * s++;
	}

	/* return our new hash value */
	return hval;
}

void
GPSDriverUBX::setSurveyInSpecs(uint32_t survey_in_acc_limit, uint32_t survey_in_min_dur)
{
	_survey_in_acc_limit = survey_in_acc_limit;
	_survey_in_min_dur = survey_in_min_dur;
}

