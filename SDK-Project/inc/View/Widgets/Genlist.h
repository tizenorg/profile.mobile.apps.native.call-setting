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

#ifndef GENLIST_H_
#define GENLIST_H_

#include "View/Widgets/Widget.h"
#include "View/Widgets/WidgetItem.h"
#include "View/Widgets/GenlistItem.h"

namespace Widgets {
	class Genlist : public Widget {

	public:
		template <class GENLIST_ITEM_TYPE>
		static GENLIST_ITEM_TYPE *appendItem(Genlist &genlist);

		template <class GENLIST_ITEM_TYPE>
		static GENLIST_ITEM_TYPE *prependItem(Genlist &genlist);

		template <class GENLIST_ITEM_TYPE>
		static GENLIST_ITEM_TYPE *insertAfter(Genlist &genlist, GenlistItem &afterItem);

		template <class GENLIST_ITEM_TYPE>
		static GENLIST_ITEM_TYPE *insertBefore(Genlist &genlist, GenlistItem &beforeItem);

		void update();

	private:
		friend Widget; // to be used in Widget::create

		Genlist() {}
		virtual ~Genlist() {}
		bool initialize(const Widget &parent);
		void onItemSelected(Evas_Object *obj, void *eventInfo);
		void onItemRealized(Evas_Object *obj, void *eventInfo);
		void onItemUnrealized(Evas_Object *obj, void *eventInfo);
	};

	template <class GENLIST_ITEM_TYPE>
	GENLIST_ITEM_TYPE *Genlist::appendItem(Genlist &genlist)
	{
		GENLIST_ITEM_TYPE *instance = WidgetItem::create<GENLIST_ITEM_TYPE>(genlist,
			[](Genlist &list, GenlistItem *listItem) -> Elm_Object_Item *{
				return elm_genlist_item_append(list.getEvasObject(),
						listItem->getItemClass(),
						listItem,
						NULL,
						ELM_GENLIST_ITEM_NONE,
						NULL,
						NULL);
			});

		return instance;
	}

	template <class GENLIST_ITEM_TYPE>
	GENLIST_ITEM_TYPE *Genlist::prependItem(Genlist &genlist)
	{
		GENLIST_ITEM_TYPE *instance = WidgetItem::create<GENLIST_ITEM_TYPE>(genlist,
			[](Genlist &list, GenlistItem *listItem) -> Elm_Object_Item *{
				return elm_genlist_item_prepend(list.getEvasObject(),
						listItem->getItemClass(),
						listItem,
						NULL,
						ELM_GENLIST_ITEM_NONE,
						NULL,
						NULL);
			});

		return instance;
	}

	template <class GENLIST_ITEM_TYPE>
	GENLIST_ITEM_TYPE *Genlist::insertAfter(Genlist &genlist, GenlistItem &afterItem)
	{
		GENLIST_ITEM_TYPE *instance = WidgetItem::create<GENLIST_ITEM_TYPE>(genlist, afterItem,
			[](Genlist &list, GenlistItem &afterItem, GenlistItem *listItem) -> Elm_Object_Item * {
				return elm_genlist_item_insert_after(list.getEvasObject(),
						listItem->getItemClass(),
						listItem,
						NULL,
						afterItem.getElmObjectItem(),
						ELM_GENLIST_ITEM_NONE,
						NULL,
						NULL);
			});

		return instance;
	}

	template <class GENLIST_ITEM_TYPE>
	GENLIST_ITEM_TYPE *Genlist::insertBefore(Genlist &genlist, GenlistItem &beforeItem)
	{
		GENLIST_ITEM_TYPE *instance = WidgetItem::create<GENLIST_ITEM_TYPE>(genlist, beforeItem,
			[](Genlist &list, GenlistItem &beforeItem, GenlistItem *listItem) -> Elm_Object_Item * {
				return elm_genlist_item_insert_after(list.getEvasObject(),
						listItem->getItemClass(),
						listItem,
						NULL,
						beforeItem.getElmObjectItem(),
						ELM_GENLIST_ITEM_NONE,
						NULL,
						NULL);
			});

		return instance;
	}
}

#endif /* GENLIST_H_ */
