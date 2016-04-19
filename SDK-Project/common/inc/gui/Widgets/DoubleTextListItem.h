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
	class DoubleTextListItem : public CheckboxListItem {
	public:
		void setSubText(const char *subtext, bool isLocalized = true);

	protected:
		DoubleTextListItem();
		virtual ~DoubleTextListItem() {}
		bool initialize(ItemAddMethod createItem, const char *text = nullptr, const char *subText = nullptr,
				bool isTextLocalized = true, bool isSubTextLocalized = true, bool m_isMultiline = false, ItemSelectionMode selectMode = GENLIST_ITEM_SELECT_MODE_ALWAYS);
		virtual Elm_Genlist_Item_Class *getItemClass() override;
		virtual char *getText(const char *part) override;

	protected:
		std::string m_subText;
		bool m_isSubTextLocalized;
		bool m_isMultiline;

	private:
		friend class WidgetItem;
	};

}
#endif /* _GUI_DOUBLE_TEXT_LISTITEM_H_ */
