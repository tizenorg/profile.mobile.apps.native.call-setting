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

#ifndef _GUI_SIMPLE_LIST_ITEMS_H_
#define _GUI_SIMPLE_LIST_ITEMS_H_

#include "GenlistItem.h"

namespace gui {

	class SimpleListItem : public GenlistItem {
	public:
		void setText(util::TString text);
		void setTextFormat(util::TString textFormat);
	protected:
		SimpleListItem() {}
		virtual ~SimpleListItem() {}
		bool initialize(ItemAddMethod createItem, util::TString text = "",
				ItemSelectionMode selectMode = GENLIST_ITEM_SELECT_MODE_ALWAYS);
		virtual char *getText(const char *part) override;

	protected:
		util::TString m_text;
		util::TString m_textFormat;

	private:
		friend class WidgetItem;

	};
}

#endif /* _GUI_SIMPLE_LIST_ITEMS_H_ */
