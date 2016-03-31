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
#include "Utils/Common.h"

namespace Widgets {

	class SimpleTextItem : public GenlistItem {
	protected:
		SimpleTextItem();
		virtual ~SimpleTextItem() {}
		virtual char *getText(const char *part) override;
		void setText(const char *text, bool isLocalized = true);
		bool initialize(ItemAddMethod createItem, const char *text = nullptr, bool isLocalized = true);
		char *makeTextLabel(std::string &text, bool isLocalized);

	protected:
		std::string m_text;
		bool m_isTextLocalized;

	private:
		friend class WidgetItem;
	};


	class TwoLineTextItem : public SimpleTextItem {
	protected:
		TwoLineTextItem();
		virtual ~TwoLineTextItem() {}
		virtual char *getText(const char *part) override;
		void setSubText(const char *subtext, bool isLocalized = true);
		bool initialize(ItemAddMethod createItem, const char *text = nullptr, const char *subText = nullptr,
				bool isTextLocalized = true, bool isSubTextLocalized = true);

	protected:
			std::string m_subText;
			bool m_isSubTextLocalized;

	private:
			friend class WidgetItem;
		};
}

#endif /* BASE_LIST_ITEMS_H_ */
