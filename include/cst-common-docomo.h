/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef _CST_COMMON_DOCOMO_H_
#define _CST_COMMON_DOCOMO_H_

#include <stdio.h>
#include <glib.h>
#include <Elementary.h>

#include "cst-common.h"
#include "cst-common-string.h"
#include "cst-debug.h"

/* Macro represents the limit of the USSD string common for send/receive USSD */
#define CST_MAX_USSD_REQ_RESP_STRING	208

/* Macro represents the possible maximum number of response SI data from the
	 network */
#define CST_MAX_USSD_RESP_SI_DATA	7

/* Macro represents the length limit of each SI data in the response USSD
	 string */
#define CST_MAX_USSD_RESP_SI_DATA_LEN	20

/* Macro represents the max possible length of the Service Code in the
	 response USSD string */
#define CST_MAX_USSD_SERVICE_CODE_LEN	3

#define CST_MAX_RINGTIME_LENGTH 3
#define CST_USSD_SI1_RINGTIME_SETTING_DOCOMO "2"

/* Signature of the callback function for UI response handling */
typedef void (*cst_ussd_resp_ui_docomo_cb)(char *resp_result, void *user_data);

typedef enum {
	CST_CB_DOCOMO_DEACTIVATED,
	CST_CB_DOCOMO_ALL_INCOMING_ACTIVATED,
	CST_CB_DOCOMO_ALL_INCOMING_VIDEO_ACTIVATED,
} cst_call_barring_status_docomo_t;
/*
 * @enum cst_ussd_code_docomo_t
 * This enumeration defines the USSD/SS code for various Telephony operations.
 */
typedef enum {
	/* Call waiting service */
	CST_CALL_WAITING_SVC_DOCOMO = 43,
	/* Call forwarding service */
	CST_CALL_FORWARDING_SVC_DOCOMO = 120,
	/* Voice mail service */
	CST_VOICE_MAIL_SVC_DOCOMO = 121,
	/* Nuisance Call Blocking Service */
	CST_NUISANCE_CALL_BLOCKING_SVC_DOCOMO = 122,
	/* Number of message notification service */
	CST_MSG_NOTI_SVC_DOCOMO = 124,
	/* Remote control setting(Remote lock) service */
	CST_REMOTE_CONTROL_SVC_DOCOMO = 125,
	/* International roaming service */
	CST_INTERNATIONAL_ROAMING_SVC_DOCOMO = 126,
	/* Roaming call notification service */
	CST_ROAMING_CALL_NOTI_SVC_DOCOMO = 126,
	/* Missed call notification service when outside the n/w coverage */
	CST_MISSED_CALL_NOTI_SVC_DOCOMO = 127,
	/* English Guidance service */
	CST_ENGLISH_GUIDANCE_SVC_DOCOMO = 145,
	/* Function selection service/Busy incoming call setting*/
	CST_BUSY_INCOMING_CALL_SVC_DOCOMO = 146,
	/* Caller ID service */
	CST_CALLER_ID_SVC_DOCOMO = 148,
	/* Caller ID display request service */
	CST_CALLER_ID_DISPLAY_REQUEST_SVC_DOCOMO = 149,
	/* Incoming when roaming regulation service */
	CST_INCOMING_WHEN_ROAMING_SVC_DOCOMO = 351,
	/* Public Mode(Power Off) Service */
	CST_PUBLIC_MODE_SVC_DOCOMO = 139,

	CST_SVC_NONE,
} cst_ussd_code_docomo_t;

/*
 * @enum cst_req_resp_msg_docomo_t
 * This enumeration defines the USSD/SS code for various Telephony operations.
 */
typedef enum {
	CST_REQ_NONE_DOCOMO = 0,

	CST_VOICE_MAIL_ACTIVATE_DOCOMO,
	CST_VOICE_MAIL_RING_TIME_SETTING_DOCOMO,
	CST_VOICE_MAIL_DEACTIVATE_DOCOMO,
	CST_VOICE_MAIL_FETCH_SETTING_DOCOMO,
	CST_VOICE_MAIL_CHECK_MSGS_DOCOMO,
	CST_VOICE_MAIL_MSG_NOTIFICATION_DOCOMO,

	CST_CALL_FORWARDING_ACTIVATE_DOCOMO,
	CST_CALL_FORWARDING_DEACTIVATE_DOCOMO,
	CST_CALL_FORWARDING_CHANGE_DOCOMO,
	CST_CALL_FORWARDING_SETTINGS_DOCOMO,
	CST_CALL_FORWARDING_GUIDANCE_SETTING_DOCOMO,
	CST_CALL_FORWARDING_FETCH_SETTING_DOCOMO,

	CST_CALL_WAITING_ACTIVATE_DOCOMO,
	CST_CALL_WAITING_DEACTIVATE_DOCOMO,
	CST_CALL_WAITING_FETCH_SETTING_DOCOMO,

	CST_CALLER_ID_NOTI_CHECK_SETTING_DOCOMO,
	CST_CALLER_ID_NOTI_CHANGE_TO_NOT_NOTIFY_DOCOMO,
	CST_CALLER_ID_NOTI_CHANGE_TO_NOTIFY_DOCOMO,

	CST_MISSED_CALL_NOTI_ACTIVATE_DOCOMO,
	CST_MISSED_CALL_NOTI_DEACTIVATE_DOCOMO,
	CST_MISSED_CALL_NOTI_FETCH_SETTING_DOCOMO,

	CST_NUISANCE_CALL_BLOCK_REG_DOCOMO,
	CST_NUISANCE_CALL_BLOCK_REG_SPEC_PHONE_NUM_DOCOMO,
	CST_NUISANCE_CALL_BLOCK_ONE_REG_NUM_DOCOMO,
	CST_NUISANCE_CALL_BLOCK_DEL_ALL_REG_NUM_DOCOMO,
	CST_NUISANCE_CALL_BLOCK_CHK_REG_PHONE_NUMS_DOCOMO,
	CST_NUISANCE_CALL_BLOCK_ADV_SETTINGS_DOCOMO,

	CST_ENG_GUIDANCE_SETTINGS_OUTGOING_DOCOMO,
	CST_ENG_GUIDANCE_SETTINGS_INGOING_DOCOMO,
	CST_ENG_GUIDANCE_FETCH_SETTING_DOCOMO,

	CST_SECOND_CALL_ACTIVATE_DOCOMO,
	CST_SECOND_CALL_DEACTIVATE_DOCOMO,
	CST_SECOND_CALL_FETCH_SETTING_DOCOMO,

	CST_CALLER_ID_REQ_SVC_ACTIVATE_DOCOMO,
	CST_CALLER_ID_REQ_SVC_DEACTIVATE_DOCOMO,
	CST_CALLER_ID_REQ_SVC_FETCH_SETTING_DOCOMO,

	CST_REMOTE_OPERATION_ACTIVATE_DOCOMO,
	CST_REMOTE_OPERATION_DEACTIVATE_DOCOMO,
	CST_REMOTE_OPERATION_FETCH_SETTING_DOCOMO,

	CST_ROAMING_GUIDANCE_ACTIVATE_DOCOMO,
	CST_ROAMING_GUIDANCE_DEACTIVATE_DOCOMO,
	CST_ROAMING_GUIDANCE_FETCH_SETTING_DOCOMO,

	CST_INTRL_DIAL_ASSIST_AUTO_CONVERSION_DOCOMO,
	CST_INTRL_DIAL_ASSIST_COUNTRY_CODE_DOCOMO,
	CST_INTRL_DIAL_ASSIST_PREFIX_DOCOMO,

	CST_ADD_SVC_REG_USSD_DOCOMO,
	CST_ADD_SVC_REG_REPLY_MSG_DOCOMO,

	CST_ROAMING_CALL_NOTI_ACTIVATE_DOCOMO,
	CST_ROAMING_CALL_NOTI_DEACTIVATE_DOCOMO,
	CST_ROAMING_CALL_NOTI_FETCH_SETTING_DOCOMO,

	CST_RESTRICT_ALL_INCOMING_CALL_ACTIVATE_DOCOMO,
	CST_RESTRICT_ALL_INCOMING_VIDEO_CALL_ACTIVATE_DOCOMO,
	CST_RESTRICT_ALL_INCOMING_CALL_DEACTIVATE_DOCOMO,
	CST_RESTRICT_ALL_INCOMING_CALL_FETCH_SETTING_DOCOMO,

	CST_PUBLIC_MODE_ACTIVATE_DOCOMO,
	CST_PUBLIC_MODE_DEACTIVATE_DOCOMO,
	CST_PUBLIC_MODE_FETCH_SETTING_DOCOMO,
} cst_req_resp_msg_docomo_t;

/*
 * @enum cst_ussd_resp_result_docomo_t
 * This enumeration defines the 'result' of the USSD operation after parsing the
 * response USSD string.
 */
typedef enum {
	CST_USSD_RESP_FAILED = 0,
	CST_USSD_RESP_SUCCESS,
	CST_USSD_RESP_UNSUBSCRIBED_SVC,

	CST_USSD_RESP_NONE = -1,
} cst_ussd_resp_result_docomo_t;

/*
 * @struct cst_req_ussd_data_docomo_t
 * This struct contains the data which is used to generate the USSD string in
 * the generator function before sending it to network.
 */
typedef struct {
	cst_req_resp_msg_docomo_t ussd_msg;
	char *si1_data;
	char *si2_data;
	char *si3_data;
	char *si4_data;
} cst_req_ussd_data_docomo_t;

/*
 * @struct cst_resp_ussd_data_docomo_t
 * This struct contains the data which is filled in the parser function based
 * upon the response string from the network for the telephony request being
 * made.
 */
typedef struct {
	cst_ussd_code_docomo_t ussd_code;
	int si_data_num;
	char si_datas[CST_MAX_USSD_RESP_SI_DATA][CST_MAX_USSD_RESP_SI_DATA_LEN];
} cst_resp_ussd_data_docomo_t;

#endif /* _CST_COMMON_DOCOMO_H_ */

