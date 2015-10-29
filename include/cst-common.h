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

#ifndef _CST_COMMON_H_
#define _CST_COMMON_H_

#include <Elementary.h>
#include <efl_extension.h>
#include <libintl.h>
#include <glib.h>
#include <app.h>
#include <TapiUtility.h>
#include <tapi_common.h>

#include "cst-debug.h"
#include <msg_types.h>

#ifndef CST_MODULE_EXPORT
#define CST_MODULE_EXPORT __attribute__ ((visibility("default")))
#endif

#define UGNAME "setting-call-efl"
#define _EDJ(o)		elm_layout_edje_get(o)
#define EDJ_DIR "/usr/apps/setting-call-efl/res/edje/"
#define EDJ_NAME EDJ_DIR"/call-setting.edj"

#define THEME_NAME EDJ_DIR"/call-setting-theme.edj"
#define IMG_DIR "/usr/apps/setting-call-efl/res/images"
#define CST_DEBUG_FILE "/usr/apps/setting-call-efl/shared/trusted/.tmp/debug/call-setting.log"
#define CST_LOCALE      "/usr/apps/setting-call-efl/res/locale"

#define CST_MAX_PASSWORD_LEN		4
#define CST_MAX_REJECT_NUM		30
#define CST_MAX_PREFIX_NUM		1
#define CST_MAX_REJECT_MESSAGE_NUM	5
#define CST_MAX_ITEM_TEXT_BUFFER_LEN	140
#define CST_MAX_REJECT_MSG_CHAR		160
#define CST_MAX_REJECT_MSG_CHAR_UNICODE		140
#define CST_MAX_PHONE_NUMBER_LEN	82
#define CST_MAX_AUTO_AREA_CODE_LEN	40
#define CST_MAX_CF_NUMBER_LEN	82
#define CST_MAX_PREFIX_NUMBER_LEN	20
#define CST_MAX_COUNTRY_PREFIX_NUMBER_LEN	6
#define CST_MAX_COUNTRY_NAME_LEN	14
#define CST_MAX_POPUP_MESSAGE_LEN 256
#define CST_NAVIFRAME_ITEM_POP_TIMER 0.15
#define CST_POPUP_TIMER 3.0
#define CST_SHOW_PROGRESSBAR_POPUP_TIMER 0.00000001
#define CST_MAX_NUM_AUTO_REJECT_LIST 100
#define CST_DISPLAY_NAME_LENGTH_MAX		(255+1)

#define CST_TITLE_BTN_ICON_BACK					IMG_DIR"/00_icon_back.png"
#define CST_TITLE_BTN_ICON_PLUS					IMG_DIR"/00_icon_plus.png"
#define CST_TITLE_BTN_ICON_EDIT					IMG_DIR"/00_icon_edit.png"
#define CST_TITLE_BTN_ICON_SELECT_ALL			IMG_DIR"/T01_title_icon_select_all.png"
#define CST_TITLE_BTN_ICON_ADD					IMG_DIR"/T01_title_icon_add.png"
#define CST_TITLE_BTN_ICON_DELETE				IMG_DIR"/T01_title_icon_delete.png"
#define CST_CTRL_ICON_CLOSE						IMG_DIR"/01_controlbar_icon_close.png"
#define CST_CTRL_ICON_CREATE					IMG_DIR"/01_controlbar_icon_create.png"
#define CST_CTRL_ICON_DELETE					IMG_DIR"/01_controlbar_icon_delete.png"
#define CST_CTRL_ICON_EDIT						IMG_DIR"/01_controlbar_icon_edit.png"
#define CST_CTRL_ICON_UPDATE					IMG_DIR"/01_controlbar_icon_update.png"
#define CST_CTRL_ICON_CONTACTS					IMG_DIR"/01_controlbar_icon_contacts.png"
#define CST_VIDEO_CALL_IMAGE_DEFAULT_PATH		IMG_DIR"/call_no_video_sim"
#define CST_VIDEO_CALL_IMAGE_CHANGED_PATH1		"/opt/usr/ug/data/ug-setting-call-efl/.changed_video_call_image.jpg"
#define CST_VIDEO_CALL_IMAGE_CHANGED_PATH2		"/opt/usr/ug/data/ug-setting-call-efl/.changed_video_call_image_.jpg"
#define CST_VIDEO_CALL_IMAGE_CHANGED_PATH3		"/opt/usr/ug/data/ug-setting-call-efl/.changed_video_call_image2.jpg"
#define CST_VIDEO_CALL_IMAGE_CHANGED_PATH4		"/opt/usr/ug/data/ug-setting-call-efl/.changed_video_call_image2_.jpg"
#define CST_CTRL_ICON_CONTACTS_DEF_IMG			IMG_DIR"/call_setting_address.png"
#define CST_NO_CONTENTS_ICON					IMG_DIR"/00_nocontents_text.png"

#define FDN_SIM_PB_FAIL_STR								"SIM Phonebook operation is failed for some unknown reason"
#define FDN_SIM_PB_INVALID_INDEX_STR					"The index is invalid"
#define FDN_SIM_PB_INVALID_NUMBER_LENGTH_STR			"The length of the number is invalid"
#define FDN_SIM_PB_INVALID_NAME_STR						"The length of the name is invalid"
#define FDN_SIM_PB_ACCESS_CONDITION_NOT_SATISFIED_STR	"The condition of access to Phonebook is not satisfied"

#define INAVLID_NUMBER_ERR_STR	"Invalid number."
#define INAVLID_PIN2_ERR_STR	"Type PIN2 that is 4 to 8 numbers."
#define OK_STR	"Ok"
#define CANCEL_STR	"Cancel"
#define EQ_PKG		"com.samsung.call-eq-analyzer"
#define GALLERY_UG	"gallery-efl-lite"
#define SETTING_ICON_PATH_CFG		"/usr/apps/com.samsung.setting-lite/res/icons/"

/**
 * The key of request bundle for type.
 * \n Value : convert id to string by using \%d.
 */
#define CST_UG_BUNDLE_VOICEMAIL_SIMSLOT "simslot"
#define CST_UG_BUNDLE_TYPE "type"
#define CST_UG_BUNDLE_VIEWTYPE "viewtype"
#define CST_UG_BUNDLE_KEYWORD "keyword"
#ifdef _CALL_SET_DUAL_SIM_ALWAYSON
#define CST_UG_BUNDLE_SIM1_NUMBER "number1"
#define CST_UG_BUNDLE_SIM2_NUMBER "number2"
#endif

/**
 * This enumeration defines the error codes returned from engine to UI
 */
typedef enum {
	CST_ERR_NONE = 0x00,						/**< numberOfPW_AttemptsViolation*/

	/*NETWORK ERRORS */
	/* Show message Not Available for Vodafone R11 */
	CST_ERR_UNKNOWN_SUBSCRIBER = 0x01,				/**<  unknownSubscriber  "Invalid User Number" */
	CST_ERR_BEARER_NOT_PROVISIONED = 0x02,				/**<  BearerServiceNotProvisioned "Request Rejected" */
	CST_ERR_TELESERVICE_NOT_PROVISIONED = 0x03,			/**<  TeleServiceNotProvisioned */
	CST_ERR_CALL_BARRED = 0x04,					/**<  CallBarred */
	CST_ERR_ILLEGAL_SS_OPER = 0x05,					/**<  illegalSS_Operation */
	CST_ERR_ERR_STATUS = 0x06,					/**<  ss_ErrorStatus */
	CST_ERR_NOT_AVAIL = 0x07,					/**<  ss_NotAvailable "Service not available" */
	CST_ERR_SUBSCRIPTION_VIOLATION = 0x08,				/**<  ss_SubscriptionViolation */
	CST_ERR_INCOMPATIBILITY = 0x09,					/**<  ss_Incompatibility */
	CST_ERR_FACILITY_NOT_SUPPORT = 0x0a,				/**<  FacilityNotSupported */
	CST_ERR_SYS_FAILURE = 0x0b,					/**<  "SystemFailure "Please try again" */
	CST_ERR_DATA_MISSING = 0x0c,					/**<  DataMissing */
	CST_ERR_UNEXPECTED_DATA = 0x0d,					/**<  UnexpectedDataValue 0x13 */
	CST_ERR_MPTY_PARTICIPANTS_EXCEED = 0x0e,			/**< maxNumberOfMPTY_ParticipantsExceeded */
	CST_ERR_RESOURCE_NOT_AVAIL = 0x0f,				/**<  ResourcesNotAvailable */
	/*Show message Password error for Vodafone R11 */
	CST_ERR_PW_REGIST_FAIL = 0x10,					/**< PasswordRegistrationFailure ( ex:password mismatch ) */
	CST_ERR_NEGATIVE_PW_CHECK = 0x11,				/**<  NegativePasswordCheck */
	/*Show message password error
	   this function has been locked, so please call customer center for Vodafone R11 */
	CST_ERR_PW_ATTEMPS_VIOLATION = 0x12,				/**<  numberOfPW_AttemptsViolation*/

	/*PHONE ERRORS */
	CST_ERR_NETWORK_API_FAILED = 0x13,				/**<  Telephony API call failed*/
	CST_ERR_DIVERT_TO_EMERGENCY_BLOCKED = 0x14,			/**<  Call diverting to an emergency number not allowed*/

	/* Call barring password mismatch errors */
	CST_ERR_CB_PWD_TOO_SORT = 0x15,					/**< Password is too short - less than 4 characters*/
	CST_ERR_CB_PWD_MISMATCH = 0x16,					/**< Password do not match - unequal password*/
	CST_ERR_CB_PWD_TOO_LONG = 0x17,					/**< Password is too long - greater than 4 characters*/

	CST_ERR_UNKNOWN = 0xff,						/**<  unknown error*/

} CstErr_t;

enum {
	CST_DIALOG_GROUP_NORMAL,
	CST_DIALOG_GROUP_END_WITH_HELP_TEXT,
	CST_DIALOG_GROUP_CALL_FORWARDING_BARRING_FDN_LIST,
	CST_DIALOG_GROUP_CALL_FORWARDING_WAITING_TIME,
};

enum {
	CST_SSTYPE_CF_UNCONDITIONAL,
	CST_SSTYPE_CF_BUSY,
	CST_SSTYPE_CF_NO_REPLY,
	CST_SSTYPE_CF_NOT_REACHABLE,
	CST_SSTYPE_CF_ALL,
	CST_SSTYPE_CF_ALL_CONDITIONAL,
	CST_SSTYPE_CB_OC,
	CST_SSTYPE_CB_OIC,
	CST_SSTYPE_CB_OICEH,
	CST_SSTYPE_CB_IC,
	CST_SSTYPE_CB_ICR,
	CST_SSTYPE_CW,
};

enum {
	CST_CF_CB_INTERROGATE_BY_NONE = 0,
	CST_CF_CB_INTERROGATE_BY_ACTIVATE = 1,
	CST_CF_CB_INTERROGATE_BY_DEACTIVATE = 2,
};

enum {
	CST_CLI_BY_NETWORK,
	CST_CLI_SHOW,
	CST_CLI_HIDE,
};

enum {
	CST_GENLIST_ENTRY_ITEM = -3,
	CST_GENLIST_ENTRY_ITEM2,
};

enum {
	CST_ALERTS_ON_CALL_OFF,
	CST_ALERTS_ON_CALL_SOUND,
	CST_ALERTS_ON_CALL_VIBRATION,
};

enum {
	CST_OUTGOING_CALL_CONDITIONS_PHONE_LOCKED,
	CST_OUTGOING_CALL_CONDITIONS_PHONE_UNLOCKED,
};

enum {
	CST_OUTGOING_CALLTYPE_LAST_CALL_LOG,
	CST_OUTGOING_CALLTYPE_VOICE,
	CST_OUTGOING_CALLTYPE_VIDEO,
};

enum {
	CST_AUTO_ANSWER_2_SECONDS,
	CST_AUTO_ANSWER_5_SECONDS,
	CST_AUTO_ANSWER_10_SECONDS,
};

enum {
	CST_ACCEPT_CALL_DEVICE_BT_HEADSET,
	CST_ACCEPT_CALL_DEVICE_RECEIVER,
};

enum {
	CST_IN_CALL_SOUND_EQ_OFF,
	CST_IN_CALL_SOUND_EQ_SOFT_SOUND,
	CST_IN_CALL_SOUND_EQ_CLEAR_SOUND,
	CST_IN_CALL_SOUND_EQ_MY_SOUND,
};

#ifdef _TIZEN_LITE_CODE
typedef enum {
	CST_SELECTED_SIM1,
	CST_SELECTED_SIM2,
} CstSimSlot_t;
#endif /*Tizen Lite code */

enum {
	CST_IN_VOICE_MAIL_VIBRATE_ALWAYS_DOCOMO,
	CST_IN_VOICE_MAIL_ONLY_IN_SILENT_MODE_DOCOMO,
	CST_IN_NEVER_DOCOMO,
};

enum {
	CST_STATUS_ACTIVATED,
	CST_STATUS_DEACTIVATED,
	CST_STATUS_UNKNOWN,
};

enum {
	CST_CALLTYPE_VOICE,
	CST_CALLTYPE_VOICE_LINE2,
	CST_CALLTYPE_VIDEO,
	CST_CALLTYPE_ALL,
};

enum {
	CST_ACTION_ACTIVATE,
	CST_ACTION_DEACTIVATE,
	CST_ACTION_REGISTER,
	CST_ACTION_QUERY,
};

enum {
	CST_SS_STATE_PROGRESS_INIT,
	CST_SS_STATE_ON,
	CST_SS_STATE_OFF,
	CST_SS_STATE_PROGRESS,
};

enum {
	CST_ERROR_NONE,
	CST_ERROR_SERVICE_UNAVAILABLE,
	CST_ERROR_INCORRECT_OPERATION,
	CST_ERROR_INVALID_PASSWORD,
	CST_ERROR_PASSWORD_BLOCKED,
	CST_ERROR_ENTER_NUMBER,
	CST_ERROR_WRONG_NUMBER,
	CST_ERROR_ENTER_MESSAGE,
	CST_ERROR_SELECT_VALID_ENTRY,
	CST_ERROR_DUPLICATE_MESSAGE,
	CST_ERROR_REJECTED_BY_NETWORK,
	CST_ERROR_INSERT_SIM_CARD,
	CST_ERROR_CHANGE_FLIGHT_MODE,
	CST_ERROR_AUL_LAUNCH_ERROR,
	CST_ERROR_CHANGE_VOICEMAIL_NUMBER,
	CST_ERROR_OEM_NOT_SUPPORTED,
	CST_ERROR_OEM_NOT_SUPPORTED_USER_BUSY,
	CST_ERROR_OEM_NOT_SUPPORTED_NO_REPLY,
	CST_ERROR_OEM_NOT_SUPPORTED_OUT_OF_REACH,
	CST_ERROR_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED,
	CST_ERROR_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED_PD,
	CST_ERROR_MAXIMUM_DIGITS_OF_PHONE_NUMBER_REACHED,
	CST_ERROR_FDN_MODE_ACTIVE,
	CST_ERROR_NO_VOICEMAIL_NUM_CHANGED,
	CST_ERROR_UNKNOWN,
};
enum {
	CST_ITEM_1,
	CST_ITEM_2,
	CST_ITEM_3,
	CST_ITEM_4,
	CST_ITEM_5,
};

enum {
	CST_GL_ITEM_TEXT,
	CST_GL_ITEM_SUB_TEXT,
	CST_GL_ITEM_GROUP_INDEX,
	CST_GL_ITEM_1TEXT_ONOFF,
	CST_GL_ITEM_1TEXT_ONOFF_PROGRESS,
	CST_GL_ITEM_RADIO_1TEXT,
	CST_GL_ITEM_EXPANDABLE,
	CST_GL_ITEM_DG_SPERATOR,
	CST_GL_ITEM_DG_TITLE,
	CST_GL_ITEM_HELP_TEXT,
	CST_GL_ITEM_2TEXT_ONOFF,
	CST_GL_ITEM_2TEXT_ONOFF_PROGRESS,
	CST_GL_ITEM_IMG_1TEXT,
	CST_GL_ITEM_2TEXT,
	CST_GL_ITEM_2TEXT_EXPANDABLE,
	CST_GL_ITEM_2TEXT_1ICON,
	CST_GL_ITEM_NONE,
};

enum {
	CST_DL_AUTO_REJECT,
	CST_DL_REJECT_CALL_WITH_MSG,
	CST_DL_FDN_CONTACTS,
	CST_DL_NONE,
};

enum {
	CST_UG_REQ_MAIN_SCREEN = 0,

	/* IMPORTANT NOTE: CST_UG_REQ_AUTO_REJECT_LIST value is "1",
	 *  Donot modify this value. */
	CST_UG_REQ_AUTO_REJECT_LIST,
	CST_UG_REQ_AUTO_REJECT_MODE,
	CST_UG_REQ_REJECT_MESSAGES,
	CST_UG_REQ_ANSWER_END_CALLS,
	CST_UG_REQ_CALL_ALERTS,
	CST_UG_REQ_CALL_ACCESSORIES,
	CST_UG_REQ_ADDITIONAL_SETTINGS,
	CST_UG_REQ_CALL_WAITING,
	CST_UG_REQ_RING_KEYPAD_TONES,
	CST_UG_REQ_PERSONALIZE_CALL_SOUND,

	/* IMPORTANT NOTE: CST_UG_REQ_VOICE_MAIL value is "11",
	 *  passed by Phone app to launch call setting UG in voice mail edit mode.
	 *  Donot modify this value. */
	CST_UG_REQ_VOICE_MAIL = 11,
	CST_UG_REQ_VIDEO_CALL_IMAGE,
	/* IMPORTANT NOTE: CST_UG_REQ_DUAL_SIM_ALWAYS_ON value is "13",
	 *  passed by SIM Manager app to launch call setting UG for dual sim always on feature.
	 *  Donot modify this value. */
	CST_UG_REQ_DUAL_SIM_ALWAYS_ON,
};

typedef enum {
	CST_IME_REJECT_MSG,
	CST_IME_CALL_FORWARD,
	CST_IME_CALL_BAR,
	CST_IME_FDN_SETTING,
	CST_IME_FDN_CONTACT_NAME,
	CST_IME_FDN_CONTACT_NUMBER,
	CST_IME_FDN_CONTACT_PIN2,
	CST_IME_FDN_DELETE,
 	CST_IME_NONE,
} CstImeType_t;

typedef struct {
	int depth;
	int str_id;
	Elm_Genlist_Item_Type flags;
	int style;
	Evas_Object_Event_Cb func;
} CstGlItemDisplayInfo_t;

typedef struct {
	char reject_number[CST_MAX_PHONE_NUMBER_LEN+1];
	char reject_name[CST_DISPLAY_NAME_LENGTH_MAX];
	int is_activate;
	int reject_id;
	int rule;
} CstRejectInfo_t;

typedef struct _Item_Data {
	int index;
	int reject_id;
	int str_id;
	Elm_Object_Item *gl_item;
	Elm_Object_Item *parent_gl_item;
	char *label;
	Eina_Bool check;
	Evas_Object *radio;
	Evas_Object *eo_check;
	Eina_Bool is_activated;
	Eina_Bool is_unknown;
	Eina_List *list;
	char number[CST_MAX_PHONE_NUMBER_LEN + 1];
	char text[CST_MAX_ITEM_TEXT_BUFFER_LEN + 1];
	char name[CST_DISPLAY_NAME_LENGTH_MAX + 1];
	void *ugd;
	int style;
	int cf_cb_request_by;
} CstGlItemData_t;

typedef struct {
	Evas_Object *base;
	Evas_Object *bg;
	Evas_Object *win_main;
	Evas_Object *conform;
	void *ug;
	bool is_app_control_invoked;
	Evas_Object *nf;
	Evas_Object *controlbar;
	Evas_Object *back_button;
	Evas_Object *backup_layout;
	Evas_Object *cf_btn;

	int ug_req_type;

	double scale;

	/* IME */
	Evas_Object *dg_entry;
	Evas_Object *dg_entry_contact_name;
	Evas_Object *dg_entry_contact_number;
	Evas_Object *dg_entry_pin2;
	Evas_Object *dg_entry_callfwd_number;
	Evas_Object *dg_entry_ringtime;
	Evas_Object *dg_entry_change_callfwd_number;
	Evas_Object *dg_entry_password;
	Evas_Object *nxt_dg_entry_focus;

	char entry_string[256];
	char time[64];

	Eina_Bool b_expanded;

	Elm_Object_Item *gl_expanded_item;
	Elm_Object_Item *more_cst_navi_it;

	int genlist_editfield_initialized;
	/*Navigation Bar */
	Elm_Object_Item *c_item[10];

	Eina_Bool b_landscape_mode;

	/*Alerts On Call */
	Evas_Object *rdg_alerts_on_call;

	/*All Calls */
	/*Show My Number */
	Evas_Object *rdg_show_my_number;

	/* Outgoing Call Conditions */
	Evas_Object *rdg_outgoing_call_conditions;

	/* Outgoing Call Type */
	Evas_Object *rdg_outgoing_call_type;

	/* Auro answer timer */
	Evas_Object *rdg_auto_answer_timer;

	/* Accept Call in device */
	Evas_Object *rdg_accept_call_in_dev;

	/* In-call Sound EQ */
	Evas_Object *rdg_in_call_sound_eq;

	/* Call rejection - Auto reject mode */
	Evas_Object *rdg_auto_rej_mode;

	/* Auto Reject */
	Eina_List *reject_list;
	Eina_List *load_list;
	int reject_id;
	Evas_Object *rdg_reject_rule;
	Eina_Bool exist_reject_unknown;

	/*voice, video call */
	int cf_state[4];
	Elm_Object_Item *cf_gl_item[4];
	int cb_state[5];
	char cf_number[CST_MAX_PHONE_NUMBER_LEN][4];
	Evas_Object *popup;

	/* Call waiting */
	int cw_state;
	CstGlItemData_t *cw_gl_item_data;
	Eina_Bool original_state;

	/* Voice mail */
	msg_handle_t msg_handle;
	Elm_Object_Item *vm_gl_item;
#ifdef _TIZEN_LITE_CODE
	/* more-settings selected sim */
	int sel_sim;
	Elm_Object_Item *sim1_btn;
	Elm_Object_Item *sim2_btn;
#endif /*Tizen lite code */
	int call_type;
	int cb_flavour;
	int cf_flavour;
	Eina_List *req_queue;
	GSList *sim_req_list;
	Eina_Bool is_requesting;
	Evas_Object *backup_genlist;

#ifdef _CALL_SET_FDN_SUPPORT
	int fdn_contacts_count;
	int fdn_contact_number_len_max;
	int fdn_contact_name_len_max;
#endif

	int kind_of_delete_list;

	GSList *tel_event_subscription_list;
	Evas_Object *video_call_img;

	/* Tapi Handle */
	TapiHandle *tapi_handle; /*Handle that point's to the SIM on which it will be operated on*/
#ifdef _TIZEN_LITE_CODE
	TapiHandle *sim1_tapi_handle;
	TapiHandle *sim2_tapi_handle;
#endif
	int fdn_status;
	int fdn_initial_state;
	void (*fdn_status_update_func)();
	CstGlItemData_t *fdn_gl_item_data;
	int sim_lock_status;
	int sim_lock_retry_account;
	int rej_msg_seg_size;

	void *intl_dial_list;
	int intl_dial_list_cnt;
	void *intl_dial_prefix_list;
	int intl_dial_prefix_list_cnt;
	Evas_Object *del_btn;
	Evas_Object *more_btn;
	Evas_Object *contact_btn;
	Eina_Bool check_state;

	app_control_h image_service_handle;
	app_control_h vibration_service_handle;
	Evas_Object *rejct_popup;
	Evas_Object *rejctlist_popup;
	Evas_Object *fdnmore_popup;
	Evas_Object *entry_count;
	char block_list_prev_text[CST_MAX_PHONE_NUMBER_LEN];
#ifdef _CALL_SET_DUAL_SIM_ALWAYSON
	char sim1_number[CST_MAX_PHONE_NUMBER_LEN + 1];
	char sim2_number[CST_MAX_PHONE_NUMBER_LEN + 1];
	int dualsimreq_id;
	int dualsim_alwayson_status;
	int dualsim_source_destination;
	Evas_Object *dualsim1_dg_entry;
	Evas_Object *dualsim2_dg_entry;
	Evas_Object *rdg_dualsim_mode;
	app_control_h dualsim_service_handle;
#endif
} CstUgData_t;

#ifdef _CALL_SET_DUAL_SIM_ALWAYSON
typedef enum {
	DUAL_SIM_WAITING_REQ_SIM2 = 1,
	DUAL_SIM_FORWARDING_REQ_SIM1,
	DUAL_SIM_WAITING_REQ_SIM1,
	DUAL_SIM_FORWARDING_REQ_SIM2,
} DualSimReq_t;
#endif

typedef enum {
	SIM_REQUEST_WAITING_FOR_RESP,
	SIM_REQUEST_CANCELLED,
} SimReqStatus_t;

typedef enum {
	TAPI_SIM_REQ_NONE,
	TAPI_SIM_REQ_VERIFY_PIN2,
	TAPI_SIM_REQ_SET_FDN,
#if 0	// Donot delete this. Its for future use.
	TAPI_SIM_REQ_CHANGE_PIN2,
	TAPI_SIM_REQ_UNBLOCK_PIN2,
#endif
	TAPI_SIM_REQ_GET_PHNBUK_CNT,
	TAPI_SIM_REQ_GET_PHNBUK_REC,
	TAPI_SIM_REQ_ADD_PHNBUK_REC,
	TAPI_SIM_REQ_UPDATE_PHNBUK_REC,
	TAPI_SIM_REQ_DELETE_PHNBUK_REC,
	TAPI_SIM_REQ_GET_PHNBUK_METADATA,
} SimReqId_t;

typedef enum {
	TAPI_SIM_OPERATION_SUCCESS,
	TAPI_SIM_OPERATION_FAILURE,
	TAPI_SIM_OPERATION_INVALID_INDEX,
	TAPI_SIM_OPERATION_INVALID_NUMBER_LENGTH,
	TAPI_SIM_OPERATION_INVALID_NAME_LENGTH,
	TAPI_SIM_OPERATION_ACCESS_CONDITION_NOT_SATISFIED,

} SimAccessResult_t;

typedef enum {
	CST_CONTACT_ADD,
	CST_CONTACT_EDIT
} CstFDNContactOperation_t;

typedef enum {
	CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY,
	CST_ERR_POPUP_TYPE_TITLE_INFO_TEXT,
} CstErrPopupType_t;

#if 0	// Donot delete this. Its for future use.
typedef enum {
	CST_PWD_POPUP_TYPE_UNBLOCK_PIN2,
	CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW,
	CST_PWD_POPUP_TYPE_UNBLOCK_PIN2_NEW_CONFIRM,
	CST_PWD_POPUP_TYPE_FDN_STATUS,
	CST_PWD_POPUP_TYPE_CHANGE_PIN2,
	CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW,
	CST_PWD_POPUP_TYPE_CHANGE_PIN2_NEW_CONFIRM,
	CST_PWD_POPUP_TYPE_ADD_FDN_CONTACT,
	CST_PWD_POPUP_TYPE_DELETE_FDN_CONTACT,
} CstPaswdPopupType_t;
#endif


typedef void (*SimReqCb)(CstUgData_t *ugd, void *req, void *resp);

typedef struct {
	SimReqId_t req_id;
	unsigned char *pin2;
	int pin2_len;
	unsigned char *name;
	unsigned char *number;
	unsigned short index;
	unsigned short next_index;

} CallSettingSimNxtReqParams_t;

typedef struct {
	int tapi_req_id;
	SimReqStatus_t req_state;
	SimReqId_t req_id;
	CallSettingSimNxtReqParams_t *next_req_params;
	SimReqCb func;
	void *data;
	CstUgData_t *ugd;
} CallSettingSimReq_t;

typedef struct {
	unsigned short index;
	unsigned short next_index;
	unsigned char *name;
	unsigned char *number;
	unsigned int ton;
} CallSettingFDNRec_t;

typedef struct {
	Elm_Object_Item *gl_item;
	CallSettingFDNRec_t *phbk_info;
	void *ugd;
	gboolean item_delete_status;
} CstFDNGlItemData_t;

typedef struct {
	SimAccessResult_t result;
	int retry_count;
	int phnbuk_used_rec_cnt;
	int phnbuk_total_rec_cnt;

	CallSettingFDNRec_t fdn_rec;
} CallSettingSimResp_t;

typedef struct {
	unsigned short index;
	unsigned short next_index;
	unsigned char *name;
	unsigned char *number;
} CstFDNContactEditInfo_t;
#endif	/* _CST_COMMON_H_ */

