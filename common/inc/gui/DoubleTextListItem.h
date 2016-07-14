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

#ifndef _GUI_DOUBLE_TEXT_LISTITEM_H_
#define _GUI_DOUBLE_TEXT_LISTITEM_H_

#include "CheckboxListItem.h"
namespace gui {
	/*
	 * @brief Represent Genlist item with double text lines and checkbox widget
	 */
	class DoubleTextListItem : public CheckboxListItem {
	public:
		/*
		 * @brief Set subline text
		 * @param[in]	subText		Text which will be set as subtext, may be translatable or not
		 */
		void setSubText(util::TString subText);

		/*
		 * @brief Set text format which will be applied for subtext
		 * @param[in]	subTextFormat		Text format for subtext
		 */
		void setSubTextFormat(util::TString subTextFormat);

	protected:
		DoubleTextListItem();
		virtual ~DoubleTextListItem() {}

		/*
		 * @brief Initialize List item
		 * @param[in]	text			Text which will be shown
		 * @param[in]	subText			SubText which will be shown
		 * @param[in]	isMultiline		If true subtext will be displayed fully in multiple lines, otherwise it will be shown in one line with ellipsis
		 * @param[in]	selectMode		Selection mode of list item
		 * @return	true on success, otherwise false
		 */
		bool initialize(ItemAddMethod createItem, util::TString text = "", util::TString subText = "",
				bool isMultiline = false, ItemSelectionMode selectMode = GENLIST_ITEM_SELECT_MODE_ALWAYS);
		/*
		 * @brief Provides ItemClass name for current ListItem widget
		 * @return	Genlist item class pointer.
		 */
		virtual Elm_Genlist_Item_Class *getItemClass() override;

		/*
		 * @brief Callback called when list item will be displayed on screen. List item text parts will be set here
		 * @param[in]	genlist		Genlist object
		 * @param[in]	part		Part name of list item
		 * @return	Allocated text string
		 */
		virtual char *getText(const char *part) override;

	protected:
		util::TString m_subText;
		util::TString m_subTextFormat;
		bool m_isMultiline;

	private:
		friend class WidgetItem;
	};

}
#endif /* _GUI_DOUBLE_TEXT_LISTITEM_H_ */
