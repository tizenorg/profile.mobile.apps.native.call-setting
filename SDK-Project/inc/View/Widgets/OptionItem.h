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

#ifndef OPTION_ITEM_H
#define OPTION_ITEM_H

#include "View/Widgets/BaseListItems.h"

namespace Widgets {

	class OptionItem : public DoubleTextItem {
	protected:
		OptionItem();
		virtual ~OptionItem() {}
		virtual Elm_Genlist_Item_Class *getItemClass() override;
		virtual char *getText(const char *part) override;
		bool initialize(ItemAddMethod createItem, const char *text = nullptr, const char *subText = nullptr,
								bool isTextLocalized = true, bool isSubTextLocalized = true);

	private:
		friend class WidgetItem;
	};
}

#endif /* OPTION_ITEM_H */
