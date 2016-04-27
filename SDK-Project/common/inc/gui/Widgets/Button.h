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

#ifndef _GUI_BUTTON_H_
#define _GUI_BUTTON_H_

#include "gui/Base/Widget.h"

namespace gui {

	class Button : public Widget {
	public:
		void setClickHandler(NotiHandler handler);

	private:
		friend Widget; // to be used in Widget::create

		Button();
		virtual ~Button();
		bool initialize(const Widget &parent, const char *style, const util::TString &text);
		void onButtonClick(Evas_Object *obj, void *event_info);

	private:
		NotiHandler m_clickHandler;
	};

}

#endif /* _GUI_BUTTON_H_ */
