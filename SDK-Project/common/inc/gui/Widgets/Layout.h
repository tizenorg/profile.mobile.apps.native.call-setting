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

#ifndef _GUI_LAYOUT_H_
#define _GUI_LAYOUT_H_

#include "gui/Base/Widget.h"

namespace gui {
	class Layout : public Widget {

	public:
		bool setContent(const char *swallowPart, const Widget &content);
		bool unsetContent(const char *swallowPart);
		bool setText(const char *textPart, const char *text);
		bool themeSet(const char *className, const char *groupName, const char *styleName);
		bool styleSet(const char *edjFilePath, const char *groupName);

	private:
		friend class Widget; // to be used in Widget::create
		Layout() {}
		virtual ~Layout() {}
		bool initialize(const Widget &parent, const char *className, const char *groupName, const char *styleName);
		bool initialize(const Widget &parent, const char *edjFilePath, const char *groupName);
	};
}

#endif /* _GUI_LAYOUT_H_ */
