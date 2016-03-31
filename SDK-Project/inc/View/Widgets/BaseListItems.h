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

#ifndef BASE_LIST_ITEMS_H_
#define BASE_LIST_ITEMS_H_

#include <app_i18n.h>

#include "View/Widgets/GenlistItem.h"

namespace Widgets {

	class SimpleTextItem : public GenlistItem {
	public:
		void setText(const char *text, bool isLocalized = true);
	protected:
		SimpleTextItem();
		virtual ~SimpleTextItem() {}
		bool initialize(ItemAddMethod createItem, const char *text = nullptr, bool isLocalized = true,
				ItemSelectionMode selectMode = GENLIST_ITEM_SELECT_MODE_ALWAYS);
		virtual char *getText(const char *part) override;
		static char *makeTextLabel(const std::string &text, bool isLocalized);

	protected:
		std::string m_text;
		bool m_isTextLocalized;

	private:
		friend class WidgetItem;

	};


	class DoubleTextItem : public SimpleTextItem {
	public:
		void setSubText(const char *subtext, bool isLocalized = true);
	protected:
		DoubleTextItem();
		virtual ~DoubleTextItem() {}
		bool initialize(ItemAddMethod createItem, const char *text = nullptr, const char *subText = nullptr,
				bool isTextLocalized = true, bool isSubTextLocalized = true, ItemSelectionMode selectMode = GENLIST_ITEM_SELECT_MODE_ALWAYS);
		virtual char *getText(const char *part) override;

	protected:
			std::string m_subText;
			bool m_isSubTextLocalized;

	private:
			friend class WidgetItem;
		};
}

#endif /* BASE_LIST_ITEMS_H_ */
