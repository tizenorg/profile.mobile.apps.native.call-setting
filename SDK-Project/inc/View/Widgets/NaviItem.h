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

#ifndef NAVI_ITEM_H_
#define NAVI_ITEM_H_

#include "View/Widgets/WidgetItem.h"

namespace View { namespace Widgets {
	class Widget;
	class NaviItem : public WidgetItem {
	public:
		virtual ~NaviItem() {};
		void showTitle();
		void hideTitle();
		void setTitleText(const char *title);
		void setContent(Widget *content);
	private:
		friend NaviItem *WidgetItem::create<NaviItem>(Elm_Object_Item *item);
		bool initialize(Elm_Object_Item *item);

	};
} }

#endif /* NAVI_ITEM_H_ */
