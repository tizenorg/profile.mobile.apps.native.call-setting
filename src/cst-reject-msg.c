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
#include <Elementary.h>
#include <vconf.h>
#include <msg.h>
#include <stdbool.h>

#include "cst-common.h"
#include "cst-common-string.h"
#include "cst-widget.h"
#include "cst-call-setting.h"
#include "cst-delete-list.h"

#include "cst-reject-msg.h"
#include "cst-reject-msg-ime.h"
#include "cst-util.h"

static void __cst_set_genlist_item_styles_reject_msg(void);
static void __cst_destroy_genlist_item_styles(void);
static void __cst_edit_reject_message_list(void *data, Evas_Object *obj, void *event_info);
static void __cst_reject_msg_back_to_prev(CstUgData_t *ugd);
static int _cst_set_reject_msg_button_status(int num);

static Elm_Genlist_Item_Class *itc_1text = NULL;
char *header = NULL;
static CstGlItemData_t *g_item_data = NULL;
static Elm_Object_Item *g_navi_it = NULL;
static Evas_Object *g_genlist = NULL;

static Evas_Object *ec_item[3];
static Evas_Object *save_btn;
static Evas_Object *call_btn;

typedef enum {
	REJECT_MSG_TEXT_UNDER_LIMIT_NUM,
	REJECT_MSG_TEXT_LIMIT_NUM,
	REJECT_MSG_TEXT_OVER_LIMIT_NUM,
} RejectMsgTextLimit_t;

static void __cst_destroy_genlist_item_styles(void)
{
	ENTER(__cst_destroy_genlist_item_styles);

	if (itc_1text) {
		_cst_destroy_genlist_item_class(itc_1text);
		itc_1text = NULL;
	}
}

static void __cst_reject_sms_changed_cb(keynode_t *node, void *data)
{
	ENTER(__cst_reject_sms_changed_cb);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	ret_if(item_data == NULL);

	int index = item_data->index;
	char *message = NULL;

	message = _cst_get_reject_message(index, EINA_TRUE, EINA_FALSE);
	ret_if(message == NULL);
	ret_if(item_data->label == NULL);
	free(item_data->label);
	item_data->label = message;
	ret_if(item_data->gl_item == NULL);
	elm_genlist_item_update(item_data->gl_item);
	LEAVE();
}

static void __cst_reject_sms_count_changed_cb(keynode_t *node, void *data)
{
	ENTER(__cst_reject_sms_count_changed_cb);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->backup_genlist) {
		vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR,
				__cst_reject_sms_changed_cb);
		vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR,
				__cst_reject_sms_changed_cb);
		vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR,
				__cst_reject_sms_changed_cb);
		vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR,
				__cst_reject_sms_changed_cb);
		vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR,
				__cst_reject_sms_changed_cb);
		vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR,
				__cst_reject_sms_changed_cb);

		elm_genlist_clear(ugd->backup_genlist);
	}

	_cst_genlist_append_reject_list_item(ugd, ugd->backup_genlist);
	_cst_set_reject_msg_button_status(_cst_get_num_of_reject_message());
	LEAVE();
}

static Eina_Bool _back_btn_clicked_reject_msg_cb(void *data, Elm_Object_Item *it)
{
	ENTER(_back_btn_clicked_reject_msg_cb);
	retv_if(NULL == data, EINA_TRUE);
	CstUgData_t *ugd = (CstUgData_t *)data;

	_cst_destroy_reject_message(ugd);

	if (ugd->ug_req_type == CST_UG_REQ_REJECT_MESSAGES) {
		ug_destroy_me(ugd->ug);
	}

	return EINA_TRUE;
}
static char *__cst_parse_input_string(char *input_string)
{
	ENTER(__cst_parse_input_string);
	retv_if(NULL == input_string, NULL);
	int i;
	char *parsed_message = NULL;

	parsed_message = (char *)calloc(strlen(input_string) + 1, sizeof(char));
	retv_if(NULL == parsed_message, NULL);

	for (i = 0; i < strlen(input_string); i++) {
		if (input_string[i] == '<' && input_string[i+1] == 'b' &&
			input_string[i+2] == 'r' && input_string[i+3] == '/' &&
			input_string[i+4] == '>') {
			i = i + 4;
		} else {
			int j = 0;
			for (; i < strlen(input_string); i++) {
				if (input_string[i] == '<' && input_string[i+1] == 'b' &&
					input_string[i+2] == 'r' && input_string[i+3] == '/' &&
					input_string[i+4] == '>') {
					if (parsed_message[j-1] != ' ') {
						parsed_message[j] = ' ';
						j++;
					}
					i = i + 4;
				} else {
					parsed_message[j] = input_string[i];
					j++;
				}
			}
		}
	}
	return parsed_message;
}

char *_cst_get_reject_message(int index, gboolean b_parsing, gboolean b_translated)
{
	char *message = NULL;
	char *markup_converted_message = NULL;
	char *return_str = NULL;
	char *parsed_message = NULL;
	switch (index) {
	case 0:
		message = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR);
		break;
	case 1:
		message = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR);
		break;
	case 2:
		message = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR);
		break;
	case 3:
		message = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR);
		break;
	case 4:
		message = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR);
		break;
	case 5:
		message = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR);
		break;
	default:
		return NULL;
	}
	retv_if(NULL == message, NULL);

	markup_converted_message = elm_entry_utf8_to_markup(message);

	if (b_parsing == EINA_TRUE) {
		parsed_message = __cst_parse_input_string(markup_converted_message);
		if (!b_translated && parsed_message && strstr(parsed_message, "IDS_CST"))
			return_str = strdup(parsed_message);
		else
			return_str = strdup(dgettext(UGNAME, parsed_message));
		free(parsed_message);
		parsed_message = NULL;
	} else {
		if (!b_translated && markup_converted_message && strstr(markup_converted_message, "IDS_CST"))
			return_str = strdup(markup_converted_message);
		else
			return_str = strdup(dgettext(UGNAME, markup_converted_message));
	}
	free(message);
	message = NULL;
	free(markup_converted_message);
	markup_converted_message = NULL;

	return return_str;
}

void _cst_set_reject_message_util(int pos, char *message_new)
{
	int num_of_reject_message;
	int index = 0;
	char *message = NULL;
	char *message_next = NULL;
	if (pos == 0) {
		_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR, message_new);
	} else {
		num_of_reject_message = _cst_get_num_of_reject_message();
		DBG("NO of reject messsage is = %d", num_of_reject_message);
		while (num_of_reject_message >= 1) {
			switch (index) {
				case 0:
					message = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR);
					message_next = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR);
					_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR, message);
					break;
				case 1:
					message = message_next;
					message_next = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR);
					_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR, message);
					break;
				case 2:
					message = message_next;
					message_next = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR);
					_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR, message);
					break;
				case 3:
					message = message_next;
					message_next = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR);
					_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR, message);
					break;
				case 4:
					message = message_next;
					message_next = _cst_vconf_get_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR);
					_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR, message);
					break;
				default:
					break;
				}
			index++;
			num_of_reject_message--;
		}
		_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR, message_new);
	}
}

void _cst_set_reject_message(int index, char *message)
{
	switch (index) {
	case 0:
		_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR, message);
		break;
	case 1:
		_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR, message);
		break;
	case 2:
		_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR, message);
		break;
	case 3:
		_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR, message);
		break;
	case 4:
		_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR, message);
		break;
	case 5:
		_cst_vconf_set_str(VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR, message);
		break;
	default:
		DBG("Error set reject message");
		break;
	}
}

static int _cst_set_reject_msg_button_status(int num)
{
	ENTER(_cst_set_reject_msg_button_status);

	if (ec_item[0] != NULL) {
		DBG("ec_item[0] = 0x%x", ec_item[0]);
		if (num == 6) {
			elm_object_disabled_set(ec_item[0], EINA_TRUE);
		} else {
			elm_object_disabled_set(ec_item[0], EINA_FALSE);
		}
	}
	if (ec_item[1] != NULL) {
		DBG("ec_item[1] = 0x%x", ec_item[1]);
		if (num == 0) {
			elm_object_disabled_set(ec_item[1], EINA_TRUE);
		} else {
			elm_object_disabled_set(ec_item[1], EINA_FALSE);
		}
	}
	if (call_btn != NULL) {
		if (num >= 6) {
			elm_object_disabled_set(call_btn, EINA_TRUE);
		} else {
			elm_object_disabled_set(call_btn, EINA_FALSE);
		}
	}
	return num;
}

int _cst_set_num_of_reject_message(int num)
{
	ENTER(_cst_set_num_of_reject_message);

	_cst_vconf_set_int(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT, num);

	return num;
}

int _cst_get_num_of_reject_message(void)
{
	int cnt;
	Eina_Bool result = _cst_vconf_get_int(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT, &cnt);
	if (result == EINA_FALSE) {
		ERR("_cst_vconf_get_int(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT) failed!!!");
		return 0;
	}

	return cnt;
}

static Eina_Bool __cst_rejectcall_message_check_same_data(void *data, const char *string)
{
	ENTER(__cst_rejectcall_message_check_same_data);
	char *buf;
	int i, reject_message_count;
	Eina_Bool value = EINA_TRUE;
	DBG("Input string : %s", string);
	Eina_Bool result = _cst_vconf_get_int(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT, &reject_message_count);
	if (result == EINA_FALSE) {
		ERR("_cst_vconf_get_int(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT) failed!!!");
		return EINA_TRUE;
	}

	for (i = 0; i < reject_message_count; i++) {
		buf = _cst_get_reject_message(i, EINA_FALSE, EINA_TRUE);
		if (buf) {
			if (strcmp(buf, string) == 0) {
				_cst_create_toast_popup(T_(CST_STR_ALREADY_IN_USE));
				value = EINA_FALSE;
			}
			free(buf);
			buf = NULL;
		}
	}

	return value;
}

static char *__cst_gl_label_get_reject_msg(void *data, Evas_Object *obj, const char *part)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	retv_if(item_data == NULL, NULL);

	if (!strcmp(part, "elm.text.multiline")) {
		char *buf = strdup(dgettext(UGNAME, (const char *)item_data->label));
		return buf;
	}

	return NULL;
}

static void __cst_gl_del_reject_msg(void *data, Evas_Object *obj)
{
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	if (item_data) {
		if (item_data->label) {
			free(item_data->label);
		}
		free(item_data);
	}

	return;
}

static void __cst_set_genlist_item_styles_reject_msg(void)
{
	if (!itc_1text) {
		itc_1text = _cst_create_genlist_item_class("multiline",
				__cst_gl_label_get_reject_msg,
				NULL, NULL, __cst_gl_del_reject_msg);
	}
}

void _cst_genlist_append_reject_list_item(CstUgData_t *ugd, Evas_Object *genlist)
{
	ENTER(_cst_genlist_append_reject_list_item);
	int num_of_message;
	CstGlItemData_t *item_data;
	int index = 0;
	int title = CST_STR_REJECT_MESSAGES;

	num_of_message = _cst_get_num_of_reject_message();

	if (header) {
		snprintf(header, sizeof(char)*256, "%s (%d/6)", T_(title), num_of_message);
		elm_object_item_part_text_set(g_navi_it, "elm.text.title", header);
	}
	if (num_of_message == 0) {
		edje_object_signal_emit(_EDJ(ugd->backup_layout), "show,content2", "code");
	} else {
		for (index = 0; index < num_of_message; index++) {
			item_data = calloc(1, sizeof(CstGlItemData_t));
			ret_if(item_data == NULL);
			item_data->index = index;
			item_data->ugd = (void *)ugd;
			item_data->label = _cst_get_reject_message(index, EINA_TRUE, EINA_FALSE);

			item_data->gl_item = elm_genlist_item_append(genlist, itc_1text,
										   item_data, NULL, ELM_GENLIST_ITEM_NONE,
										   __cst_edit_reject_message_list, item_data);

			switch (index) {
				case 0:
					vconf_notify_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR,
							__cst_reject_sms_changed_cb, item_data);
					break;
				case 1:
					vconf_notify_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR,
							__cst_reject_sms_changed_cb, item_data);
					break;
				case 2:
					vconf_notify_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR,
							__cst_reject_sms_changed_cb, item_data);
					break;
				case 3:
					vconf_notify_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR,
							__cst_reject_sms_changed_cb, item_data);
					break;
				case 4:
					vconf_notify_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR,
							__cst_reject_sms_changed_cb, item_data);
					break;
				case 5:
					vconf_notify_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR,
							__cst_reject_sms_changed_cb, item_data);
					break;
				default:
					DBG("Error set reject message");
					break;
				}
		}
		edje_object_signal_emit(_EDJ(ugd->backup_layout), "show,content1", "code");
	}
}

static Evas_Object *__cst_create_genlist_reject_msg(CstUgData_t *ugd)
{
	ENTER(__cst_create_genlist_reject_msg);
	Evas_Object *genlist;

	genlist = elm_genlist_add(ugd->nf);

	elm_genlist_tree_effect_enabled_set(genlist, EINA_TRUE);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	vconf_notify_key_changed(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT,
			__cst_reject_sms_count_changed_cb, ugd);
	_cst_genlist_append_reject_list_item(ugd, genlist);
	evas_object_smart_callback_add(genlist, "realized",
			_cst_gl_realized_cb, NULL);
	return genlist;
}

static Eina_Bool __cst_click_reject_msg_edit_back(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_click_reject_msg_edit_back);
	retv_if(!data, EINA_TRUE);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	__cst_reject_msg_back_to_prev(ugd);

	return EINA_TRUE;
}

static Eina_Bool __cst_click_reject_msg_create_back(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_click_reject_msg_create_back);
	retv_if(!data, EINA_TRUE);
	CstUgData_t *ugd = (CstUgData_t *)data;
	__cst_reject_msg_back_to_prev(ugd);

	return EINA_TRUE;
}

static Eina_Bool __cst_check_empty(char *msg)
{
	int i = 0;
	if (!msg)
		return EINA_TRUE;
	else if (strlen(msg) == 0)
		return EINA_TRUE;
	else {
		for (i = 0; i < strlen(msg); i++) {
			if (msg[i] != ' ') {
				return EINA_FALSE;
			}
		}
	}

	return EINA_TRUE;
}

static void __cst_click_reject_msg_edit_done(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_click_reject_msg_edit_done);
	ret_if(!data);

	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	ret_if(!ugd);

	char *parsed_message = __cst_parse_input_string((char *)elm_entry_entry_get(ugd->dg_entry));
	char *new_message = elm_entry_markup_to_utf8(elm_entry_entry_get(ugd->dg_entry));
	char *reject_message = _cst_get_reject_message(item_data->index, EINA_FALSE, EINA_TRUE);

	DBG("item_data->label = %s", item_data->label);

	if (NULL == new_message) {
		DBG("No new message");
	} else if (__cst_check_empty(parsed_message)) {
		DBG("No Message");
		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}
		_cst_create_error_popup(CST_ERROR_ENTER_MESSAGE);
	} else if ((reject_message && !strcmp(reject_message, new_message))) {
		DBG("No change");
		__cst_reject_msg_back_to_prev(ugd);
	} else if ((__cst_rejectcall_message_check_same_data((void *)ugd, new_message)) == EINA_FALSE) {
		DBG("Duplicate message");
	} else {
		free(item_data->label);
		item_data->label = _cst_get_reject_message(item_data->index, EINA_TRUE, EINA_FALSE);
		_cst_set_reject_message(item_data->index, (char *)new_message);
		elm_genlist_item_update(item_data->gl_item);

		__cst_reject_msg_back_to_prev(ugd);
	}
	free(parsed_message);
	free(new_message);
	free(reject_message);
}

static void __cst_click_reject_msg_edit_cancel(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_click_reject_msg_edit_cancel);
	__cst_click_reject_msg_edit_back(data, NULL);
}

static void __cst_click_reject_msg_create_done(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_click_reject_msg_create_done);
	ret_if(!data);

	CstUgData_t *ugd = (CstUgData_t *)data;
	int num_of_reject_message;
	char *parsed_message = NULL;
	char *new_message = NULL;

	num_of_reject_message = _cst_get_num_of_reject_message();

	parsed_message = __cst_parse_input_string((char *)elm_entry_entry_get(ugd->dg_entry));

	new_message = elm_entry_markup_to_utf8(elm_entry_entry_get(ugd->dg_entry));

	if (NULL == new_message) {
		DBG("No new message");
	} else if (__cst_check_empty(parsed_message)) {
		DBG("No Message");
		if (ugd->popup) {
			evas_object_del(ugd->popup);
			ugd->popup = NULL;
		}
		_cst_create_error_popup(CST_ERROR_ENTER_MESSAGE);
	} else if ((__cst_rejectcall_message_check_same_data((void *)ugd, new_message)) == EINA_FALSE) {
		DBG("Duplicate message");
	} else {
		_cst_set_reject_message_util(num_of_reject_message, (char *)new_message);
		_cst_set_num_of_reject_message(num_of_reject_message + 1);
		__cst_reject_msg_back_to_prev(ugd);
	}
	free(parsed_message);
	parsed_message = NULL;
	free(new_message);
	new_message = NULL;
}

static void __cst_click_reject_msg_create_cancel(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_click_reject_msg_create_cancel);
	__cst_click_reject_msg_create_back(data, NULL);
}

static int __cst_reject_msg_exceed_limit_text_size(const char *text, CstUgData_t *ugd)
{
	retv_if(!ugd, REJECT_MSG_TEXT_UNDER_LIMIT_NUM);

	unsigned int text_size = 0;
	unsigned int segment_size = 0;
	int encoded_mode = 0;
	int ret = MSG_SUCCESS;

	ret = msg_util_calculate_text_length(text, MSG_ENCODE_AUTO, &text_size, &segment_size, (msg_encode_type_t *)&encoded_mode);
	DBG("encode_mode = %d, text_size = %d, segment_size = %d, encoded_mode = %d", MSG_ENCODE_AUTO, text_size, segment_size, encoded_mode);

	if (ret != MSG_SUCCESS) {
		ERR("msg_calculate_text_length is failed ret = %d", ret);
	}
	char buf[24] = { 0, };
	if (text_size > 0) {
		snprintf(buf, sizeof(buf), "%i/%i", text_size, segment_size);
	}
	elm_object_part_text_set(ugd->entry_count, "entry_count", buf);

	ugd->rej_msg_seg_size = segment_size;
	if (text_size > segment_size) {
		DBG("text_size = %d > segment_size = %d", text_size, segment_size);
		return REJECT_MSG_TEXT_OVER_LIMIT_NUM;
	} else if (text_size == segment_size && segment_size != 0) {
		DBG("text_size = %d == segment_size = %d", text_size, segment_size);
		return REJECT_MSG_TEXT_LIMIT_NUM;
	} else {
		return REJECT_MSG_TEXT_UNDER_LIMIT_NUM;
	}
}

void _cst_reject_msg_entry_filter_check_boundary(void *data, Evas_Object *entry, char **text)
{
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	unsigned int text_size = 0;
	unsigned int segment_size = 0;
	int encoded_mode = 0;
	int ret = MSG_SUCCESS;
	gchar *full_str = NULL;
	char *new_str = NULL;
	char *pre_str = NULL;

	ret_if(NULL == text);
	ret_if(NULL == *text);

	DBG("*text = %s", *text);

	if ((*text)[0] == '<' && (*text)[1] == 'p' && (*text)[2] == 'r') {
		new_str = elm_entry_markup_to_utf8(*text);
		if (new_str) {
			pre_str = elm_entry_markup_to_utf8(elm_entry_entry_get(entry));

			if (pre_str) {
				full_str = g_strconcat(pre_str, new_str, NULL);
			} else {
				full_str = g_strdup(new_str);
			}
		}
		DBG("pre_str = [%s], new_str = [%s], full_str = [%s]", pre_str, new_str, full_str);

		ret = msg_util_calculate_text_length(full_str, MSG_ENCODE_AUTO, &text_size, &segment_size, (msg_encode_type_t *)&encoded_mode);
		DBG("encode_mode = %d, text_size = %d, segment_size = %d, encoded_mode = %d", MSG_ENCODE_AUTO, text_size, segment_size, encoded_mode);

		if (ret != MSG_SUCCESS) {
			ERR("msg_util_calculate_text_length is failed ret = %d", ret);
		} else {
			if (text_size > segment_size) {
				DBG("text_size = %d > segment_size = %d", text_size, segment_size);
			}
		}
	} else {
		new_str = elm_entry_markup_to_utf8(*text);

		if (new_str) {
			pre_str = elm_entry_markup_to_utf8(elm_entry_entry_get(entry));

			if (pre_str) {
				full_str = g_strconcat(pre_str, new_str, NULL);
			} else {
				full_str = g_strdup(new_str);
			}
		}
		DBG("pre_str = [%s], new_str = [%s], full_str = [%s]", pre_str, new_str, full_str);

		ret = msg_util_calculate_text_length(full_str, MSG_ENCODE_AUTO, &text_size, &segment_size, (msg_encode_type_t *)&encoded_mode);
		DBG("encode_mode = %d, text_size = %d, segment_size = %d, encoded_mode = %d", MSG_ENCODE_AUTO, text_size, segment_size, encoded_mode);

		if (ret != MSG_SUCCESS) {
			ERR("msg_util_calculate_text_length is failed ret = %d", ret);
		} else {
			if (text_size > segment_size) {
				DBG("text_size = %d > segment_size = %d", text_size, segment_size);
				char *changed_new_str = NULL;
				int count = 0;
				do {
					if (changed_new_str) {
						g_free(changed_new_str);
						changed_new_str = NULL;
					}
					count++;
					text_size = 0;
					segment_size = 0;

					if (new_str) {
						changed_new_str = g_new0(char, strlen(new_str)+1);
					}

					if (changed_new_str) {
						g_utf8_strncpy(changed_new_str, new_str, g_utf8_strlen(new_str, -1) - count);
						DBG("changed_new_str = %s", changed_new_str);
						if (full_str) {
							g_free(full_str);
							full_str = NULL;
						}
						full_str = g_strconcat(pre_str, changed_new_str, NULL);
					} else {
						ERR("g_new0 failed");
					}

					ret = msg_util_calculate_text_length(full_str, MSG_ENCODE_AUTO, &text_size, &segment_size, (msg_encode_type_t *)&encoded_mode);
					if (ret != MSG_SUCCESS) {
						ERR("msg_util_calculate_text_length is failed ret = %d", ret);
						if (changed_new_str) {
							g_free(changed_new_str);
							changed_new_str = NULL;
						}
						break;
					}
				} while (text_size > segment_size);

				char *label = g_strdup_printf(T_(CST_STR_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED_PD), ugd->rej_msg_seg_size);
				_cst_create_toast_popup(label);
				g_free(label);

				if (*text) {
					g_free(*text);
					*text = NULL;
				}
				*text = elm_entry_utf8_to_markup(changed_new_str);
				if (changed_new_str) {
					g_free(changed_new_str);
					changed_new_str = NULL;
				}
			}
		}
	}

	if (full_str) {
		g_free(full_str);
		full_str = NULL;
	}
	if (new_str) {
		g_free(new_str);
		new_str = NULL;
	}
	if (pre_str) {
		g_free(pre_str);
		pre_str = NULL;
	}
}

void _cst_reject_msg_changed_editfield_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	const char *entry_str = elm_entry_entry_get(obj);
	char *text = NULL;
	DBG("entry_str = %s", entry_str);
	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ugd->nf);
	ret_if(!navi_it);
	Evas_Object *btn = elm_object_item_part_content_get(navi_it,
					   "title_right_btn");

	if (NULL == entry_str || '\0' == entry_str[0]) {
		if (btn != NULL) {
			elm_object_disabled_set(btn, EINA_TRUE);
		}
		if (save_btn != NULL) {
			elm_object_disabled_set(save_btn, EINA_TRUE);
		}
	}

	if (NULL != entry_str) {
		text = elm_entry_markup_to_utf8(entry_str);
		int ret = __cst_reject_msg_exceed_limit_text_size(text, ugd);
		switch (ret) {
		case REJECT_MSG_TEXT_OVER_LIMIT_NUM:
			if (btn != NULL) {
				elm_object_disabled_set(btn, EINA_TRUE);
			}
			if (save_btn != NULL) {
				elm_object_disabled_set(save_btn, EINA_TRUE);
			}
			char *label = g_strdup_printf(T_(CST_STR_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED_PD), ugd->rej_msg_seg_size);
			_cst_create_toast_popup(label);
			elm_entry_prediction_allow_set(obj, EINA_FALSE);

			g_free(label);
			break;
		case REJECT_MSG_TEXT_LIMIT_NUM:
			if (btn != NULL) {
				elm_object_disabled_set(btn, EINA_FALSE);
			}
			if (save_btn != NULL) {
				elm_object_disabled_set(save_btn, EINA_FALSE);
			}
			elm_entry_prediction_allow_set(obj, EINA_FALSE);
			break;
		case REJECT_MSG_TEXT_UNDER_LIMIT_NUM:
			if (btn != NULL) {
				elm_object_disabled_set(btn, EINA_FALSE);
			}
			if (save_btn != NULL) {
				elm_object_disabled_set(save_btn, EINA_FALSE);
			}
			elm_entry_prediction_allow_set(obj, EINA_TRUE);
			break;
		default:
			ERR("Invalid case");
		}
	}

	if (text) {
		free(text);
		text = NULL;
	}
}

static void __cst_edit_reject_message_list(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_edit_reject_message_list);
	ret_if(!data);
	CstGlItemData_t *item_data = (CstGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	Elm_Object_Item *navi_it;
	Evas_Object *scroller;
	Evas_Object *layout;
	const char *entry_input = NULL;

	g_item_data = item_data;

	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);
	ugd->popup = NULL;

	scroller = elm_scroller_add(ugd->nf);
	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	layout = _cst_create_reject_message_ime(scroller, _cst_get_reject_message(item_data->index, EINA_FALSE, EINA_TRUE), ugd);
	elm_object_content_set(scroller, layout);

	navi_it = elm_naviframe_item_push(ugd->nf,
									  I_(CST_STR_BODY_EDIT), NULL, NULL, scroller, NULL);
	if (navi_it == NULL) {
		ERR("elm_naviframe_item_push fail...");
		return;
	}
	cst_util_item_domain_text_translatable_set(navi_it, I_(CST_STR_BODY_EDIT));

	_cst_create_navi_control_bar(ugd->nf,
										(char *)I_(CST_STR_NAVI_BTN_CANCEL), NULL,
										__cst_click_reject_msg_edit_cancel,
										(char *)I_(CST_STR_NAVI_BTN_DONE), NULL,
										__cst_click_reject_msg_edit_done,
										(void *)g_item_data, navi_it,
										NULL);
	save_btn = elm_object_item_part_content_get(navi_it, "title_right_btn");

	elm_naviframe_item_pop_cb_set(navi_it, __cst_click_reject_msg_edit_back, (void *)g_item_data);

	entry_input = elm_entry_entry_get(ugd->dg_entry);

	if (save_btn != NULL) {
		if (entry_input && strlen(entry_input) > 0) {
			elm_object_disabled_set(save_btn, EINA_FALSE);
		} else {
			elm_object_disabled_set(save_btn, EINA_TRUE);
		}
	}
	evas_object_smart_callback_add(ugd->nf, "transition,finished", _cst_transition_cb, navi_it);
}

static void __cst_on_click_reject_message_add_button(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_on_click_reject_message_add_button);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Elm_Object_Item *navi_it;
	Evas_Object *scroller;
	Evas_Object *layout;
	const char *entry_input = NULL;

	ugd->popup = NULL;
	if (ugd->rejct_popup) {
		evas_object_del(ugd->rejct_popup);
		ugd->rejct_popup = NULL;
	}
	scroller = elm_scroller_add(ugd->nf);
	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	layout = _cst_create_reject_message_ime(scroller, NULL, ugd);
	elm_object_content_set(scroller, layout);

	navi_it = elm_naviframe_item_push(ugd->nf,
									  I_(CST_STR_SK3_CREATE), NULL, NULL, scroller, NULL);
	if (navi_it == NULL) {
		DBG("elm_naviframe_item_push fail...");
	}
	cst_util_item_domain_text_translatable_set(navi_it, I_(CST_STR_SK3_CREATE));

	_cst_create_navi_control_bar(ugd->nf,
										(char *)I_(CST_STR_NAVI_BTN_CANCEL), NULL,
										__cst_click_reject_msg_create_cancel,
										(char *)I_(CST_STR_NAVI_BTN_DONE), NULL,
										__cst_click_reject_msg_create_done,
										(void *)ugd, navi_it,
										NULL);
	save_btn = elm_object_item_part_content_get(navi_it, "title_right_btn");

	elm_naviframe_item_pop_cb_set(navi_it, __cst_click_reject_msg_create_back, (void *)ugd);

	entry_input = elm_entry_entry_get(ugd->dg_entry);

	if (save_btn != NULL) {
		if (entry_input && strlen(entry_input) > 0) {
			elm_object_disabled_set(save_btn, EINA_FALSE);
		} else {
			elm_object_disabled_set(save_btn, EINA_TRUE);
		}
	}
	evas_object_smart_callback_add(ugd->nf, "transition,finished", _cst_transition_cb, navi_it);
}

static Evas_Object *__cst_create_nocontents_layout(CstUgData_t *ugd)
{
	ENTER(__cst_create_nocontents_layout);

	Evas_Object *layout;
	layout = elm_layout_add(ugd->nf);
	if (!layout) {
		return NULL;
	}
	elm_layout_theme_set(layout, "layout", "nocontents", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	cst_util_domain_translatable_part_text_set(layout, "elm.text", I_(CST_STR_NO_REJECTION_MESSAGE));
	cst_util_domain_translatable_part_text_set(layout, "elm.help.text", I_(CST_STR_MESSAGE_WILL_BE_SHOWN));
	elm_layout_signal_emit(layout, "text,disabled", "");
	elm_layout_signal_emit(layout, "align.center", "elm");

	return layout;
}

void _cst_reject_msg_create_navi_control_bar(CstUgData_t *ugd)
{
	ENTER(_cst_reject_msg_create_navi_control_bar);

	memset(ec_item, 0, sizeof(ec_item));

	_cst_create_navi_control_bar(ugd->nf,
										(char *)I_(CST_STR_HEADER_DELETE), NULL,
										_cst_create_delete_list,
										(char *)I_(CST_STR_SK3_CREATE), NULL,
										__cst_on_click_reject_message_add_button,
										(void *)ugd, g_navi_it,
										ec_item);

	_cst_set_reject_msg_button_status(_cst_get_num_of_reject_message());
}

static void __cst_reject_msg_lang_changed(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_reject_msg_lang_changed);
	ret_if(NULL == data);
	int title = CST_STR_REJECT_MESSAGES;

	memset(header, 0x0, 256);

	snprintf(header, sizeof(char)*256, "%s (%d/6)", T_(title), _cst_get_num_of_reject_message());

	DBG("header = %s", header);
	if (g_navi_it) {
		elm_object_item_part_text_set(g_navi_it, "elm.text.title", header);
	}
}

static void __cst_Reject_msg_ctxpopup_rotation_changed(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_Reject_msg_ctxpopup_rotation_changed);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Coord x, y, w , h;
	int rotation_angle = 0;

	if (ugd->rejct_popup) {
		elm_win_screen_size_get(ugd->win_main, &x, &y, &w, &h);
		rotation_angle = elm_win_rotation_get((Evas_Object *)ugd->win_main);
		switch (rotation_angle) {
			case 0:
			case 180:
				evas_object_move(ugd->rejct_popup, 0, h);
				break;
			case 90:
				evas_object_move(ugd->rejct_popup, 0, w);
				break;
			case 270:
				evas_object_move(ugd->rejct_popup, h, w);
				break;
			default:
				evas_object_move(ugd->rejct_popup, 0, h);
				break;
		}
		evas_object_show(ugd->rejct_popup);
	}
}

static void __cst_Reject_msg_dismiss_popup_more_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_Reject_msg_dismiss_popup_more_cb);
	ret_if(NULL == data);

	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->rejct_popup) {
		evas_object_smart_callback_del(ugd->win_main, "rotation,changed",
				__cst_Reject_msg_ctxpopup_rotation_changed);

		evas_object_event_callback_del_full(ugd->rejct_popup, EVAS_CALLBACK_DEL,
				__cst_Reject_msg_dismiss_popup_more_cb, data);

		evas_object_del(ugd->rejct_popup);
		ugd->rejct_popup = NULL;
	}
}

static void __cst_Reject_msg_dismiss_popup_more_cb_wrapper(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	__cst_Reject_msg_dismiss_popup_more_cb(data, (Evas *)EVAS_CALLBACK_DEL, NULL, NULL);
	return;
}

static void more_btn_cb(void *data, Evas_Object* obj, void* event_info)
{
	ENTER(more_btn_cb);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Coord x, y, w , h;
	int rotation_angle = 0;
	int num = 0;
	if (ugd->rejct_popup) {
		evas_object_del(ugd->rejct_popup);
		ugd->rejct_popup = NULL;
	}

	ugd->rejct_popup = elm_ctxpopup_add(ugd->win_main);
	elm_object_style_set(ugd->rejct_popup, "more/default");
	evas_object_smart_callback_add(ugd->rejct_popup, "dismissed",
			NULL, NULL);
	eext_object_event_callback_add(ugd->rejct_popup, EEXT_CALLBACK_BACK, __cst_Reject_msg_dismiss_popup_more_cb_wrapper, data);
	eext_object_event_callback_add(ugd->rejct_popup,  EEXT_CALLBACK_MORE, __cst_Reject_msg_dismiss_popup_more_cb_wrapper, data);
	elm_ctxpopup_auto_hide_disabled_set(ugd->rejct_popup, EINA_TRUE);

	evas_object_event_callback_add(ugd->rejct_popup, EVAS_CALLBACK_DEL,
			__cst_Reject_msg_dismiss_popup_more_cb, data);

	num  = _cst_get_num_of_reject_message();

	if (num < 6) { /*If number is 6, then max number of Reject msgs set hence dont show 'Create' option*/
		elm_ctxpopup_item_append(ugd->rejct_popup, T_(CST_STR_SK3_CREATE), NULL, __cst_on_click_reject_message_add_button, ugd);
	}

	if (num > 0) { /*If number is 0, then no Reject msgs set hence dont show 'Delete' option*/
		elm_ctxpopup_item_append(ugd->rejct_popup, T_(CST_STR_HEADER_DELETE), NULL, _cst_create_delete_list, ugd);
	}

	elm_ctxpopup_direction_priority_set(ugd->rejct_popup, ELM_CTXPOPUP_DIRECTION_UP,
						ELM_CTXPOPUP_DIRECTION_UNKNOWN,
						ELM_CTXPOPUP_DIRECTION_UNKNOWN,
						ELM_CTXPOPUP_DIRECTION_UNKNOWN);
	elm_win_screen_size_get(ugd->win_main, &x, &y, &w, &h);
	evas_object_smart_callback_add(ugd->win_main, "rotation,changed",
			__cst_Reject_msg_ctxpopup_rotation_changed, data);
	rotation_angle = elm_win_rotation_get((Evas_Object *)ugd->win_main);
	switch (rotation_angle) {
		case 0:
		case 180:
			evas_object_move(ugd->rejct_popup, (w / 2), h);
			break;
		case 90:
			evas_object_move(ugd->rejct_popup,	(h / 2), w);
			break;
		case 270:
			evas_object_move(ugd->rejct_popup, (h / 2), w);
			break;
		default:
			evas_object_move(ugd->rejct_popup, (w / 2), h);
			break;
	}
	evas_object_show(ugd->rejct_popup);
	return;
}

static void __cst_create_reject_message(Evas_Object *parent, void *data)
{
	ENTER(_cst_on_click_reject_message);
	Evas_Object *layout, *nocontent;
	CstUgData_t *ugd = data;
	static Evas_Object *more_btn = NULL;
	Evas_Object *back_btn = NULL;
	ret_if(ugd == NULL);

	int title = CST_STR_REJECT_MESSAGES;

	__cst_set_genlist_item_styles_reject_msg();
	ugd->kind_of_delete_list = CST_DL_REJECT_CALL_WITH_MSG;

	/*Create layout */
	layout = elm_layout_add(ugd->nf);
	ugd->backup_layout = layout;
	elm_layout_file_set(layout, EDJ_NAME, "toolbar");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	nocontent = __cst_create_nocontents_layout(ugd);
	g_genlist = ugd->backup_genlist = __cst_create_genlist_reject_msg(ugd);
	evas_object_smart_callback_add(g_genlist, "language,changed", __cst_reject_msg_lang_changed, data);

	elm_object_part_content_set(layout, "elm.swallow.contents", ugd->backup_genlist);
	elm_object_part_content_set(layout, "elm.swallow.contents2", nocontent);

	/*Create naviframe back button*/
	back_btn = _cst_util_navi_back_btn_create(ugd->nf);

	header = (char *)malloc(sizeof(char)*256);
	memset(header, 0x0, 256);

	snprintf(header, sizeof(char)*256, "%s (%d/6)", T_(title), _cst_get_num_of_reject_message());

	DBG("header = %s", header);
	g_navi_it = elm_naviframe_item_push(ugd->nf, header, back_btn, NULL, layout, NULL);
	elm_naviframe_item_pop_cb_set(g_navi_it, _back_btn_clicked_reject_msg_cb, (void *)ugd);

	/*create more button*/
	more_btn = elm_button_add(ugd->nf);
	elm_object_style_set(more_btn, "naviframe/more/default");
	evas_object_smart_callback_add(more_btn, "clicked", more_btn_cb, ugd);
	elm_object_item_part_content_set(g_navi_it, "toolbar_more_btn", more_btn);
}

void _cst_on_click_reject_message(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	ENTER(_cst_on_click_reject_message);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	__cst_create_reject_message(ugd->nf, ugd);
}

void _cst_reject_msg_input_panel_event_callback(void *data, Ecore_IMF_Context *imf_context, int value)
{
	ENTER(_cst_reject_msg_input_panel_event_callback);

	ret_if(NULL == data);

	if (value == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_SHOW");
	} else if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_HIDE");
	}
}

static Eina_Bool _cst_reject_msg_naviframe_item_pop_cb(void *data)
{
	CstUgData_t *ugd = (CstUgData_t *)data;

	evas_object_smart_callback_del(ugd->nf, "transition,finished", _cst_transition_cb);

	ugd->dg_entry = NULL;

	elm_naviframe_item_pop_cb_set(elm_naviframe_top_item_get(ugd->nf), NULL, NULL);

	elm_naviframe_item_pop(ugd->nf);

	save_btn = NULL;

	return ECORE_CALLBACK_CANCEL;
}

static void __cst_reject_msg_back_to_prev(CstUgData_t *ugd)
{
	ret_if(!ugd);
	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ugd->dg_entry);

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, _cst_reject_msg_input_panel_event_callback);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}

	evas_object_smart_callback_del(ugd->dg_entry, "changed", _cst_reject_msg_changed_editfield_cb);
	evas_object_smart_callback_del(ugd->dg_entry, "preedit,changed", _cst_reject_msg_changed_editfield_cb);

/*	ecore_timer_add(CST_NAVIFRAME_ITEM_POP_TIMER, _cst_reject_msg_naviframe_item_pop_cb, ugd);
*/
	_cst_reject_msg_naviframe_item_pop_cb(ugd);
}

void _cst_destroy_reject_message(CstUgData_t *ugd)
{
	ENTER(_cst_destroy_reject_message);
	ret_if(!ugd);

	vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR,
			__cst_reject_sms_changed_cb);
	vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR,
			__cst_reject_sms_changed_cb);
	vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR,
			__cst_reject_sms_changed_cb);
	vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR,
			__cst_reject_sms_changed_cb);
	vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR,
			__cst_reject_sms_changed_cb);
	vconf_ignore_key_changed(VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR,
			__cst_reject_sms_changed_cb);
	vconf_ignore_key_changed(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT,
			__cst_reject_sms_count_changed_cb);
	__cst_destroy_genlist_item_styles();

	if (ugd->rejct_popup) {
		evas_object_del(ugd->rejct_popup);
		ugd->rejct_popup = NULL;
	}

	if (header) {
		free(header);
		header = NULL;
	}

	if (g_genlist) {
		evas_object_smart_callback_del(g_genlist, "language,changed", __cst_reject_msg_lang_changed);
		g_genlist = NULL;
	}

	if (ugd->backup_genlist) {
		ugd->backup_genlist = NULL;
	}

	if (ugd->backup_layout) {
		ugd->backup_layout = NULL;
	}
}

