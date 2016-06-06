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


#ifndef _GUI_OPTIONMENUPOPUP_H_
#define _GUI_OPTIONMENUPOPUP_H_

#include "Widget.h"

namespace gui {

	class OptionMenuPopup : public Widget {
	public:
		bool addItem(NotiHandler optionSelectHandler, const util::TString &text, const Widget *icon = nullptr);
		void show();
		void hide();

	protected:
		OptionMenuPopup() {}
		~OptionMenuPopup() {}
		bool initialize(const Widget &parent);
		void onItemSelected(Evas_Object *obj, void *eventInfo);
		void onPopupDismissed(Evas_Object *obj, void *eventInfo);

	private:
		friend class Widget;
		std::vector<std::pair<Elm_Object_Item *, NotiHandler>> m_itemSelectHandlers;
	};
}
#endif /* OPTIONMENUPOPUP_H_ */
