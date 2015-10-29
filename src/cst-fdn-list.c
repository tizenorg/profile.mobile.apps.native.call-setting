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
#include <ITapiPhonebook.h>
#include <ITapiSim.h>

#include "cst-common-string.h"
#include "cst-common.h"
#include "cst-call-setting.h"
#include "cst-delete-list.h"
#include "cst-fdn-contact.h"
#include "cst-fdn-setting.h"
#include "cst-fdn-list.h"
#include "cst-widget.h"
#include "cst-tapi-request.h"
#include "cst-util.h"

#define NO_CONTENT_LYT_DATA_KEY	"_cst_fdn_list_no_content_lyt"

static Elm_Genlist_Item_Class *itc_2text = NULL;
/*Evas_Object *fdn_list_genlist = NULL;*/
static int g_phnbuk_cnt = 0;
static int g_phnbuk_total_cnt = 0;
static char *g_phnbuk_available_free_index;

static gboolean genlist_initialized = EINA_FALSE;
static unsigned short first_index = 0;

static CstFDNGlItemData_t *g_item_data = NULL;

static Evas_Object *ec_item[3];

static gboolean keypad_enabled = EINA_FALSE;

static void __cst_set_genlist_item_styles_fdn_list(void);

#if 0 /* function not use */
static void __cst_create_fdn_navi_control_bar_btns(void *data);
#endif

static void __cst_add_to_fdn_phnbuk_cb(void *data, Evas_Object *obj, void *event_info);
static Eina_Bool __cst_click_fdn_list_contact_edit_cancel(void *data,
		Elm_Object_Item *it);
static Eina_Bool __cst_click_fdn_list_contact_create_cancel(void *data,
		Elm_Object_Item *it);

void _cst_fdn_list_set_button_status(int current, int max)
{
	if (ec_item[1] != NULL) {
		if (current == 0) {
			elm_object_disabled_set(ec_item[1], EINA_TRUE);
		} else {
			elm_object_disabled_set(ec_item[1], EINA_FALSE);
		}
	}
	if (ec_item[0] != NULL) {
		if (current == max) {
			elm_object_disabled_set(ec_item[0], EINA_TRUE);
		} else {
			elm_object_disabled_set(ec_item[0], EINA_FALSE);
		}
	}
}

static void __cst_gl_del_cb(void *data, Evas_Object *obj)
{
	CstFDNGlItemData_t *item_data = (CstFDNGlItemData_t *)data;
	ret_if(item_data == NULL);
	ENTER(__cst_gl_del_cb);

	if (item_data->phbk_info) {
		if (item_data->phbk_info->name) {
			g_free(item_data->phbk_info->name);
		}
		if (item_data->phbk_info->number) {
			g_free(item_data->phbk_info->number);
		}
		free(item_data->phbk_info);
	}
	free(item_data);
	return;
}

static char *__cst_gl_label_get_cb(void *data, Evas_Object *obj, const char *part)
{
	CstFDNGlItemData_t *item_data = (CstFDNGlItemData_t *)data;
	retv_if(NULL == data, NULL);
	DBG("part = %s", part);

	if (strcmp(part, "elm.text.main.left.top") == 0) {
		return g_strdup((const char *)item_data->phbk_info->name);
	} else if (strcmp(part, "elm.text.sub.left.bottom") == 0) {
		return g_strdup((const char *)item_data->phbk_info->number);
	}
	return NULL;
}

static void __cst_fdn_list_launch_voice_call(const char *number)
{
	ret_if(NULL == number);
	app_control_h service = NULL;
	char telnum[255] = { 0, };

	SEC_DBG("launch voice call for number = %s", number);

	snprintf(telnum, sizeof(telnum), "tel:%s", number);

	if (APP_CONTROL_ERROR_NONE != app_control_create(&service)) {
		ERR("app_control_create() is failed");
		return;
	} else if (APP_CONTROL_ERROR_NONE != app_control_set_operation(service, APP_CONTROL_OPERATION_CALL)) {
		ERR("app_control_set_operation() is failed");
	} else if (APP_CONTROL_ERROR_NONE != app_control_set_uri(service, telnum)) {
		ERR("app_control_set_uri() is failed");
	} else if (APP_CONTROL_ERROR_NONE != app_control_send_launch_request(service, NULL,
			NULL)) {
		ERR("app_control_send_launch_request() is failed");
	}

	app_control_destroy(service);
}

static void __cst_fdn_list_call_btn_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(NULL == data);
	CstFDNGlItemData_t *item_data = (CstFDNGlItemData_t *)data;

	DBG("CALL CLICKED LAUNCH THE CALL APP");
	__cst_fdn_list_launch_voice_call(
			(const char *)item_data->phbk_info->number);
}

static Evas_Object *__cst_fdn_list_icon_get_cb(void *data, Evas_Object *obj,
		const char *part)
{
	retv_if(NULL == data, NULL);
	CstFDNGlItemData_t *item_data = (CstFDNGlItemData_t *)data;
	Evas_Object *button = NULL;

	if (!strcmp(part, "elm.icon.right")) {
		button = elm_button_add(obj);
		elm_object_style_set(button, "style_fdn_list_call_btn");
		evas_object_propagate_events_set(button, EINA_FALSE);
		evas_object_smart_callback_add(button, "clicked",
				__cst_fdn_list_call_btn_cb, item_data);
#ifdef _CALL_SET_TTS_SUPPORT
		elm_access_info_set(button , ELM_ACCESS_INFO,
				     dgettext("sys_string", "IDS_COM_BODY_VOICE_CALL"));
#endif
		return button;
	}

	return NULL;
}

static void __cst_fdn_contact_edit_cancel(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_fdn_contact_edit_cancel);
	__cst_click_fdn_list_contact_edit_cancel(data, NULL);
}

static void __cst_fdn_contact_edit_save(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_fdn_contact_edit_save);
	ret_if(!data);
	CstFDNGlItemData_t *item_data = (CstFDNGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;

	_cst_create_pin2_confirm_popup(ugd, CST_IME_FDN_CONTACT_PIN2,
			I_(CST_STR_DONE), _cst_fdn_contact_edit_done, NULL,
			(void *)data, NULL);
}

static void __cst_fdn_ctxpopup_rotation_changed(void *data,
		Evas_Object *obj, void *event_info)
{
	ENTER(__cst_fdn_ctxpopup_rotation_changed);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Coord x, y, w , h;
	int rotation_angle = 0;

	if (ugd->fdnmore_popup) {
		elm_win_screen_size_get(ugd->win_main, &x, &y, &w, &h);
		rotation_angle = elm_win_rotation_get((Evas_Object *)ugd->win_main);
		switch (rotation_angle) {
			case 0:
			case 180:
				evas_object_move(ugd->fdnmore_popup, 0, h);
				break;
			case 90:
				evas_object_move(ugd->fdnmore_popup, 0, w);
				break;
			case 270:
				evas_object_move(ugd->fdnmore_popup, h, w);
				break;
			default:
				evas_object_move(ugd->fdnmore_popup, 0, h);
				break;
		}
		evas_object_show(ugd->fdnmore_popup);
	}
}

static void __cst_fdn_dismiss_popup_more_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_fdn_dismiss_popup_more_cb);
	ret_if(NULL == data);

	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->fdnmore_popup) {
		evas_object_smart_callback_del(ugd->win_main, "rotation,changed",
				__cst_fdn_ctxpopup_rotation_changed);

		evas_object_event_callback_del_full(ugd->fdnmore_popup, EVAS_CALLBACK_DEL,
				__cst_fdn_dismiss_popup_more_cb, data);

		evas_object_del(ugd->fdnmore_popup);
		ugd->fdnmore_popup = NULL;
	}
}

static void __cst_fdn_dismiss_popup_more_cb_wrapper(void *data, Evas_Object *obj, void *event_info)
{
	__cst_fdn_dismiss_popup_more_cb(data, NULL, NULL, NULL);
	return;
}

static void more_btn_cb(void *data, Evas_Object* obj, void* event_info)
{
	ENTER(more_btn_cb);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Evas_Coord x, y, w , h;
	int rotation_angle = 0;
	if (ugd->fdnmore_popup) {
		evas_object_del(ugd->fdnmore_popup);
		ugd->fdnmore_popup = NULL;
	}

	ugd->fdnmore_popup = elm_ctxpopup_add(ugd->win_main);
	elm_object_style_set(ugd->fdnmore_popup, "more/default");
	evas_object_smart_callback_add(ugd->fdnmore_popup, "dismissed",
			NULL, NULL);
	eext_object_event_callback_add(ugd->fdnmore_popup, EEXT_CALLBACK_BACK, __cst_fdn_dismiss_popup_more_cb_wrapper, data);
	eext_object_event_callback_add(ugd->fdnmore_popup,  EEXT_CALLBACK_MORE, __cst_fdn_dismiss_popup_more_cb_wrapper, data);
	elm_ctxpopup_auto_hide_disabled_set(ugd->fdnmore_popup, EINA_TRUE);

	evas_object_event_callback_add(ugd->fdnmore_popup, EVAS_CALLBACK_DEL,
			__cst_fdn_dismiss_popup_more_cb, data);

	ugd->kind_of_delete_list = CST_DL_FDN_CONTACTS;

	if (ugd->fdn_contacts_count < g_phnbuk_total_cnt) {
		elm_ctxpopup_item_append(ugd->fdnmore_popup, T_(CST_STR_BODY_ADD), NULL, __cst_add_to_fdn_phnbuk_cb, ugd);
	}
	if (ugd->fdn_contacts_count > 0) {
		elm_ctxpopup_item_append(ugd->fdnmore_popup, T_(CST_STR_REMOVE), NULL, _cst_create_delete_list, ugd);
	}
	elm_ctxpopup_direction_priority_set(ugd->fdnmore_popup, ELM_CTXPOPUP_DIRECTION_UP,
						ELM_CTXPOPUP_DIRECTION_UNKNOWN,
						ELM_CTXPOPUP_DIRECTION_UNKNOWN,
						ELM_CTXPOPUP_DIRECTION_UNKNOWN);
	elm_win_screen_size_get(ugd->win_main, &x, &y, &w, &h);
	evas_object_smart_callback_add(ugd->win_main, "rotation,changed",
			__cst_fdn_ctxpopup_rotation_changed, data);
	rotation_angle = elm_win_rotation_get((Evas_Object *)ugd->win_main);
	switch (rotation_angle) {
		case 0:
		case 180:
			evas_object_move(ugd->fdnmore_popup, (w / 2), h);
			break;
		case 90:
			evas_object_move(ugd->fdnmore_popup,	(h / 2), w);
			break;
		case 270:
			evas_object_move(ugd->fdnmore_popup, (h / 2), w);
			break;
		default:
			evas_object_move(ugd->fdnmore_popup, (w / 2), h);
			break;
	}
	evas_object_show(ugd->fdnmore_popup);
	return;
}

static void __cst_gl_sel_fdn_list_item(void *data, Evas_Object *obj, void *event_info)
{
	ENTER();
	ret_if(data == NULL);
	CstFDNGlItemData_t *item_data = (CstFDNGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	Elm_Object_Item *navi_it ;
	Evas_Object *scroller;
	CstFDNContactEditInfo_t edit_info;
	const char *entry_str_name = NULL;
	const char *entry_str_number = NULL;

	elm_genlist_item_selected_set(item_data->gl_item, EINA_FALSE);

	if (ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) {
		_cst_fdn_disable_main_screen_show_unlock_pin2_popup((void *)ugd);
		return;
	}

	memset(ec_item, 0, sizeof(ec_item));

	g_item_data = item_data;

	SEC_DBG("Name = %s; Number = %s", item_data->phbk_info->name, item_data->phbk_info->number);
	edit_info.name = item_data->phbk_info->name;
	edit_info.number = item_data->phbk_info->number;
	edit_info.index = item_data->phbk_info->index;
	edit_info.next_index = item_data->phbk_info->next_index;

	scroller = _cst_create_fdn_contact_list(ugd, CST_CONTACT_EDIT, &edit_info);


	navi_it = elm_naviframe_item_push(ugd->nf, I_(CST_STR_BODY_EDIT),
										NULL, NULL, scroller, NULL);
	if (navi_it == NULL) {
		DBG("elm_naviframe_item_push fail...");
	}
	cst_util_item_domain_text_translatable_set(navi_it, I_(CST_STR_BODY_EDIT));

	_cst_create_navi_control_bar(ugd->nf,
										(char *)I_(CST_STR_NAVI_BTN_CANCEL), NULL,
										__cst_fdn_contact_edit_cancel,
										(char *)I_(CST_STR_NAVI_BTN_DONE), NULL,
										__cst_fdn_contact_edit_save,
										(void *)g_item_data, navi_it,
										ec_item);

	elm_naviframe_item_pop_cb_set(navi_it, __cst_click_fdn_list_contact_edit_cancel, (void *)g_item_data);

	entry_str_name = elm_entry_entry_get(ugd->dg_entry_contact_name);
	entry_str_number = elm_entry_entry_get(ugd->dg_entry_contact_number);

	if (ec_item[1] != NULL) {
		if ((NULL != entry_str_name && '\0' != entry_str_name[0])
				&& (NULL != entry_str_number && '\0' != entry_str_number[0])) {
			elm_object_disabled_set(ec_item[1], EINA_FALSE);
		} else {
			elm_object_disabled_set(ec_item[1], EINA_TRUE);
		}
	}
	evas_object_smart_callback_add(ugd->nf, "transition,finished", _cst_transition_cb, navi_it);

	return;
}

void _cst_add_new_fdn_contact_to_list(void *data, CallSettingFDNRec_t *fdn_rec, gboolean is_fdn_contact_view)
{
	CstUgData_t *ugd = (CstUgData_t *)data;
	if (!fdn_rec || !data || !ugd->backup_genlist) {
		ERR("Invalid params!");
		return;
	}
	if (genlist_initialized) {
		ugd->fdn_contacts_count++;
		DBG("ugd->fdn_contacts_count = %d", ugd->fdn_contacts_count);
	}

	if (ugd->fdn_contacts_count >= g_phnbuk_total_cnt) {
		_cst_create_toast_popup(T_(CST_STR_MAX_CONTACT_LISTS_REACHED));
	}

	CstFDNGlItemData_t *item_data = g_malloc0(sizeof(CstFDNGlItemData_t));
	ret_if(NULL == item_data);
	char *markup_converted_name = NULL;

	CallSettingFDNRec_t *phbk_info = g_malloc0(sizeof(CallSettingFDNRec_t));
	if (!phbk_info) {
		ERR("phbk_info is NULL");
		g_free(item_data);
		return;
	}

	item_data->phbk_info = phbk_info;
	item_data->phbk_info->index = fdn_rec->index;
	item_data->phbk_info->next_index = fdn_rec->next_index;
	markup_converted_name = elm_entry_utf8_to_markup((const char *)fdn_rec->name);
	item_data->phbk_info->name = (unsigned char *)g_strdup(markup_converted_name);
	free(markup_converted_name);
	markup_converted_name = NULL;
	if (fdn_rec->ton == TAPI_SIM_TON_INTERNATIONAL) {
		char *number = g_strdup_printf("%s", fdn_rec->number);
		item_data->phbk_info->number = (unsigned char *)number;
	} else {
		item_data->phbk_info->number = (unsigned char *)g_strdup((const char *)fdn_rec->number);
	}
	item_data->ugd = (void *)ugd;
	item_data->item_delete_status = EINA_FALSE;
	if (is_fdn_contact_view == EINA_TRUE) { /* Check for item index in the genlist and reorder if necessary */
		Elm_Object_Item *gl_item = NULL, *gl_insert_before_item = NULL;
		CstFDNGlItemData_t *gl_item_data = NULL;

		/* Fetch the first item which is a seperator and ignore it */
		gl_item = elm_genlist_first_item_get(ugd->backup_genlist);
		if (gl_item == NULL) {
			DBG("gl_item = NULL, returning");
			return;
		}

		/* Traverse to the next item which is the FDN contact item */
		gl_item = elm_genlist_item_next_get(gl_item);
		while ((gl_item != NULL) &&
				(elm_genlist_item_next_get(gl_item) != NULL)) { /* Ignore the last gl_item as it is the separator */
			gl_item_data = (CstFDNGlItemData_t *)elm_object_item_data_get(gl_item);
			if (gl_item_data == NULL) {
				DBG("gl_item_data = NULL, returning");
				return;
			}

			if ((gl_item_data->phbk_info->index == item_data->phbk_info->next_index) ||
					(gl_item_data->phbk_info->index > item_data->phbk_info->next_index)) {
				DBG("*** REORDER IS NEEDED HERE ***");
				gl_insert_before_item = gl_item;
				break;
			}
			gl_item = elm_genlist_item_next_get(gl_item);
		}

		if (gl_insert_before_item != NULL) {
			item_data->gl_item = elm_genlist_item_insert_before(ugd->backup_genlist, itc_2text,
								 (void *)item_data, NULL, gl_insert_before_item, ELM_GENLIST_ITEM_NONE,
								 __cst_gl_sel_fdn_list_item, item_data);
			return;
		} else { /* Insert before the bottom separator */
			item_data->gl_item = elm_genlist_item_insert_before(ugd->backup_genlist, itc_2text,
								 (void *)item_data, NULL, gl_item, ELM_GENLIST_ITEM_NONE,
								 __cst_gl_sel_fdn_list_item, item_data);
			return;
		}
	}
	item_data->gl_item = elm_genlist_item_append(ugd->backup_genlist, itc_2text,
						 (void *)item_data, NULL, ELM_GENLIST_ITEM_NONE,
						 __cst_gl_sel_fdn_list_item, item_data);
}

void _cst_update_fdn_list(void *data, CallSettingFDNRec_t *fdn_rec)
{
	CstUgData_t *ugd = (CstUgData_t *)data;
	char *markup_converted_name = NULL;
	if (!fdn_rec || !data || !ugd->backup_genlist) {
		ERR("Invalid params!");
		return;
	}

	if (g_item_data) {
		if (g_item_data->phbk_info->name) {
			g_free(g_item_data->phbk_info->name);
		}
		markup_converted_name = elm_entry_utf8_to_markup((const char *)fdn_rec->name);
		g_item_data->phbk_info->name = (unsigned char *)g_strdup(markup_converted_name);

		if (g_item_data->phbk_info->number) {
			g_free(g_item_data->phbk_info->number);
		}
		g_item_data->phbk_info->number = (unsigned char *)g_strdup((const char *)fdn_rec->number);
		elm_genlist_item_update(g_item_data->gl_item);
		g_item_data = NULL;
		free(markup_converted_name);
		markup_converted_name = NULL;
	} else {
		ERR("g_item_data is NULL");
	}
}

int _cst_get_first_available_free_index()
{
	int index = 0;
	if (g_phnbuk_available_free_index) {
		for (index = 0; index < g_phnbuk_total_cnt; index++) {
			if (!g_phnbuk_available_free_index[index]) {
				return (index+1);
			}
		}
	}
	return index;
}

void _cst_set_index_used(int index, Eina_Bool used)
{
	if (!g_phnbuk_available_free_index || index < 1 || index > g_phnbuk_total_cnt) {
		return;
	}

	g_phnbuk_available_free_index[index-1] = (char)used;
	return;
}

void _cst_fdn_list_initialize_genlist(CstUgData_t *ugd)
{
	ret_if(!ugd);
	if (!ugd->backup_genlist) {

		Evas_Object *layout = evas_object_data_get(ugd->nf, NO_CONTENT_LYT_DATA_KEY);
		Evas_Object *lay_obj = elm_object_part_content_get(layout, "contents");
		if (lay_obj) {
			evas_object_del(lay_obj);
			lay_obj = NULL;
		}

		__cst_set_genlist_item_styles_fdn_list();
		ugd->backup_genlist = elm_genlist_add(ugd->nf);
		elm_genlist_mode_set(ugd->backup_genlist, ELM_LIST_COMPRESS);
		_cst_create_genlist_separator(ugd->backup_genlist, EINA_FALSE);

		elm_object_part_content_set(layout, "contents", ugd->backup_genlist);
		_cst_create_genlist_separator(ugd->backup_genlist, EINA_FALSE);
	}
}

void cst_fdn_list_tapi_sim_response_update_cb(CstUgData_t *ugd, void *req_data, void *resp_data)
{
	ENTER(cst_fdn_list_tapi_sim_response_update_cb);
	ret_if(req_data == NULL);
	ret_if(resp_data == NULL);
	CallSettingSimReq_t *req = (CallSettingSimReq_t *)req_data;
	CallSettingSimResp_t *resp = (CallSettingSimResp_t *)resp_data;
	static int phn_buk_recs_recvd = 0;
	static Evas_Object *more_btn = NULL;

	DBG("sim response received");
	switch (req->req_id) {
	case TAPI_SIM_REQ_GET_PHNBUK_METADATA:
		DBG("TAPI_SIM_REQ_GET_PHNBUK_METADATA received");
		if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
			_cst_get_fdn_phnbuk_cnt(ugd, cst_fdn_list_tapi_sim_response_update_cb, ugd);
		} else {
			ERR("Failed to get the phone book meta data! ");
			char *txt = NULL;
			Evas_Object *layout = NULL, *lay = NULL;
			ugd->fdn_contacts_count = g_phnbuk_cnt = g_phnbuk_total_cnt = 0;
			layout = evas_object_data_get(ugd->nf, NO_CONTENT_LYT_DATA_KEY);
			lay = elm_object_part_content_get(layout, "contents");
			txt = (char *)I_(CST_STR_FDN_NOT_SUPPORTED);
			cst_util_domain_translatable_part_text_set(lay, "elm.text", txt);
		}
		break;

	case TAPI_SIM_REQ_GET_PHNBUK_CNT:
		DBG("TAPI_SIM_REQ_GET_PHNBUK_CNT received");
		if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
			DBG("Used records = %d; Total records = %d", resp->phnbuk_used_rec_cnt, resp->phnbuk_total_rec_cnt);
			if (g_phnbuk_available_free_index) {
				g_free(g_phnbuk_available_free_index);
			}
			g_phnbuk_available_free_index = g_malloc0(resp->phnbuk_total_rec_cnt);
			ugd->fdn_contacts_count = g_phnbuk_cnt = resp->phnbuk_used_rec_cnt;
			g_phnbuk_total_cnt = resp->phnbuk_total_rec_cnt;
			if (resp->phnbuk_used_rec_cnt > 0) {
				/* Some records exists. Lets trigger the read of phone book records. */
				phn_buk_recs_recvd = 0;
				first_index = 1;
				_cst_get_phnbuk_record(ugd, first_index, cst_fdn_list_tapi_sim_response_update_cb, ugd);
			} else {
				Evas_Object *layout = NULL, *lay = NULL;
				ugd->fdn_contacts_count = g_phnbuk_cnt = 0;
				layout = evas_object_data_get(ugd->nf, NO_CONTENT_LYT_DATA_KEY);
				lay = elm_object_part_content_get(layout, "contents");
				cst_util_domain_translatable_part_text_set(lay, "elm.text", I_(CST_STR_NO_CONTACTS));
				cst_util_domain_translatable_part_text_set(lay, "elm.help.text", I_(CST_STR_NO_CONTACTS_HELP_TEXT));
				elm_layout_signal_emit(lay, "text,disabled", "");
				elm_layout_signal_emit(lay, "align.center", "elm");
				/*create more button*/
				more_btn = elm_button_add(ugd->nf);
				elm_object_style_set(more_btn, "naviframe/more/default");
				evas_object_smart_callback_add(more_btn, "clicked", more_btn_cb, ugd);
				Elm_Object_Item *top_it = elm_naviframe_top_item_get(ugd->nf);
				elm_object_item_part_content_set(top_it, "toolbar_more_btn", more_btn);
				genlist_initialized = EINA_TRUE;
			}
		} else {
			ERR("Failed to get the phone book record count! ");
			char *txt = NULL;
			Evas_Object *layout = NULL, *lay = NULL;
			ugd->fdn_contacts_count = g_phnbuk_cnt = g_phnbuk_total_cnt = 0;
			layout = evas_object_data_get(ugd->nf, NO_CONTENT_LYT_DATA_KEY);
			lay = elm_object_part_content_get(layout, "contents");
			txt = (char *)I_(CST_STR_FDN_NOT_SUPPORTED);
			cst_util_domain_translatable_part_text_set(lay, "elm.text", txt);
		}
		break;

	case TAPI_SIM_REQ_GET_PHNBUK_REC:
		DBG("TAPI_SIM_REQ_GET_PHNBUK_REC received");
		if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
			phn_buk_recs_recvd++;
			if (!ugd->backup_genlist) {
				Evas_Object *layout = NULL;
				__cst_set_genlist_item_styles_fdn_list();
				ugd->backup_genlist = elm_genlist_add(ugd->nf);
				elm_genlist_mode_set(ugd->backup_genlist, ELM_LIST_COMPRESS);
				_cst_create_genlist_separator(ugd->backup_genlist, EINA_FALSE);

				layout = evas_object_data_get(ugd->nf, NO_CONTENT_LYT_DATA_KEY);
				Evas_Object *lay_obj = elm_object_part_content_get(layout, "contents");
				if (lay_obj) {
					evas_object_del(lay_obj);
					lay_obj = NULL;
				}
				elm_object_part_content_set(layout, "contents", ugd->backup_genlist);
				/*create more button*/
				more_btn = elm_button_add(ugd->nf);
				elm_object_style_set(more_btn, "naviframe/more/default");
				evas_object_smart_callback_add(more_btn, "clicked", more_btn_cb, ugd);
				Elm_Object_Item *top_it = elm_naviframe_top_item_get(ugd->nf);
				elm_object_item_part_content_set(top_it, "toolbar_more_btn", more_btn);
			}
			_cst_set_index_used(resp->fdn_rec.index, 1);
			_cst_add_new_fdn_contact_to_list(ugd, &(resp->fdn_rec), EINA_FALSE);

			DBG("phn_buk_recs_recvd = %d; g_phnbuk_cnt = %d", phn_buk_recs_recvd, g_phnbuk_cnt);
			if (phn_buk_recs_recvd < g_phnbuk_cnt) {
				_cst_get_phnbuk_record(ugd, resp->fdn_rec.next_index, cst_fdn_list_tapi_sim_response_update_cb, ugd);
			} else {
				_cst_create_genlist_separator(ugd->backup_genlist, EINA_FALSE);
				evas_object_smart_callback_add(ugd->backup_genlist, "realized",
						_cst_gl_realized_cb, (const void *)CST_DIALOG_GROUP_CALL_FORWARDING_BARRING_FDN_LIST);
				genlist_initialized = EINA_TRUE;
				/* Received all the FDN contacts. */
			}
		} else if (phn_buk_recs_recvd < g_phnbuk_cnt && first_index < g_phnbuk_total_cnt) {
			first_index++;
			_cst_get_phnbuk_record(ugd, first_index, cst_fdn_list_tapi_sim_response_update_cb, ugd);
		} else {
			ERR("Fatal: Unable to read the phone book record!");
			char *txt = NULL;
			Evas_Object *layout = NULL, *lay = NULL;
			ugd->fdn_contacts_count = g_phnbuk_cnt = g_phnbuk_total_cnt = 0;
			layout = evas_object_data_get(ugd->nf, NO_CONTENT_LYT_DATA_KEY);
			lay = elm_object_part_content_get(layout, "contents");
			txt = (char *)I_(CST_STR_FDN_NOT_SUPPORTED);
			cst_util_domain_translatable_part_text_set(lay, "elm.text", txt);
		}
		break;

	case TAPI_SIM_REQ_VERIFY_PIN2:
		DBG("TAPI_SIM_REQ_VERIFY_PIN2 received");
		if (req->next_req_params) {
			if (TAPI_SIM_REQ_DELETE_PHNBUK_REC == req->next_req_params->req_id) {
				if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
					DBG("PIN2 AUTH SUCCESS");
					ugd->sim_lock_retry_account = 3;
					_cst_click_FDN_delete_list_ime_back_cb(ugd, NULL);
				} else if (resp->retry_count) {
					DBG("PIN2 AUTH FAILED");
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
					elm_object_focus_set(ugd->dg_entry, EINA_TRUE);
				} else {
					/* Max try over */
					_cst_click_FDN_delete_list_ime_back_cb(ugd, NULL);
					ugd->sim_lock_status = TAPI_SIM_LOCK_KEY_PUK2;
					ugd->sim_lock_retry_account = 10;
					_cst_fdn_disable_main_screen_show_unlock_pin2_popup(
							(void *)ugd);
				}

				if (ugd->backup_genlist) {
					/* Delete the checked item from the phonebook record if Authentication is successful */
					Elm_Object_Item *del_item = NULL;
					CstFDNGlItemData_t *cur_item_data = NULL;

					/* Fetch the first item which is a seperator and ignore it */
					del_item = elm_genlist_first_item_get(ugd->backup_genlist);
					if (del_item == NULL) {
						DBG("del_item = NULL, returning");
						return;
					}

					/* Traverse to the next item which is the FDN contact item */
					del_item = elm_genlist_item_next_get(del_item);
					while ((del_item != NULL) &&
							(elm_genlist_item_next_get(del_item) != NULL)) { /* Ignore the last gl_item as it is the separator */
						cur_item_data = (CstFDNGlItemData_t *)elm_object_item_data_get(del_item);
						if (cur_item_data == NULL) {
							DBG("cur_item_data = NULL, returning");
							return;
						}

						if (cur_item_data->item_delete_status == EINA_TRUE) {
							if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
								SEC_DBG("Contact Name = %s selected for delete", cur_item_data->phbk_info->name);
								_cst_delete_phnbuk_record(ugd, cur_item_data->phbk_info->index, cst_fdn_list_tapi_sim_response_update_cb, del_item);
							}
							cur_item_data->item_delete_status = EINA_FALSE;
						}
						del_item = elm_genlist_item_next_get(del_item);
					}
				}
			}
		}
		break;

	case TAPI_SIM_REQ_DELETE_PHNBUK_REC:
		DBG("TAPI_SIM_REQ_DELETE_PHNBUK_REC received");
		if (TAPI_SIM_OPERATION_SUCCESS == resp->result) {
			DBG("Contact record deleted successfully!");
			Elm_Object_Item *deleted_item = (Elm_Object_Item *)req->data;
			if (deleted_item != NULL) { /* Delete the item from the Genlist */
				CstFDNGlItemData_t *item_data = NULL;
				item_data = (CstFDNGlItemData_t *)elm_object_item_data_get(deleted_item);
				if (item_data != NULL) {
					_cst_set_index_used(item_data->phbk_info->index, 0);
				}
				elm_object_item_del(deleted_item);
				ugd->fdn_contacts_count--;
				_cst_fdn_list_set_button_status(ugd->fdn_contacts_count, g_phnbuk_total_cnt);

				DBG("ugd->fdn_contacts_count = %d", ugd->fdn_contacts_count);
				if (ugd->fdn_contacts_count > 0) {
					_cst_gl_realized_cb((void *)CST_DIALOG_GROUP_CALL_FORWARDING_BARRING_FDN_LIST, ugd->backup_genlist, NULL);
				} else {
					evas_object_smart_callback_del(ugd->backup_genlist, "realized", _cst_gl_realized_cb);
				}
				/*_cst_create_indicator_popup(T_(CST_STR_DELETED));*/
			}

			if (ugd->fdn_contacts_count <= 0) { /* Show Nocontent layout */
				Evas_Object *layout = evas_object_data_get(ugd->nf, NO_CONTENT_LYT_DATA_KEY);
				if (layout) {
					if (ugd->backup_genlist) {
						elm_genlist_clear(ugd->backup_genlist);
						ugd->backup_genlist = NULL;
					}
					/* Create elm_layout and set its style as nocontents/text */
					Evas_Object *lay = elm_layout_add(ugd->nf);
					elm_layout_theme_set(lay, "layout", "nocontents", "search");
					elm_object_part_content_set(layout, "contents", lay);

					if (TRUE == _cst_is_sim_phnbuk_ready(ugd, &first_index)) {
						_cst_get_fdn_phnbuk_metadata(ugd, cst_fdn_list_tapi_sim_response_update_cb, ugd);
					} else {
						const char *txt = I_(CST_STR_FDN_NOT_SUPPORTED);
						cst_util_domain_translatable_part_text_set(lay, "elm.text", txt);
					}
				}
			}
		} else {
			DBG("Contact record deleted UnSuccessfully!");
		}
		break;

	default:
		ERR("Response received for an Invalid Request id[%d]", req->req_id);
		break;
	}

	LEAVE();
	return;
}

static void __cst_set_genlist_item_styles_fdn_list(void)
{
	ENTER(__cst_set_genlist_item_styles_fdn_list);

	if (!itc_2text) {
		itc_2text = _cst_create_genlist_item_class("2line.top",
				__cst_gl_label_get_cb, __cst_fdn_list_icon_get_cb, NULL,
				__cst_gl_del_cb);
	}
}

static void __cst_fdn_contact_create_cancel(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_fdn_contact_create_cancel);
	__cst_click_fdn_list_contact_create_cancel(data, NULL);
}

static void __cst_fdn_contact_create_save(void *data, Evas_Object *obj,
		void *event_info)
{
	ENTER(__cst_fdn_contact_create_save);
	ret_if(!data);
	CstUgData_t *ugd = (CstUgData_t *)data;

	_cst_create_pin2_confirm_popup(ugd, CST_IME_FDN_CONTACT_PIN2,
			 I_(CST_STR_DONE), _cst_fdn_contact_create_done, NULL,
			(void *)data, NULL);
}

static void __cst_add_to_fdn_phnbuk_cb(void *data, Evas_Object *obj, void *event_info)
{
	ENTER(__cst_add_to_fdn_phnbuk_cb);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Elm_Object_Item *navi_it;
	Evas_Object *scroller;
	const char *entry_str_name = NULL;
	const char *entry_str_number = NULL;

	if (ugd->fdnmore_popup) {
		evas_object_del(ugd->fdnmore_popup);
		ugd->fdnmore_popup = NULL;
	}

	if ((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) && (ugd->sim_lock_retry_account == 0)) {
		/* PUK2 blocked (PIN2 permanently blocked) */
		ugd->popup = _cst_create_1btn_popup(ugd->nf, ugd,
				T_(CST_STR_PUK2_BLOCKED), NULL,
				NULL, ugd);
		return;
	} else if (((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PUK2) && (ugd->sim_lock_retry_account > 0))
		|| ((ugd->sim_lock_status == TAPI_SIM_LOCK_KEY_PIN2) && (ugd->sim_lock_retry_account == 0))) {
		_cst_fdn_disable_main_screen_show_unlock_pin2_popup((void *)ugd);
		return;
	}

	memset(ec_item, 0, sizeof(ec_item));

	scroller = _cst_create_fdn_contact_list(data, CST_CONTACT_ADD, NULL);

	navi_it = elm_naviframe_item_push(ugd->nf, T_(CST_STR_BODY_ADD),
										NULL, NULL, scroller, NULL);
	if (navi_it == NULL) {
		DBG("elm_naviframe_item_push fail...");
	}
	cst_util_item_domain_text_translatable_set(navi_it, T_(CST_STR_BODY_ADD));

	_cst_create_navi_control_bar(ugd->nf,
										(char *)I_(CST_STR_NAVI_BTN_CANCEL), NULL,
										__cst_fdn_contact_create_cancel,
										(char *)I_(CST_STR_NAVI_BTN_DONE), NULL,
										__cst_fdn_contact_create_save,
										(void *)ugd, navi_it,
										ec_item);

	elm_naviframe_item_pop_cb_set(navi_it, __cst_click_fdn_list_contact_create_cancel, ugd);

	entry_str_name = elm_entry_entry_get(ugd->dg_entry_contact_name);
	entry_str_number = elm_entry_entry_get(ugd->dg_entry_contact_number);

	if (ec_item[1] != NULL) {
		if ((NULL != entry_str_name && '\0' != entry_str_name[0])
				&& (NULL != entry_str_number && '\0' != entry_str_number[0])) {
			elm_object_disabled_set(ec_item[1], EINA_FALSE);
		} else {
			elm_object_disabled_set(ec_item[1], EINA_TRUE);
		}
	}
	evas_object_smart_callback_add(ugd->nf, "transition,finished", _cst_transition_cb, navi_it);

	LEAVE();
}

#if 0 /* Function not used */
static void __cst_create_fdn_navi_control_bar_btns(void *data)
{
	ENTER(__cst_create_fdn_navi_control_bar_btns);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Elm_Object_Item *top_it = elm_naviframe_top_item_get(ugd->nf);

	if (strcmp(elm_object_item_part_text_get(top_it, "elm.text.title"), T_(CST_STR_FDN_CONTACTS)) != 0) {
		return;
	}

	ugd->kind_of_delete_list = CST_DL_FDN_CONTACTS;

	_cst_create_navi_control_bar(ugd->nf,
										(char *)I_(CST_STR_SK3_CREATE), NULL,
										__cst_add_to_fdn_phnbuk_cb,
										(char *)I_(CST_STR_DONE), NULL,
										_cst_create_delete_list,
										(void *)ugd, top_it,
										ec_item);
}
#endif

static void __cst_destroy_genlist_item_styles(void)
{
	ENTER(__cst_destroy_genlist_item_styles);

	if (itc_2text) {
		_cst_destroy_genlist_item_class(itc_2text);
		itc_2text = NULL;
	}
}

void _cst_destroy_fdn_list(CstUgData_t *ugd)
{
	ENTER(_cst_destroy_fdn_list);
	ret_if(!ugd);

	__cst_destroy_genlist_item_styles();

	if (ugd->fdnmore_popup) {
		evas_object_del(ugd->fdnmore_popup);
		ugd->fdnmore_popup = NULL;
	}

	if (ugd->backup_genlist) {
		ugd->backup_genlist = NULL;
	}

	if (ugd->backup_layout) {
		ugd->backup_layout = NULL;
	}

	if (ugd->sim_req_list) {
		int i;
		for (i = 0; i < g_slist_length(ugd->sim_req_list); i++) {
			((CallSettingSimReq_t *)g_slist_nth_data(ugd->sim_req_list, i))->req_state = SIM_REQUEST_CANCELLED;
		}
	}
}

static Eina_Bool __cst_on_click_fdn_list_back_button(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_on_click_fdn_list_back_button);

	_cst_destroy_fdn_list(data);

	return EINA_TRUE;
}

void _cst_on_click_fdn_list(void *data)
{
	ENTER(_cst_on_click_fdn_list);
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	Elm_Object_Item *navi_it;
	Evas_Object *layout, *lay;
	const char *txt;

	if (ugd->backup_genlist) {
		elm_genlist_clear(ugd->backup_genlist);
		ugd->backup_genlist = NULL;
	}
	genlist_initialized = EINA_FALSE;
	ugd->fdn_contacts_count = 0;

	/* Create elm_layout and set its style as nocontents/text */
	lay = elm_layout_add(ugd->nf);
	txt = I_(CST_STR_READING_FROM_SIM_CARD);
	cst_util_domain_translatable_part_text_set(lay, "elm.text", txt);
	elm_layout_theme_set(lay, "layout", "nocontents", "search");

	/* Full view layout */
	layout = elm_layout_add(ugd->nf);
	ugd->backup_layout = layout;

	elm_layout_file_set(layout, EDJ_NAME, "nocontents");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_data_set(ugd->nf, NO_CONTENT_LYT_DATA_KEY, layout);
	elm_object_part_content_set(layout, "contents", lay);

	if (TRUE == _cst_is_sim_phnbuk_ready(data, &first_index)) {
		_cst_get_fdn_phnbuk_metadata(ugd, cst_fdn_list_tapi_sim_response_update_cb, ugd);
	} else {
		txt = I_(CST_STR_FDN_NOT_SUPPORTED);
		cst_util_domain_translatable_part_text_set(lay, "elm.text", txt);
	}

	navi_it = elm_naviframe_item_push(ugd->nf, I_(CST_STR_FDN_CONTACTS),
										NULL, NULL, layout, NULL);
	if (navi_it == NULL) {
		DBG("elm_naviframe_item_push fail...");
	}
	cst_util_item_domain_text_translatable_set(navi_it, I_(CST_STR_FDN_CONTACTS));

	elm_naviframe_item_pop_cb_set(navi_it, __cst_on_click_fdn_list_back_button,
			(void *)ugd);

	LEAVE();
}

static Eina_Bool __cst_click_fdn_list_contact_edit_cancel(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_click_fdn_list_contact_edit_cancel);
	retv_if(!data, EINA_TRUE);
	CstFDNGlItemData_t *item_data = (CstFDNGlItemData_t *)data;
	CstUgData_t *ugd = (CstUgData_t *)item_data->ugd;
	g_item_data = NULL;

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	_cst_fdn_list_back_to_prev(ugd);

	return EINA_TRUE;
}

static Eina_Bool __cst_click_fdn_list_contact_create_cancel(void *data, Elm_Object_Item *it)
{
	ENTER(__cst_click_fdn_list_contact_create_cancel);
	retv_if(!data, EINA_TRUE);
	CstUgData_t *ugd = (CstUgData_t *)data;

	if (ugd->popup) {
		evas_object_del(ugd->popup);
		ugd->popup = NULL;
	}

	_cst_fdn_list_back_to_prev(ugd);

	return EINA_TRUE;
}

void _cst_fdn_list_changed_editfield_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data);
	CstUgData_t *ugd = (CstUgData_t *)data;
	const char *entry_str = elm_entry_entry_get(obj);

	DBG("entry_str = %s", entry_str);
	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ugd->nf);
	ret_if(!navi_it);
	Evas_Object *btn = elm_object_item_part_content_get(navi_it,
					   "title_right_btn");

	if (ugd->dg_entry_contact_name != NULL &&
			   ugd->dg_entry_contact_number != NULL) {
		const char *entry_str_name = elm_entry_entry_get(ugd->dg_entry_contact_name);
		const char *entry_str_number = elm_entry_entry_get(ugd->dg_entry_contact_number);

		if ((NULL != entry_str_name && '\0' != entry_str_name[0])
				&& (NULL != entry_str_number && '\0' != entry_str_number[0])) {
			if (btn != NULL) {
				elm_object_disabled_set(btn, EINA_FALSE);
			}
			if (ec_item[1] != NULL) {
				elm_object_disabled_set(ec_item[1], EINA_FALSE);
			}
		} else {
			if (btn != NULL) {
				elm_object_disabled_set(btn, EINA_TRUE);
			}
			if (ec_item[1] != NULL) {
				elm_object_disabled_set(ec_item[1], EINA_TRUE);
			}
		}

		if (ugd->dg_entry_pin2 != NULL) {
			const char *entry_str_pin2 = elm_entry_entry_get(ugd->dg_entry_pin2);
			Evas_Object *done_btn = elm_object_part_content_get(ugd->popup,
					"button2");

			if (NULL == entry_str_pin2 || '\0' == entry_str_pin2[0] || 4 >
					strlen(entry_str_pin2)) {
				if (done_btn != NULL) {
					elm_object_disabled_set(done_btn, EINA_TRUE);
				}
			} else {
				if (done_btn != NULL) {
					elm_object_disabled_set(done_btn, EINA_FALSE);
				}
			}
		}
	}
}

void _cst_fdn_list_input_panel_event_cb(void *data, Ecore_IMF_Context *imf_context, int value)
{
	ENTER(_cst_fdn_list_input_panel_event_cb);
	ret_if(NULL == data);

	if (value == ECORE_IMF_INPUT_PANEL_STATE_WILL_SHOW) {
		return;
	}

	if (EINA_FALSE == keypad_enabled) {
		DBG("keypad_enabled = %d", keypad_enabled);
		keypad_enabled = EINA_TRUE;

		if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
			return;
		}
	}

	if (value == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_SHOW");
	} else if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
		DBG("ECORE_IMF_INPUT_PANEL_STATE_HIDE");
	}
}

static Eina_Bool _cst_fdn_list_naviframe_item_pop_cb(void *data)
{
	CstUgData_t *ugd = (CstUgData_t *)data;

	evas_object_smart_callback_del(ugd->nf, "transition,finished", _cst_transition_cb);

	ugd->dg_entry = NULL;

	elm_naviframe_item_pop_cb_set(elm_naviframe_top_item_get(ugd->nf), NULL, NULL);

	elm_naviframe_item_pop(ugd->nf);

	memset(ec_item, 0, sizeof(ec_item));

	ugd->dg_entry_contact_name = NULL;
	ugd->dg_entry_contact_number = NULL;
	ugd->dg_entry_pin2 = NULL;


	return ECORE_CALLBACK_CANCEL;
}

void _cst_fdn_list_back_to_prev(CstUgData_t *ugd)
{
	ret_if(!ugd);
	Ecore_IMF_Context *imf_context = elm_entry_imf_context_get(ugd->dg_entry_pin2);

	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, _cst_fdn_list_input_panel_event_cb);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}

	evas_object_smart_callback_del(ugd->dg_entry_pin2, "changed", _cst_fdn_list_changed_editfield_cb);
	evas_object_smart_callback_del(ugd->dg_entry_pin2, "preedit,changed", _cst_fdn_list_changed_editfield_cb);

	ugd->dg_entry_pin2 = NULL;

	imf_context = elm_entry_imf_context_get(ugd->dg_entry_contact_name);
	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, _cst_fdn_list_input_panel_event_cb);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}

	evas_object_smart_callback_del(ugd->dg_entry_contact_name, "changed", _cst_fdn_list_changed_editfield_cb);
	evas_object_smart_callback_del(ugd->dg_entry_contact_name, "preedit,changed", _cst_fdn_list_changed_editfield_cb);

	ugd->dg_entry_contact_name = NULL;

	imf_context = elm_entry_imf_context_get(ugd->dg_entry_contact_number);
	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_del(imf_context, ECORE_IMF_INPUT_PANEL_STATE_EVENT, _cst_fdn_list_input_panel_event_cb);
		ecore_imf_context_input_panel_enabled_set(imf_context, EINA_FALSE);
	}

	evas_object_smart_callback_del(ugd->dg_entry_contact_number, "changed", _cst_fdn_list_changed_editfield_cb);
	evas_object_smart_callback_del(ugd->dg_entry_contact_number, "preedit,changed", _cst_fdn_list_changed_editfield_cb);

	ugd->dg_entry_contact_number = NULL;

	keypad_enabled = EINA_FALSE;

/*	ecore_timer_add(CST_NAVIFRAME_ITEM_POP_TIMER, _cst_fdn_list_naviframe_item_pop_cb, ugd);
*/
	_cst_fdn_list_naviframe_item_pop_cb(ugd);
}

#endif  /* _CALL_SET_FDN_SUPPORT */
