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

#ifndef _CST_UTIL_H_
#define _CST_UTIL_H_

#include <Elementary.h>
#include <app.h>
#include <vconf.h>
#include <system_settings.h>

#include "cst-common.h"

void cst_util_domain_translatable_text_set(Evas_Object *obj, const char* text);
void cst_util_domain_translatable_part_text_set(Evas_Object *obj, const char* part, const char* text);
void cst_util_item_domain_translatable_part_text_set(Elm_Object_Item *it, const char* part, const char* text);
void cst_util_item_domain_text_translatable_set(Elm_Object_Item *it, const char* text);
void cst_util_feedback_init(void);
void cst_util_feedback_deinit(void);
void cst_util_feedback_play_tap_sound(void);
Eina_Bool _cst_vconf_get_bool(const char *in_key, int *boolval);
Eina_Bool _cst_vconf_get_int(const char *in_key, int *intval);
char *_cst_vconf_get_str(const char *in_key);
void _cst_vconf_set_bool(const char *in_key, const int boolval);
void _cst_vconf_set_int(const char *in_key, const int intval);
void _cst_vconf_set_str(const char *in_key, const char *strval);
Eina_Bool _cst_check_flight_mode(void);
Eina_Bool _cst_check_dual_sim_status(CstAppData_t *ad);
int _cst_parse_bundle(app_control_h app_control);
void _cst_on_click_ime_contact_btn(void *data, Evas_Object *obj, void *event_info);
void _cst_connect_reject_list_handle(CstAppData_t *ad);
void _cst_unlisten_vconf_change(void);
Eina_Bool _cst_naviframe_item_pop_cb(void *data);
void _cst_flight_mode_genlist_item_disable(void *data);
void _cst_flight_mode_changed_cb_for_call_forwarding(system_settings_key_e key, void *user_data);
void _cst_vconfkey_change_notify_cb(keynode_t *node, void *data);
void _cst_flight_mode_changed_cb_for_voice_mail_number(system_settings_key_e key, void *user_data);
void _cst_flight_mode_changed_cb_for_call_barring(system_settings_key_e key, void *user_data);
void _cst_flight_mode_changed_cb_for_call_waiting(system_settings_key_e key, void *user_data);
void _cst_flight_mode_unlisten_setting_change_for_main_menu(void);
void _cst_flight_mode_unlisten_setting_change_for_more_call_setting(void);
Eina_Bool _cst_util_remove_invalid_chars_from_phone_num(const char *src_num, char *dst_num);

/**
 * @brief Set callback on system setting change
 * @param[in] key The key name of the system settings
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data to be passed to the callback function
 * @return Error code
 */
int _cst_util_system_settings_set_changed_cb(system_settings_key_e key, system_settings_changed_cb callback, void *user_data);

/**
 * @brief Unset callback on system setting change
 * @param[in] key The key name of the system settings
 * @param[in] callback The callback function
 * @return Error code
 */
int _cst_util_system_settings_unset_changed_cb(system_settings_key_e key, system_settings_changed_cb callback);

/**
 * @brief Unset all callbacks for specific setting change
 * @param[in] key The key name of the system settings
 * @return Error code
 */
int _cst_util_system_settings_unset_changed_cb_all(system_settings_key_e key);

Evas_Object *_cst_util_navi_back_btn_create(Evas_Object *naviframe);

const char *_cst_util_get_edj_path();

const char *_cst_util_get_locale_path();

#endif

