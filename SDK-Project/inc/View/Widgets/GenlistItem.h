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

#ifndef GENLIST_ITEM_H_
#define GENLIST_ITEM_H_

#include <functional>

#include "View/Widgets/WidgetItem.h"
#include "Utils/Common.h"

namespace Widgets {

	class Genlist;
	class GenlistItem : public WidgetItem {

	public:
	typedef enum {
		GENLIST_ITEM_FIELD_TEXT = 1,
		GENLIST_ITEM_FIELD_CONTENT = 2,
		GENLIST_ITEM_FIELD_STATE = 4,
	} GenlistPartType;

	typedef enum {
		GENLIST_ITEM_SELECT_MODE_ONCE, /*Item is selected only once, select callback calls only once*/
		GENLIST_ITEM_SELECT_MODE_ALWAYS, /*Item selected callbacks will be called every time for click events*/
		GENLIST_ITEM_SELECT_MODE_NONE, /*Item never be selected and no callback will be invoked*/
	} ItemSelectionMode;

	typedef std::function<Elm_Object_Item *(GenlistItem *item)> ItemAddMethod;

	public:
		virtual Elm_Genlist_Item_Class *getItemClass();
		void setSelectHandler(NotifyHandler handler);
		void setSelectionMode(ItemSelectionMode mode);
		void update(const char *parts = nullptr, int updateFlag = 0);

	protected:
		GenlistItem();
		virtual ~GenlistItem();
		virtual void onSelected();
		virtual void onRealized() {}
		virtual void onUnrealized() {}
		virtual char *getText(const char *part) {return nullptr;}
		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) {return nullptr;}
		virtual Eina_Bool getState(const char *part) {return EINA_FALSE;}
		Elm_Genlist_Item_Class createItemClass(const char *style = "default");
		bool initialize(ItemSelectionMode itemSelectMode = GENLIST_ITEM_SELECT_MODE_ALWAYS);

	private:
		friend class Genlist;
		static GenlistItem *fromData(void *data);

	private:
		NotifyHandler m_selectHandler;
	};
}
#endif /* GENLIST_ITEM_H_ */
