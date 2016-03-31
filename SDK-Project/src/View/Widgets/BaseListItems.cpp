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
		m_isTextLocalized(true)
	{
	}

	bool SimpleTextItem::initialize(ItemAddMethod createItem, const char *text, bool isLocalized, ItemSelectionMode selectMode)
	{
		if (text) {
			m_text.assign(text);
			m_isTextLocalized = isLocalized;
		}

		return GenlistItem::initialize(createItem, selectMode);
	}

	void SimpleTextItem::setText(const char *text, bool isLocalized)
	{
		RETM_IF(!text, "Invalid args!");

		m_text.assign(text);
		m_isTextLocalized = isLocalized;
		update("*", GL_PART_TYPE_TEXT);
	}

	char *SimpleTextItem::getText(const char *part)
	{
		if (strcmp(part, "elm.text") == 0) {
			return makeTextLabel(m_text, m_isTextLocalized);
		}

		return nullptr;
	}

	char *SimpleTextItem::makeTextLabel(const std::string &text, bool isLocalized)
	{
		if(text.empty()) {
			return nullptr;
		}

		if (isLocalized) {
			return strdup(_(text.c_str()));
		} else {
			return strdup(text.c_str());
		}
	}

	DoubleTextItem::DoubleTextItem() :
		m_subText(),
		m_isSubTextLocalized(false)
	{
	}

	bool DoubleTextItem::initialize(GenlistItem::ItemAddMethod createItem, const char *text, const char *subText,
				bool isTextLocalized, bool isSubTextLocalized, ItemSelectionMode selectMode)
	{
		if (subText) {
			m_subText.assign(subText);
			m_isSubTextLocalized = isSubTextLocalized;
		}

		return SimpleTextItem::initialize(createItem, text, isTextLocalized, selectMode);
	}
	void DoubleTextItem::setSubText(const char *subtext, bool isLocalized)
	{
		RETM_IF(!subtext, "Invalid args!");

		m_subText.assign(subtext);
		m_isSubTextLocalized = isLocalized;
		update("*", GL_PART_TYPE_TEXT);

	}
	char *DoubleTextItem::getText(const char *part)
	{
		if (strcmp(part, "elm.text.sub") == 0) {
			return makeTextLabel(m_subText, m_isSubTextLocalized);
		} else {
			return SimpleTextItem::getText(part);
		}
	}
}
