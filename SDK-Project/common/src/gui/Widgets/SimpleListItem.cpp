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

#include "gui/Widgets/SimpleListItem.h"

namespace gui {

	SimpleListItem::SimpleListItem() :
		m_isTextLocalized(true)
	{
	}

	bool SimpleListItem::initialize(ItemAddMethod createItem, const char *text, bool isLocalized, ItemSelectionMode selectMode)
	{
		if (text) {
			m_text.assign(text);
			m_isTextLocalized = isLocalized;
		}

		return GenlistItem::initialize(createItem, selectMode);
	}

	void SimpleListItem::setText(const char *text, bool isLocalized)
	{
		if (!text) {
			m_text.erase();
			m_isTextLocalized = false;
		} else {
			m_text.assign(text);
			m_isTextLocalized = isLocalized;
		}

		update("*", GL_PART_TYPE_TEXT);
	}

	char *SimpleListItem::getText(const char *part)
	{
		if (strcmp(part, "elm.text") == 0) {
			return makeTextLabel(m_text, m_isTextLocalized);
		}

		return nullptr;
	}

	char *SimpleListItem::makeTextLabel(const std::string &text, bool isLocalized)
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
}
