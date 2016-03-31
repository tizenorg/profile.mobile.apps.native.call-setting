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

#include "View/Widgets/BaseListItems.h"

namespace Widgets {

	SimpleTextItem::SimpleTextItem() :
		m_text(),
		m_isTextLocalized(true)
	{
	}

	bool SimpleTextItem::initialize(ItemAddMethod createItem, const char *text, bool isLocalized)
	{
		if (text) {
			m_text.assign(text);
			m_isTextLocalized = isLocalized;
		}

		m_pEOItem = createItem(this);
		if (!m_pEOItem) {
			return false;
		}

		return GenlistItem::initialize();
	}

	void SimpleTextItem::setText(const char *text, bool isLocalized)
	{
		m_text.assign(text);
		m_isTextLocalized = isLocalized;
		update("*", GENLIST_ITEM_FIELD_TEXT);
	}

	char *SimpleTextItem::getText(const char *part)
	{
		if (strcmp(part, "elm.text") == 0 && !m_text.empty()) {
			if (m_isTextLocalized) {
				return strdup(_(m_text.c_str()));
			} else {
				return strdup(m_text.c_str());
			}
		}

		return nullptr;
	}

	char *SimpleTextItem::makeTextLabel(std::string &text, bool isLocalized)
	{
		if (isLocalized) {
			return strdup(_(text.c_str()));
		} else {
			return strdup(text.c_str());
		}
	}

	TwoLineTextItem::TwoLineTextItem() :
		SimpleTextItem(),
		m_subText(),
		m_isSubTextLocalized(false)
	{
	}

	bool TwoLineTextItem::initialize(GenlistItem::ItemAddMethod createItem, const char *text, const char *subText,
				bool isTextLocalized, bool isSubTextLocalized)
	{
		if (subText) {
			m_subText.assign(subText);
			m_isSubTextLocalized = isSubTextLocalized;
		}

		return SimpleTextItem::initialize(createItem, text, isTextLocalized);
	}
	void TwoLineTextItem::setSubText(const char *subtext, bool isLocalized)
	{
		m_subText.assign(subtext);
		m_isSubTextLocalized = isLocalized;
		update("*", GENLIST_ITEM_FIELD_TEXT);

	}
	char *TwoLineTextItem::getText(const char *part)
	{
		if (strcmp(part, "elm.text") == 0 && !m_text.empty()) {
			return makeTextLabel(m_text, m_isTextLocalized);
		} else if (strcmp(part, "elm.text.sub") == 0 && !m_subText.empty()) {
			return makeTextLabel(m_subText, m_isSubTextLocalized);
		}

		return nullptr;
	}
}
