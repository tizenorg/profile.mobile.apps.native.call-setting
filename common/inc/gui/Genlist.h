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

#include "Widget.h"
#include "WidgetItem.h"
#include "GenlistItem.h"

namespace gui {
	/*
	 * @brief Represent Genlist widget
	 */
	class Genlist : public Widget {
	public:
		/*
		 * @brief Fabric function for append new List item to the end of list
		 * @return ListItem instance
		 */
		template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
		GENLIST_ITEM_TYPE *appendItem(TYPE_ARGS&&... args);

		/*
		 * @brief Fabric function for append new List item to the top of list
		 * @return ListItem instance
		 */
		template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
		GENLIST_ITEM_TYPE *prependItem(TYPE_ARGS&&... args);

		/*
		 * @brief Fabric function for append new List item after specific list
		 * @param[in]	afterItem	List item anchor after which new one will be added
		 * @return ListItem instance
		 */
		template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
		GENLIST_ITEM_TYPE *insertAfter(GenlistItem &afterItem, TYPE_ARGS&&... args);

		/*
		 * @brief Fabric function for append new List item before specific list
		 * @param[in]	afterItem	List item anchor after which new one will be added
		 * @return ListItem instance
		 */
		template <class GENLIST_ITEM_TYPE, typename... TYPE_ARGS>
		GENLIST_ITEM_TYPE *insertBefore(GenlistItem &beforeItem, TYPE_ARGS&&... args);

		/*
		 * @briefUpdate genlist realized content
		 */
		void update();

		/*
		 * @brief Clear genlist content. All list items will be deleted
		 */
		void clear();

		/*
		 * @brief Set Homogeneous mode
		 * @param[in] isHomogenModeEnabled	Enable homogenus mode so all items will have the same height, otherwise - disable homogenus mode
		 * @note If list contains multiline items behavior will be undefined
		 */
		void setHomogeneousMode(bool isHomogenModeEnabled);
		void setContentMinLimit(bool w, bool h);

	private:
		friend Widget; // to be used in Widget::create

		Genlist() {}
		virtual ~Genlist() {}
		/*
		 * @brief Initialization of Genlist widget
		 * @param[in]	parent	Genlist widget parent
		 * @return true on success, otherwise false
		 */
		bool initialize(const Widget &parent);

		/*
		 * @brief Genlist item select callback
		 * @param[in]	obj			Genlist object
		 * @param[in]	eventInfo	Callback event info data
		 */
		void onItemSelected(Evas_Object *obj, void *eventInfo);

		/*
		 * @brief Pointer converter function
		 * @param[in]	eventInfo	void data pointer
		 * @return	Pointer to ListItem instance
		 */
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
