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

#include "cst-editfield.h"
#include <Elementary.h>

static void clear_button_cb(void *data, Evas_Object *obj, void *event_info);
static void focus_cb(void *data, Evas_Object *obj, void *event_info);
static void unfocus_cb(void *data, Evas_Object *obj, void *event_info);
static void change_cb(void *data, Evas_Object *obj, void *event_info);

static void clear_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	elm_entry_entry_set((Evas_Object *) data, "");
}

static void focus_cb(void *data, Evas_Object *obj, void *event_info)
{
	elm_object_signal_emit((Evas_Object*) data, "elm,state,focused", "");
}

static void unfocus_cb(void *data, Evas_Object *obj, void *event_info)
{
	elm_object_signal_emit((Evas_Object*) data, "elm,state,unfocused", "");
}

static void change_cb(void *data, Evas_Object *obj, void *event_info)
{
	if (elm_entry_is_empty(obj))
		elm_object_signal_emit((Evas_Object*) data, "elm,action,hide,button", "");
	else
		elm_object_signal_emit((Evas_Object*) data, "elm,action,show,button", "");
}

Evas_Object *editfield_create(Evas_Object *parent, Editfield_Type type, const char *guide_text)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "editfield", "singleline");

	Evas_Object *entry = elm_entry_add(parent);
	elm_object_part_content_set(layout, "elm.swallow.content", entry);

	if (ET_SINGLELINE == type || ET_PASSWORD == type) {
		elm_entry_single_line_set(entry, EINA_TRUE);
		elm_entry_scrollable_set(entry, EINA_TRUE);
	}
	if (ET_PASSWORD == type) {
		elm_entry_password_set(entry, EINA_TRUE);
	}

	if (guide_text) {
		elm_object_part_text_set(entry, "elm.guide", guide_text);
	}

	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "editfield_clear");
	elm_object_focus_allow_set(button, EINA_FALSE);
	elm_object_part_content_set(layout, "elm.swallow.button", button);

	evas_object_smart_callback_add(button, "clicked", clear_button_cb, entry);
	evas_object_smart_callback_add(entry, "focused", focus_cb, layout);
	evas_object_smart_callback_add(entry, "unfocused", unfocus_cb, layout);
	evas_object_smart_callback_add(entry, "changed", change_cb, layout);

	return layout;
}

void editfield_clear_button_disabled_set(Evas_Object *editfield, Eina_Bool set)
{
	Evas_Object *button = elm_object_part_content_get(editfield, "elm.swallow.button");
	if (button) {
		if (set) {
			evas_object_hide(button);
		} else {
			evas_object_show(button);
		}
	}
}

Evas_Object *editfield_get_entry(Evas_Object *editfield)
{
	return elm_object_part_content_get(editfield, "elm.swallow.content");
}
