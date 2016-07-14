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
	/*
	 * @brief Represents Genlist item onject
	 */
	class GenlistItem : public WidgetItem {
	public:
		/*
		 * @brief List item part types
		 */
		typedef enum {
			GL_PART_TYPE_TEXT = 1,
			GL_PART_TYPE_CONTENT = 2,
			GL_PART_TYPE_STATE = 4,
		} GenlistPartType;

		/*
		 * @brief List item selection mode
		 */
		typedef enum {
			GENLIST_ITEM_SELECT_MODE_ONCE,		/**Item is selected only once, select callback calls only once*/
			GENLIST_ITEM_SELECT_MODE_ALWAYS,	/**Item selected callbacks will be called every time for click events*/
			GENLIST_ITEM_SELECT_MODE_NONE,		/**Item never be selected and no callback will be invoked*/
		} ItemSelectionMode;

		/*
		 * @brief Provides ItemClass name for current ListItem widget
		 * @return	Genlist item class pointer.
		 */
		virtual Elm_Genlist_Item_Class *getItemClass();

		/*
		 * @brief Set select event handler
		 * @param[in]	handler		Event handler
		 */
		void setSelectHandler(ItemNotiHandler handler);

		/*
		 * @brief Set selection mode
		 * @param[in]	mode	Selection mode
		 */
		void setSelectionMode(ItemSelectionMode mode);

	protected:
		/*
		 * @brief Function wrapper type for item insertion method
		 */
		typedef std::function<Elm_Object_Item *(GenlistItem *item)> ItemAddMethod;

	protected:
		GenlistItem();
		virtual ~GenlistItem();

		/*
		 * @brief Initialize List item
		 * param[in]	createItem			Function wrapper for insetion List item into list. Is provided by Genlist class
		 * param[in]	itemSelectMode		Item selection mode
		 * @return	true on success, otherwise false
		 */
		bool initialize(ItemAddMethod createItem, ItemSelectionMode itemSelectMode = GENLIST_ITEM_SELECT_MODE_ALWAYS);

		/*
		 * @brief List item select callback
		 */
		virtual void onSelected();

		/*
		 * @brief Callback called when list item will be displayed on screen. List item text parts will be set here
		 * @param[in]	genlist		Genlist object
		 * @param[in]	part		Part name of list item
		 * @return	Allocated text string
		 */
		virtual char *getText(const char *part) {return nullptr;}

		/* @brief Callback called when list item will be displayed on screen. List item content will be created here
		 * @param[in]	genlist		Genlist object
		 * @param[in]	part		Part name of list item
		 * @return	Widget which will be placed in mentioned part in list item
		 */
		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) {return nullptr;}

		/*
		 * @brief Callback for getting List item state
		 * @param[in]	part	part name
		 * @return List item state
		 */
		virtual Eina_Bool getState(const char *part) {return EINA_FALSE;}

		/*
		 * @brief Create new item class
		 * @param[in]	style	Item class name
		 * @return	New item class instance
		 */
		static Elm_Genlist_Item_Class createItemClass(const char *style = "default");

		/*
		 * @brief Update list item. All item content will be recreated and getContent() getText() callbacks will be invoked
		 */
		void update();

		/*
		 * @brief Update list item part.
		 * @param[in]	parts		Parts name, symbol * may used to determine part name template
		 * @param[in]	updateFlag	Flags to determine parts type @see GenlistPartType
		 */
		void update(const char *parts, int updateFlag);

	private:
		friend class Genlist;
		static GenlistItem *fromData(void *data);

	private:
		ItemNotiHandler m_selectHandler;
	};
}
#endif /* _GUI_GENLIST_ITEM_H_ */
