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

#ifndef _GUI_NAVI_ITEM_H_
#define _GUI_NAVI_ITEM_H_

#include "gui/Base/WidgetItem.h"

namespace gui {

	class Widget;
	class NaviItem : public WidgetItem {
	public:
		virtual ~NaviItem();
		void showTitle();
		void hideTitle();
		void setTitleText(const char *title);

		void setContent(const Widget &content);
		void unsetContent();

		void setTitleLeftBtn(const Widget &content);
		void unsetTitleLeftBtn();

		void setTitleRightBtn(const Widget &content);
		void unsetTitleRightBtn();

		void showBackButton();
		void hideBackButton();

	private:
		friend WidgetItem;

		NaviItem();
		bool initialize(Elm_Object_Item *item, Evas_Object *backButton);
		void setObjectToPart(Evas_Object *content, const char *part);
		void unsetObjectFromPart(const char *part);

	private:
		Evas_Object *m_pBackButton;

	};
}

#endif /* _GUI_NAVI_ITEM_H_ */
