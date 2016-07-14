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


#ifndef _GUI_OPTION_MENU_POPUP_H_
#define _GUI_OPTION_MENU_POPUP_H_

#include "Widget.h"

namespace gui {
	/*
	 * @brief Represents More menu option popup widget
	 */
	class OptionMenuPopup : public Widget {
	public:
		/*
		 * @brief insert new list item into more options menu
		 * @param[in]	optionSelectHandler		Select option handler
		 * @param[in]	text					Option item text
		 * @param[in]	icon					Widget which represents option icon
		 * @return true on success, otherwise false
		 */
		bool addItem(NotiHandler optionSelectHandler, const util::TString &text, const Widget *icon = nullptr);

		/*
		 * @brief Make visible more menu option popup
		 */
		void show();

		/*
		 * @brief Make invisible more menu option popup
		 */
		void hide();

	protected:
		OptionMenuPopup() {}
		~OptionMenuPopup() {}

		/*
		 * @brief Option popup initialization
		 * @param[in]	parent	Parent widget for opetion menu popup
		 * @return true on success, otherwise false
		 */
		bool initialize(const Widget &parent);

		/*
		 * @brief Option item select callback
		 * @param[in]	obj			Popup object
		 * @param[in]	eventInfo	Callback event info data
		 */
		void onItemSelected(Evas_Object *obj, void *eventInfo);

		/*
		 * @brief Option popup dissmiss callback
		 * @param[in]	obj			Popup object
		 * @param[in]	eventInfo	Callback event info data
		 */
		void onPopupDismissed(Evas_Object *obj, void *eventInfo);

	private:
		friend class Widget;
		std::vector<std::pair<Elm_Object_Item *, NotiHandler>> m_itemSelectHandlers;
	};
}
#endif /* _GUI_OPTION_MENU_POPUP_H_ */
