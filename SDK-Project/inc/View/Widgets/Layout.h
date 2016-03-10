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

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "View/Widgets/Widget.h"

namespace View { namespace Widgets {
	class Layout : public Widget {
	private:
		friend Layout* Widget::create<Layout>(Widget *parent, const char *className, const char *groupName, const char *styleName);
		friend Layout* Widget::create<Layout>(Widget *parent, const char *edcFilePath, const char *groupName);

		Layout() {}
		virtual ~Layout() {}
		bool initialize(Widget *parent, const char *className, const char *groupName, const char *styleName);
		bool initialize(Widget *parent, const char *edcFilePath, const char *groupName);
		bool setContent(Widget *content, char *swallowPart);
		bool unsetContent(char *swallowPart);
		bool setText(const char *textPart, char *part);
	};
} }

#endif /* LAYOUT_H_ */
