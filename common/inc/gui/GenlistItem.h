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

#ifndef _GUI_GENLIST_ITEM_H_
#define _GUI_GENLIST_ITEM_H_

#include "WidgetItem.h"

namespace gui {

	class Genlist;

	class GenlistItem : public WidgetItem {
	public:

		virtual Elm_Genlist_Item_Class *getItemClass();
		void setSelectHandler(ItemNotiHandler handler);
		void setSelectionMode(ItemSelectionMode mode);

	protected:
		typedef std::function<Elm_Object_Item *(GenlistItem *item)> ItemAddMethod;

	protected:
		GenlistItem();
		virtual ~GenlistItem();
		bool initialize(ItemAddMethod createItem, ItemSelectionMode itemSelectMode = GENLIST_ITEM_SELECT_MODE_ALWAYS);
		virtual void onSelected();
		virtual char *getText(const char *part) {return nullptr;}
		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) {return nullptr;}
		virtual Eina_Bool getState(const char *part) {return EINA_FALSE;}
		static Elm_Genlist_Item_Class createItemClass(const char *style = "default");
		void update();
		void update(const char *parts, int updateFlag);

	private:
		friend class Genlist;
		static GenlistItem *fromData(void *data);

	private:
		ItemNotiHandler m_selectHandler;
	};
}
#endif /* _GUI_GENLIST_ITEM_H_ */
