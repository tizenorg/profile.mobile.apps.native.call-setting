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

#ifdef _CALL_SET_FDN_SUPPORT

#include <Elementary.h>
#include <ui-gadget-module.h>
#include <ITapiSim.h>

#include "cst-common-string.h"
#include "cst-common.h"
#include "cst-call-setting.h"
#include "cst-util.h"
#include "cst-fdn-list.h"
#include "cst-fdn-setting.h"
#include "cst-fdn-contact.h"
#include "cst-widget.h"
#include "cst-tapi-request.h"

#define CST_ADD_FDN_CONTACT_STR		"Add fixed dialling contact"
#define CST_EDIT_FDN_CONTACT_STR	"Edit fixed dialling contact"
#define CST_SAVE_STR				"Save"
#define CST_CANCEL_STR				"Cancel"
#define CST_IMPORT_STR				"Import"
#define CST_DELETE_STR				"Delete"
#define CST_NAME_STR				"Name"
#define CST_NUMBER_STR				"Number"
#define CST_PIN2_STR				"PIN2"
#define DELETE_CONTACT_STR			"Delete contact"
#define ENTER_PIN2					"Enter PIN2"
#define FDN_DELETED_SUCESS_STR	"Fixed dialling contact deleted."

#define NAVI_ITEM_KEY			"_cst_fdn_contact_navi_item_key"

#define NUMBER_IS_EMPTY	"No contact number entered"

typedef enum {
	CST_EDITFIELD_NAME = 0,
	CST_EDITFIELD_NUMBER = 1,
	CST_EDITFIELD_PIN2 = 2,
	CST_EDITFIELD_MAX = 3,
} __cst_editfield_type_id_t;

typedef struct {
	__cst_editfield_type_id_t id;
	char *guide_txt;
	char *entry_txt;
} __cst_editfield_data_t;

typedef struct item_data {
	int index;
	CstUgData_t *ugd;
} item_data_s;

static unsigned short g_rec_index;
static unsigned short g_next_rec_index;

static unsigned char *input_name, *input_number;

void _cst_fdn_contact_entry_focus_set(Evas_Object *entry, CstUgData_t *ugd)
{
	ENTER(_cst_fdn_contact_entry_focus_set);
	ret_if(ugd == NULL);
	ret_if(entry == NULL);

	if (entry != ugd->dg_entry_pin2) {
		ecore_imf_context_input_panel_enabled_set(elm_entry_imf_context_get(ugd->dg_entry_pin2), EINA_TRUE);
	}
	ecore_imf_context_input_panel_enabled_set(elm_entry_imf_context_get(entry), EINA_TRUE);
	ecore_imf_context_input_panel_show(elm_entry_imf_context_get(entry));
	elm_object_focus_set(entry, EINA_TRUE);
	elm_entry_cursor_end_set(entry);
}

static void __cst_fdn_contact_tapi_sim_response_update_cb(CstUgData_t *ugd, void *req_data, void *resp_data)
{
	ENTER(__cst_fdn_contact_tapi_sim_response_update_cb);
	ret_if(ugd == NULL);
	ret_if(req_data == NULL);
	ret_if(resp_data == NULL);

	CallSettingSimReq_t *req = (CallSettingSimReq_t *)req_data;
	CallSettingSimResp_t *resp = (CallSettingSimResp_t *)resp_data;

	DBG("sim response received. Req id = [%d]", req->req_id);
	switch (req->req_id) {
	case TAPI_SIM_REQ_VERIFY_PIN2:
		DBG("TAPI_SIM_REQ_VERIFY_PIN2 received");
		if (req->next_req_params) {
			if (TAPI_SIM_REQ_UPDATE_PHNBUK_REC == req->next_req_params->req_id ||
					TAPI_SIM_REQ_ADD_PHNBUK_REC == req->next_req_params->req_id) {
				if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
					DBG("PIN2 verified success");
					ugd->sim_lock_retry_account = 3;
					CallSettingFDNRec_t *fdn_rec = (CallSettingFDNRec_t *)g_malloc0(sizeof(CallSettingFDNRec_t));
					ret_if(NULL == fdn_rec);
					fdn_rec->index = req->next_req_params->index;
					fdn_rec->next_index = req->next_req_params->next_index;
					fdn_rec->number = (unsigned char *)g_strdup((gchar *)req->next_req_params->number);
					fdn_rec->name = (unsigned char *)g_strdup((gchar *)req->next_req_params->name);
					SEC_DBG("Name = %s", fdn_rec->name);
					DBG("Number = %s", fdn_rec->number);
					DBG("Index = %d", fdn_rec->index);
					DBG("Next Index = %d", fdn_rec->next_index);
					if (TAPI_SIM_REQ_UPDATE_PHNBUK_REC == req->next_req_params->req_id) {
						_cst_update_phnbuk_record(ugd, FALSE, fdn_rec, __cst_fdn_contact_tapi_sim_response_update_cb);
					} else {
						_cst_update_phnbuk_record(ugd, TRUE, fdn_rec, __cst_fdn_contact_tapi_sim_response_update_cb);
					}
				} else if (resp->retry_count) {
					ugd->sim_lock_retry_account = resp->retry_count;
					/* Show PIN2 change error */
					const char *popupmsg = NULL;

					switch (resp->retry_count) {
					case 1:
						popupmsg = I_(CST_STR_INCORRECT_PIN2_1_ATTEMPT_LEFT);
						break;
					case 2:
						popupmsg = I_(CST_STR_INCORRECT_PIN2_2_ATTEMPTS_LEFT);
						break;
					default:
						popupmsg = I_(CST_STR_INCORRECT_PIN2);
						break;
					}

					cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, popupmsg);
					_cst_fdn_contact_entry_focus_set(ugd->dg_entry_pin2, ugd);
				} else {
					DBG("Max retries over !!");
					/* TODO: we need to inform fdn_settings to set PIN2 blocked */
					/* TODO: After the popup is closed we need to go back to prev screen */
					ugd->sim_lock_status = TAPI_SIM_LOCK_KEY_PUK2;
					ugd->sim_lock_retry_account = 10;
					_cst_fdn_disable_main_screen_show_unlock_pin2_popup(
							(void *)ugd);
				}
			} else if (TAPI_SIM_REQ_DELETE_PHNBUK_REC == req->next_req_params->req_id) {
				if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
					DBG("PIN2 verified success");
					ugd->sim_lock_retry_account = 3;
					_cst_delete_phnbuk_record(ugd, g_rec_index, __cst_fdn_contact_tapi_sim_response_update_cb, ugd);
				} else if (resp->retry_count) {
					ugd->sim_lock_retry_account = resp->retry_count;
					/* Show PIN2 change error */
					const char *popupmsg = NULL;

					switch (resp->retry_count) {
					case 1:
						popupmsg = I_(CST_STR_INCORRECT_PIN2_1_ATTEMPT_LEFT);
						break;
					case 2:
						popupmsg = I_(CST_STR_INCORRECT_PIN2_2_ATTEMPTS_LEFT);
						break;
					default:
						popupmsg = I_(CST_STR_INCORRECT_PIN2);
						break;
					}

					cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, popupmsg);
					/* TODO: After the popup is closed we need to go back to prev screen */
				} else {
					DBG("Max retries over !!");
					ugd->sim_lock_status = TAPI_SIM_LOCK_KEY_PUK2;
					ugd->sim_lock_retry_account = 10;
					_cst_fdn_disable_main_screen_show_unlock_pin2_popup(
							(void *)ugd);
				}
			} else {
				ERR("Unknown request id: %d", req->next_req_params->req_id);
			}
		}
		break;

	case TAPI_SIM_REQ_UPDATE_PHNBUK_REC:
	case TAPI_SIM_REQ_ADD_PHNBUK_REC:
		DBG("TAPI_SIM_REQ_UPDATE_PHNBUK_REC or TAPI_SIM_REQ_ADD_PHNBUK_REC received");
		ret_if(req->data == NULL);

		switch (resp->result) {
		case TAPI_SIM_OPERATION_SUCCESS:
			DBG("Phone book update success for index = %d", ((CallSettingFDNRec_t *)(req->data))->index);
			SEC_DBG("name = %s, number = %s", ((CallSettingFDNRec_t *)(req->data))->name, ((CallSettingFDNRec_t *)(req->data))->number);
			_cst_set_index_used(((CallSettingFDNRec_t *)(req->data))->index, 1);
			_cst_fdn_list_initialize_genlist(ugd);
			if (TAPI_SIM_REQ_UPDATE_PHNBUK_REC == req->req_id) {
				_cst_update_fdn_list(ugd, (CallSettingFDNRec_t *)(req->data));
			} else {
				_cst_add_new_fdn_contact_to_list(ugd, (CallSettingFDNRec_t *)(req->data), EINA_TRUE);
			}
			_cst_fdn_list_back_to_prev(ugd);
			break;
		case TAPI_SIM_OPERATION_FAILURE:
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, FDN_SIM_PB_FAIL_STR);
			_cst_fdn_contact_entry_focus_set(ugd->dg_entry_pin2, ugd);
			break;
		case TAPI_SIM_OPERATION_INVALID_INDEX:
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, FDN_SIM_PB_INVALID_INDEX_STR);
			_cst_fdn_contact_entry_focus_set(ugd->dg_entry_pin2, ugd);
			break;
		case TAPI_SIM_OPERATION_INVALID_NUMBER_LENGTH:
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, FDN_SIM_PB_INVALID_NUMBER_LENGTH_STR);
			_cst_fdn_contact_entry_focus_set(ugd->dg_entry_contact_number, ugd);
			break;
		case TAPI_SIM_OPERATION_INVALID_NAME_LENGTH:
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, FDN_SIM_PB_INVALID_NAME_STR);
			_cst_fdn_contact_entry_focus_set(ugd->dg_entry_contact_name, ugd);
			break;
		case TAPI_SIM_OPERATION_ACCESS_CONDITION_NOT_SATISFIED:
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, FDN_SIM_PB_ACCESS_CONDITION_NOT_SATISFIED_STR);
			_cst_fdn_contact_entry_focus_set(ugd->dg_entry_pin2, ugd);
			break;
		default:
			cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, FDN_SIM_PB_FAIL_STR);
			_cst_fdn_contact_entry_focus_set(ugd->dg_entry_pin2, ugd);
			break;
		}
		break;

	default:
		ERR("Response received for an Invalid Request id[%d]", req->req_id);
		break;
	}

	LEAVE();
	return;
}

static Evas_Object *__cst_gl_icon_get_fdn_contact_name_ime(Evas_Object *parent,
		void *data)
{
	ENTER(__cst_gl_icon_get_fdn_contact_name_ime);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *editfield = NULL;

	editfield = _cst_create_ime_editfield(ugd, parent, CST_IME_FDN_CONTACT_NAME,
			(char *)input_name);
	retv_if(editfield == NULL, NULL);

	return editfield;
}

static Evas_Object *__cst_gl_icon_get_fdn_contact_number_ime(Evas_Object *parent, void *data)
{
	ENTER(__cst_gl_icon_get_fdn_contact_number_ime);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;

	Evas_Object *ef = _cst_create_ime_editfield(ugd, parent, CST_IME_FDN_CONTACT_NUMBER, (char *)input_number);
	retv_if(ef == NULL, NULL);

	return ef;
}

static Evas_Object *gl_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	item_data_s *id = (item_data_s *)data;
	retv_if(id == NULL, NULL);
	Evas_Object *content = NULL;
	CstUgData_t *ugd = (CstUgData_t *)id->ugd;
	retv_if(ugd == NULL, NULL);

	if (!strcmp(part, "elm.icon.entry")) {
		if (id->index == 0) {
			content = __cst_gl_icon_get_fdn_contact_name_ime(obj, ugd);
		} else {
			content = __cst_gl_icon_get_fdn_contact_number_ime(obj, ugd);
		}
	} else if (!strcmp(part, "elm.icon")) {
		/* Set the Contact icon part of the Editfield layout */
		content = _cst_create_ime_contacts_btn_obj(obj, (void *)ugd);
	}
	return content;
}

static void gl_del_cb(void *data, Evas_Object *obj)
{
	item_data_s *id = (item_data_s *)data;
	ret_if(id == NULL);
	free(id);
}

static Evas_Object *__cst_create_layout_add_fdn_contact(void *data)
{
	ENTER(__cst_create_layout_add_fdn_contact);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *genlist = NULL;
	item_data_s *id = NULL;
	int idx = -1;

	Elm_Genlist_Item_Class *itc_entry = elm_genlist_item_class_new();
	retv_if(NULL == itc_entry, NULL);

	itc_entry->item_style = "entry";
	itc_entry->func.text_get = NULL;;
	itc_entry->func.content_get = gl_content_get_cb;
	itc_entry->func.del = gl_del_cb;

	Elm_Genlist_Item_Class *itc_entry_icon = elm_genlist_item_class_new();
	if (NULL == itc_entry_icon) {
		ERR("Failed to allocate genlist item");
		elm_genlist_item_class_free(itc_entry);
		return NULL;
	}

	itc_entry_icon->item_style = "entry.icon";
	itc_entry_icon->func.text_get = NULL;
	itc_entry_icon->func.content_get = gl_content_get_cb;
	itc_entry_icon->func.del = gl_del_cb;

	/* Create genlist */
	genlist = elm_genlist_add(ugd->nf);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_select_mode_set(genlist, ELM_OBJECT_SELECT_MODE_NONE);

	for (idx = 0; idx < 2; idx++) {
		id = calloc(sizeof(item_data_s), 1);
		if (NULL == id) {
			ERR("Failed to allocate memory");
			elm_genlist_item_class_free(itc_entry);
			elm_genlist_item_class_free(itc_entry_icon);
			return NULL;
		}
		id->index = idx;
		id->ugd = ugd;

		if (idx == 0) {
			/* Name */
			elm_genlist_item_append(genlist, itc_entry_icon, (void *)id, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
		} else {
			/* Number */
			elm_genlist_item_append(genlist, itc_entry, (void *)id, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
		}
	}

	elm_genlist_item_class_free(itc_entry);
	elm_genlist_item_class_free(itc_entry_icon);

	return genlist;
}
#if 0
static Eina_Bool __cst_is_number_valid(char *number)
{
	ENTER(__cst_is_number_valid);
	Eina_Bool plus_recvd = EINA_FALSE;
	Eina_Bool number_recvd = EINA_FALSE;
	if (!number) {
		return EINA_FALSE;
	}

	while (*number) {
		if ('+' == *number) {
			if (EINA_TRUE == plus_recvd || EINA_TRUE == number_recvd)
				return EINA_FALSE;
			plus_recvd = EINA_TRUE;
		} else if (*number >= '0' && *number <= '9') {
			number_recvd = EINA_TRUE;
		} else {
			return EINA_FALSE;
		}
		number++;
	}

	return number_recvd;
}
#endif
void _cst_fdn_contact_edit_done(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	CstFDNGlItemData_t *item_data = (CstFDNGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	char *name = NULL, *number = NULL, *pin2 = NULL;
	int pin2_legth = 0;
	name = elm_entry_markup_to_utf8(elm_entry_entry_get(ugd->dg_entry_contact_name));
	SEC_DBG("Name = %s", name);

	number = elm_entry_markup_to_utf8(elm_entry_entry_get(ugd->dg_entry_contact_number));
	SEC_DBG("Number = %s", number);

	pin2 = elm_entry_markup_to_utf8(elm_entry_entry_get(ugd->dg_entry_pin2));
	SEC_DBG("PIN2 = %s", pin2);

	if (pin2) {
		pin2_legth = strlen(pin2);
	}

	if (ugd->dg_entry) {
		evas_object_del(ugd->dg_entry);
		ugd->dg_entry = NULL;
	}

	if (ugd->dg_entry_pin2) {
		evas_object_del(ugd->dg_entry_pin2);
		ugd->dg_entry_pin2 = NULL;
	}

	if (!number) {
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, NUMBER_IS_EMPTY);
	} else if (!strlen(number)) {
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, NUMBER_IS_EMPTY);
	} else if (4 > pin2_legth) {
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, INAVLID_PIN2_ERR_STR);
	} else {
		CallSettingSimNxtReqParams_t nxt_re_params = {0, };
		nxt_re_params.pin2 = (unsigned char *)pin2;
		nxt_re_params.pin2_len = pin2_legth;
		nxt_re_params.name = (unsigned char *)name;
		nxt_re_params.number = (unsigned char *)number;
		nxt_re_params.index = g_rec_index;
		nxt_re_params.next_index = g_next_rec_index;
		nxt_re_params.req_id = TAPI_SIM_REQ_UPDATE_PHNBUK_REC;
		_cst_verify_pin2(ugd, &nxt_re_params, __cst_fdn_contact_tapi_sim_response_update_cb, ugd);
		/*ecore_imf_context_input_panel_enabled_set(elm_entry_imf_context_get(ugd->dg_entry_pin2), EINA_FALSE);*/
	}

	if (name) {
		g_free(name);
		name = NULL;
	}
	if (number) {
		g_free(number);
		number = NULL;
	}
	if (pin2) {
		g_free(pin2);
		pin2 = NULL;
	}

	/*Destroy the Popup*/
	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	return;
}

void _cst_fdn_contact_create_done(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	char *name = NULL, *number = NULL, *pin2 = NULL;
	name = elm_entry_markup_to_utf8(elm_entry_entry_get(ugd->dg_entry_contact_name));
	SEC_DBG("Name = %s", name);

	number = elm_entry_markup_to_utf8(elm_entry_entry_get(ugd->dg_entry_contact_number));

	pin2 = elm_entry_markup_to_utf8(elm_entry_entry_get(ugd->dg_entry_pin2));

	if (ugd->dg_entry) {
		evas_object_del(ugd->dg_entry);
		ugd->dg_entry = NULL;
	}

	if (ugd->dg_entry_pin2) {
		evas_object_del(ugd->dg_entry_pin2);
		ugd->dg_entry_pin2 = NULL;
	}

	if (!number) {
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, NUMBER_IS_EMPTY);
		goto RESOURCE_RELEASE;
	}
	SEC_DBG("Number = %s", number);

	if (!pin2) {
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, INAVLID_PIN2_ERR_STR);
		goto RESOURCE_RELEASE;
	}
	SEC_DBG("PIN2 = %s", pin2);

	if (!strlen(number)) {
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, NUMBER_IS_EMPTY);
	} else if (4 > strlen(pin2)) {
		cst_FDN_setting_show_popup(CST_ERR_POPUP_TYPE_INFO_TEXT_ONLY, ugd, NULL, INAVLID_PIN2_ERR_STR);
	} else {
		CallSettingSimNxtReqParams_t nxt_re_params = {0, };
		nxt_re_params.pin2 = (unsigned char *)pin2;
		nxt_re_params.pin2_len = strlen(pin2);
		nxt_re_params.name = (unsigned char *)name;
		nxt_re_params.number = (unsigned char *)number;
		nxt_re_params.index = g_rec_index;
		nxt_re_params.next_index = g_next_rec_index;
		nxt_re_params.req_id = TAPI_SIM_REQ_ADD_PHNBUK_REC;
		_cst_verify_pin2(ugd, &nxt_re_params, __cst_fdn_contact_tapi_sim_response_update_cb, ugd);
		/*ecore_imf_context_input_panel_enabled_set(elm_entry_imf_context_get(ugd->dg_entry_pin2), EINA_FALSE);*/
	}

RESOURCE_RELEASE:
	if (name) {
		g_free(name);
		name = NULL;
	}
	if (number) {
		g_free(number);
		number = NULL;
	}
	if (pin2) {
		g_free(pin2);
		pin2 = NULL;
	}

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	return;
}

Evas_Object *_cst_create_fdn_contact_list(void *data, CstFDNContactOperation_t operation_type, CstFDNContactEditInfo_t *edit_info)
{
	ENTER(_cst_create_fdn_contact_list);
	retv_if(NULL == data, NULL);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Object *scroller = NULL;

	if (CST_CONTACT_ADD == operation_type) {
		input_name = NULL;
		input_number = NULL;
		scroller = __cst_create_layout_add_fdn_contact(ugd);
		g_rec_index = _cst_get_first_available_free_index();
		g_next_rec_index = g_rec_index + 1;
	} else if (CST_CONTACT_EDIT == operation_type) {
		if (!edit_info) {
			ERR("Fatal: Empty edit info !!");
			return NULL;
		}
		g_rec_index = edit_info->index;
		g_next_rec_index = edit_info->next_index;
		input_name = edit_info->name;
		input_number = edit_info->number;
		scroller = __cst_create_layout_add_fdn_contact(ugd);
	}
	LEAVE();
	return scroller;
}

#endif  /* _CALL_SET_FDN_SUPPORT */

