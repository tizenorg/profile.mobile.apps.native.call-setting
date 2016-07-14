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

#ifndef _GUI_PROGRESSING_CIRCLE_H_
#define _GUI_PROGRESSING_CIRCLE_H_

#include "Widget.h"

namespace gui {
	/*
	 * @brief Processing circle widget type
	 */
	typedef enum {
		PROCESSING_TYPE_SMALL,
		PROCESSING_TYPE_MID,
		PROCESSING_TYPE_LARGE,
	} ProcessingType;

	/*
	 * @brief Represents Processing circle widget which used for GUI interpretation of heavy processes
	 */
	class ProcessingCircle : public Widget {
	public:
		/*
		 * @brief Set style for processing circle widget
		 * @param[in]	type	Circle widget type
		 */
		void setStyle(ProcessingType type);

	protected:
		/*
		 * @brief Processing circle widget initialization
		 * @param[in]	parent	Circle widget parent
		 * @param[in]	type	Circle widget type
		 * @return	true on success, otherwise false
		 */
		bool initialize(const Widget &parent, ProcessingType type = PROCESSING_TYPE_MID);

	private:
		friend Widget; // to be used in Widget::create

		ProcessingCircle() {}
		virtual ~ProcessingCircle() {}
	};
}

#endif /* _GUI_PROGRESSING_CIRCLE_H_ */
