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

#ifndef _GUI_PROGRESS_BAR_H_
#define _GUI_PROGRESS_BAR_H_

#include "gui/Base/Widget.h"

namespace gui {

	typedef enum {
		PR_BAR_TYPE_CIRCLE_SMALL,
		PR_BAR_TYPE_CIRCLE_MID,
		PR_BAR_TYPE_CIRCLE_LARGE,
		PR_BAR_TYPE_HORIZONTAL_PENDING,
		PR_BAR_TYPE_HORIZONTAL,
	} ProgressbarType;

	class ProgressBar : public Widget {
	public:
		void setStyle(ProgressbarType type);
		void setText(const char *textPart, const char *text);
		bool setValue(double val);
		double getValue();

	protected:
		bool initialize(const Widget &parent, ProgressbarType type = PR_BAR_TYPE_CIRCLE_MID);

	private:
		friend Widget; // to be used in Widget::create

		ProgressBar() {}
		virtual ~ProgressBar() {}
	};
}

#endif /* _GUI_PROGRESS_BAR_H_ */
