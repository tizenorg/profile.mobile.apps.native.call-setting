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
		void setSelectHandler(NotifyHandler handler);
		virtual Elm_Genlist_Item_Class *getItemClass();

	protected:
		GenlistItem();
		virtual ~GenlistItem();
		virtual void onSelected();
		virtual void onRealized() {}
		virtual void onUnrealized() {}
		virtual char *getText(const char *part) {return nullptr;}
		virtual Evas_Object *getContent(const char *part) {return nullptr;}
		virtual Eina_Bool getState(const char *part) {return EINA_FALSE;}
		Elm_Genlist_Item_Class createItemClass(const char *style = "default");

	private:
		friend class WidgetItem;
		friend class Genlist;

		typedef std::function<Elm_Object_Item *(Genlist &list, GenlistItem *item)> ItemAddMethod;
		bool initialize(Genlist &list, ItemAddMethod createItem);

		typedef std::function<Elm_Object_Item *(Genlist &list, GenlistItem &parentItem, GenlistItem *item)> ItemInsertMethod;
		bool initialize(Genlist &list, GenlistItem &parentItem, ItemInsertMethod createItem);

	private:
		Elm_Genlist_Item_Class *m_pItemClass;
		NotifyHandler m_onSelectHandler;
	};
}
#endif /* GENLIST_ITEM_H_ */
