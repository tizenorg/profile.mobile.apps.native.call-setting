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

#include "Widget.h"

namespace gui {
	/*
	 * @brief Represents Layout object instance
	 */
	class Layout : public Widget {

	public:
		/*
		 * @brief Set widget as content of layout
		 * @param[in]	swallowPart		Part name
		 * @param[in]	content			Widget content instance
		 * @return true on success, otherwise false
		 */
		bool setContent(const char *swallowPart, const Widget &content);

		/*
		 * @brief Unset widget from layout widget
		 * @param[in]	swallowPart		Part name
		 * @return true on success, otherwise false
		 */
		bool unsetContent(const char *swallowPart);

		/*
		 * @brief Setup theme for layout widget
		 * @param[in]	className		Theme class name
		 * @param[in]	groupName		Theme group name
		 * @param[in]	styleName		Theme style name
		 * @return true on success, otherwise false
		 */
		bool themeSet(const char *className, const char *groupName, const char *styleName);

		/*
		 * @brief Setup style for layout widget
		 * @param[in]	edjFilePath		Path to edj file
		 * @param[in]	groupName		Group name
		 * @return true on success, otherwise false
		 */
		bool styleSet(const char *edjFilePath, const char *groupName);

		/*
		 * @brief Emit signal for layout content
		 * @param[in]	signal		Signal name
		 * @param[in]	source		Source name
		 */
		void emitSignal(const char *signal, const char *source);

	private:
		friend class Widget; // to be used in Widget::create
		Layout() {}
		virtual ~Layout() {}

		/*
		 * @brief Layout widget initialization
		 * @param[in]	className		Theme class name
		 * @param[in]	groupName		Theme group name
		 * @param[in]	styleName		Theme style name
		 * @return true on success, otherwise false
		 */
		bool initialize(const Widget &parent, const char *className, const char *groupName, const char *styleName);

		/*
		 * @brief Layout widget initialization
		 * @param[in]	edjFilePath		Path to edj file
		 * @param[in]	groupName		Group name
		 * @return true on success, otherwise false
		 */
		bool initialize(const Widget &parent, const char *edjFilePath, const char *groupName);
	};
}

#endif /* _GUI_LAYOUT_H_ */
