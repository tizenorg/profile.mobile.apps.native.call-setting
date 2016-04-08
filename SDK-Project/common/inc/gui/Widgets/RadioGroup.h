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

#ifndef _GUI_RADIO_POPUP_H_
#define _GUI_RADIO_POPUP_H_

#include "gui/Widgets/Genlist.h"

namespace gui {

	typedef util::Delegate <void(int itemIndex)> RadioSelectHandler;
	class RadioGroup : public Widget {
	public:
		void selectRadioItem(int itemIndex);
		void setSelectedCallback(RadioSelectHandler handler) {m_radioSelectHandler = handler;}
		bool addItem(const char *text, int itemIndex, bool isTranslatable = true);

	protected:
		RadioGroup();
		~RadioGroup() {}
		bool initialize(const Widget &parent);
		bool createRadioList();
		void onRadioItemSelected(gui::WidgetItem *item);

	private:
		friend class Widget;

		Genlist *m_pGenlist;
		Evas_Object *m_pRadioGroup;
		RadioSelectHandler m_radioSelectHandler;
	};

}
#endif /* _GUI_LIST_POPUP_H_ */
