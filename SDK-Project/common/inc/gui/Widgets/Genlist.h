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

#ifndef _GUI_GENLIST_H_
#define _GUI_GENLIST_H_

#include "gui/Base/Widget.h"
#include "gui/Base/WidgetItem.h"
#include "gui/Widgets/GenlistItem.h"

namespace gui {
	class Genlist : public Widget {
	public:
		template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
		GENLIST_ITEM_TYPE *appendItem(TYPE_ARGS&&... args);

		template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
		GENLIST_ITEM_TYPE *prependItem(TYPE_ARGS&&... args);

		template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
		GENLIST_ITEM_TYPE *insertAfter(GenlistItem &afterItem, TYPE_ARGS&&... args);

		template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
		GENLIST_ITEM_TYPE *insertBefore(GenlistItem &beforeItem, TYPE_ARGS&&... args);

		void update();
		void clear();

		void setHomogeneousMode(bool isHomogenModeEnabled);
		void setContentMinLimit(bool w, bool h);

	private:
		friend Widget; // to be used in Widget::create

		Genlist() {}
		virtual ~Genlist() {}
		bool initialize(const Widget &parent);
		void onItemSelected(Evas_Object *obj, void *eventInfo);
		void onItemRealized(Evas_Object *obj, void *eventInfo);
		void onItemUnrealized(Evas_Object *obj, void *eventInfo);
		static GenlistItem *toGenlistItem(void *eventInfo);
	};

	template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
	GENLIST_ITEM_TYPE *Genlist::appendItem(TYPE_ARGS&&... args)
	{
		GENLIST_ITEM_TYPE *instance = WidgetItem::create<GENLIST_ITEM_TYPE>(
			[this](GenlistItem *listItem) -> Elm_Object_Item *{
				RETVM_IF(!listItem, nullptr, "Invalid data set!");
				return elm_genlist_item_append(m_pEvasObject,
						listItem->getItemClass(),
						static_cast<WidgetItem *>(listItem),
						NULL,
						ELM_GENLIST_ITEM_NONE,
						NULL,
						NULL);
			},
			std::forward<TYPE_ARGS>(args)...);

		return instance;
	}

	template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
	GENLIST_ITEM_TYPE *Genlist::prependItem(TYPE_ARGS&&... args)
	{
		GENLIST_ITEM_TYPE *instance = WidgetItem::create<GENLIST_ITEM_TYPE>(
			[this](GenlistItem *listItem) -> Elm_Object_Item *{
				RETVM_IF(!listItem, nullptr, "Invalid data set!");
				return elm_genlist_item_prepend(m_pEvasObject,
						listItem->getItemClass(),
						static_cast<WidgetItem *>(listItem),
						NULL,
						ELM_GENLIST_ITEM_NONE,
						NULL,
						NULL);
			},
			std::forward<TYPE_ARGS>(args)...);

		return instance;
	}

	template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
	GENLIST_ITEM_TYPE *Genlist::insertAfter(GenlistItem &afterItem, TYPE_ARGS&&... args)
	{
		GENLIST_ITEM_TYPE *instance = WidgetItem::create<GENLIST_ITEM_TYPE>(
			[this, &afterItem](GenlistItem *listItem) -> Elm_Object_Item * {
				RETVM_IF(!listItem, nullptr, "Invalid data set!");
				return elm_genlist_item_insert_after(m_pEvasObject,
						listItem->getItemClass(),
						static_cast<WidgetItem *>(listItem),
						NULL,
						afterItem.getElmObjectItem(),
						ELM_GENLIST_ITEM_NONE,
						NULL,
						NULL);
			},
			std::forward<TYPE_ARGS>(args)...);

		return instance;
	}

	template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
	GENLIST_ITEM_TYPE *Genlist::insertBefore(GenlistItem &beforeItem, TYPE_ARGS&&... args)
	{
		GENLIST_ITEM_TYPE *instance = WidgetItem::create<GENLIST_ITEM_TYPE>(
			[this, &beforeItem](GenlistItem *listItem) -> Elm_Object_Item * {
				RETVM_IF(!listItem, nullptr, "Invalid data set!");
				return elm_genlist_item_insert_before(m_pEvasObject,
						listItem->getItemClass(),
						static_cast<WidgetItem *>(listItem),
						NULL,
						beforeItem.getElmObjectItem(),
						ELM_GENLIST_ITEM_NONE,
						NULL,
						NULL);
			},
			std::forward<TYPE_ARGS>(args)...);

		return instance;
	}
}

#endif /* _GUI_GENLIST_H_ */
