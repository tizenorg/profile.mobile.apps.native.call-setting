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

#include "gui/Widgets/CategoryItem.h"

namespace gui {

	CategoryItem::CategoryItem() :
		SimpleTextItem()
	{
	}

	Elm_Genlist_Item_Class *CategoryItem::getItemClass()
	{
		static Elm_Genlist_Item_Class itc = createItemClass("group_index");
		return &itc;
	}

	bool CategoryItem::initialize(ItemAddMethod createItem, const char *text, bool isTextLocalized, ItemSelectionMode selectMode)
	{
		if (!SimpleTextItem::initialize(createItem, text, isTextLocalized, selectMode)) {
			ERR("Failed to initialize TextItem");
			return false;
		}

		return true;
	}
}
