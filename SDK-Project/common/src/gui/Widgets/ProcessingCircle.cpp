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

#include "gui/Widgets/ProcessingCircle.h"

namespace gui {

	bool ProcessingCircle::initialize(const Widget &parent, ProcessingType type)
	{
		Evas_Object *eoParent = parent.getEvasObject();
		RETVM_IF(!eoParent, false, "Failed to create processing item: Parent in null");

		m_pEvasObject = elm_progressbar_add(eoParent);
		elm_progressbar_pulse(m_pEvasObject, EINA_TRUE);
		RETVM_IF(!m_pEvasObject, false, "Failed to create processing item: Internal error");

		setStyle(type);
		return true;
	}

	void ProcessingCircle::setStyle(ProcessingType type)
	{

		switch(type) {
		case PROCESSING_TYPE_SMALL:
			elm_object_style_set(m_pEvasObject, "process_small");
			break;
		case PROCESSING_TYPE_MID:
			elm_object_style_set(m_pEvasObject, "process_medium");
			break;
		case PROCESSING_TYPE_LARGE:
			elm_object_style_set(m_pEvasObject, "process_large");
			break;
		}
	}
}
