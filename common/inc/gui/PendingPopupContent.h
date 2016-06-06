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

#ifndef _GUI_PENDING_POPUP_CONTENT_H_
#define _GUI_PENDING_POPUP_CONTENT_H_

#include "Widget.h"
#include "Layout.h"
#include "ProcessingCircle.h"

namespace gui {

	class PendingPopupContent : public Widget {
	public:
		void setLabel(const util::TString &label);
		void setStyle(ProcessingType type);

	protected:
		bool initialize(const Widget &parent, const util::TString &label, ProcessingType type = PROCESSING_TYPE_MID);

	private:
		friend Widget; // to be used in Widget::create
		PendingPopupContent();
		virtual ~PendingPopupContent();

	private:
		Layout *m_pLayout;
		ProcessingCircle *m_pProcessing;
	};
}

#endif /* _GUI_PENDING_POPUP_CONTENT_H_ */
