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

#include "gui/SimpleListItem.h"

namespace gui {

	bool SimpleListItem::initialize(ItemAddMethod createItem, util::TString text, ItemSelectionMode selectMode)
	{
		m_text = std::move(text);

		return GenlistItem::initialize(createItem, selectMode);
	}

	void SimpleListItem::setText(util::TString text)
	{
		m_text = std::move(text);

		if (m_text.isEmpty()) {
			update();
		} else {
			update("*", GL_PART_TYPE_TEXT);
		}
	}

	void SimpleListItem::setTextFormat(util::TString textFormat)
	{
		m_textFormat = std::move(textFormat);
		update("*", GL_PART_TYPE_TEXT);
	}

	char *SimpleListItem::getText(const char *part)
	{
		if (strcmp(part, "elm.text") == 0) {
			if(m_textFormat.isNotEmpty() && m_text.isNotEmpty()) {
				return util::strDupSafe(m_textFormat.format(m_text.translate()));
			}
			return util::strDupSafe(util::ne(m_text.translate()));
		}

		return nullptr;
	}

}
