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

#ifndef _GUI_SCROLLER_H_
#define _GUI_SCROLLER_H_

#include "gui/Base/Widget.h"

namespace gui {

	typedef enum {
		SCROLLBAR_POLICY_AUTO,
		SCROLLBAR_POLICY_ON,
		SCROLLBAR_POLICY_OFF
	} ScrollerPolicy;

	class Scroller : public Widget {
	public:
		void setContent(const Widget &content);
		void setScrollbarPolicy(ScrollerPolicy widthScroll, ScrollerPolicy heightScroll);
		void bringIn(int x, int y, int width, int height);
		void regionShow(int x, int y, int width, int height);
		void setBounceEffect(bool bounceHorizont, bool bounceVertical);
		static Elm_Scroller_Policy toElmSollerPolicy(ScrollerPolicy policy);

	private:
		friend Widget; // to be used in Widget::create

		Scroller() {};
		virtual ~Scroller() {};
		bool initialize(const Widget &parent);

	};
}

#endif /* _GUI_SCROLLER_H_ */
