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



#include "View/Widgets/OptionItem.h"

namespace Widgets {

	OptionItem::OptionItem()
	{
	}

	Elm_Genlist_Item_Class *OptionItem::getItemClass()
	{
		static Elm_Genlist_Item_Class itc = createItemClass("multiline");
		return &itc;
	}

	bool OptionItem::initialize(GenlistItem::ItemAddMethod createItem, const char *text, const char *subText,
			bool isTextLocalized, bool isSubTextLocalized)
	{
		return DoubleTextItem::initialize(createItem, text, subText, isTextLocalized, isSubTextLocalized);
	}

	char *OptionItem::getText(const char *part)
	{
		if (strcmp(part, "elm.text.multiline") == 0 && !m_subText.empty()) {
			return makeTextLabel(m_subText, m_isSubTextLocalized);
		} else {
			return DoubleTextItem::getText(part);
		}
	}

}
