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

#include "WidgetItem.h"

namespace gui {

	class Widget;
	/*
	 * @brief Represents NaviframeItem instance
	 */
	class NaviItem : public WidgetItem {
	public:
		virtual ~NaviItem();

		/*
		 * @brief Show Naviframe title for current page
		 */
		void showTitle();

		/*
		 * @brief Hide Naviframe title for current page
		 */
		void hideTitle();

		/*
		 * @brief Set Naviframe page title
		 * @param[in]	text	Title text
		 */
		void setTitleText(const util::TString &text);

		/*
		 * @brief Set Naviframe page content
		 * @param[in]	content		Naviframe page content widget
		 */
		void setContent(const Widget &content);

		/*
		 * @brief Unset Naviframe page content
		 */
		void unsetContent();

		/*
		 * @brief Set content for left title button part
		 * @param[in]	content		Content which will be placed into left title button part
		 */
		void setTitleLeftBtn(const Widget &content);

		/*
		 * @brief Unset content for left title button part
		 */
		void unsetTitleLeftBtn();

		/*
		 * @brief Set content for right title button part
		 * @param[in]	content		Content which will be placed into left title button part
		 */
		void setTitleRightBtn(const Widget &content);

		/*
		 * @brief Unset content for right title button part
		 */
		void unsetTitleRightBtn();

		/*
		 * @brief Make visible software back button in naviframe page title
		 */
		void showBackButton();

		/*
		 * @brief Make invisible software back button in naviframe page title
		 */
		void hideBackButton();

	private:
		friend class WidgetItem;

		NaviItem();
		bool initialize(Elm_Object_Item *item, Evas_Object *backButton);
		void setObjectToPart(Evas_Object *content, const char *part);
		void unsetObjectFromPart(const char *part);

	private:
		Evas_Object *m_pBackButton;

	};
}

#endif /* _GUI_NAVI_ITEM_H_ */
