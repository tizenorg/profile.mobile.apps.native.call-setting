/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _GUI_GUITYPES_H_
#define _GUI_GUITYPES_H_

#include <Elementary.h>
#include <efl_extension.h>

#include "Common.h"

#define GUI_COLOR_SUBITEM_TEXT_HEX "#3DB9CC"

#define TO_EINA_BOOL(b) ((b) ? EINA_TRUE : EINA_FALSE)

namespace gui {

	class BaseView;
	typedef util::Delegate<void(BaseView *)> ViewDestroyHandler;

	class Widget;
	typedef util::Delegate <void(Widget &)> WidgetNotiHandler;

	class WidgetItem;
	typedef util::Delegate <void(WidgetItem &)> ItemNotiHandler;

	typedef util::Callback<void(Evas *e, Evas_Object *obj, void *event_info)> EvasEventCb;
	typedef util::Callback<void(Evas_Object *obj, void *event_info)> EoSmartCb;

	typedef enum {
			VIEW_CHANGE_BEGIN,
			VIEW_CHANGE_END,
			MENU_BUTTON_PRESSED,
			BACK_BUTTON_PRESSED,
			ORIENTATION_CHANGED,
		} ViewEvent;

	typedef util::Delegate<void(ViewEvent)> ViewEventHandler;
	typedef util::Delegation<void(ViewEvent)> ViewEventHandlersDelegation;

	typedef enum {
			GL_PART_TYPE_TEXT = 1,
			GL_PART_TYPE_CONTENT = 2,
			GL_PART_TYPE_STATE = 4,
		} GenlistPartType;

	typedef enum {
		GENLIST_ITEM_SELECT_MODE_ONCE, /*Item is selected only once, select callback calls only once*/
		GENLIST_ITEM_SELECT_MODE_ALWAYS, /*Item selected callbacks will be called every time for click events*/
		GENLIST_ITEM_SELECT_MODE_NONE, /*Item never be selected and no callback will be invoked*/
	} ItemSelectionMode;

	typedef enum {
		CHECKBOX_DEFAULT,
		CHECKBOX_SWITCHER,
		CHECKBOX_STAR,
	} CheckboxStyle;

}

#endif /* _GUI_GUITYPES_H_ */
