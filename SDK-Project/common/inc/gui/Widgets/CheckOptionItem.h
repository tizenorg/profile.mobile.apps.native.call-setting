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

#ifndef _GUI_CHECK_OPTION_ITEM_H_
#define _GUI_CHECK_OPTION_ITEM_H_

#include "OptionItem.h"
#include "Checkbox.h"
#include "ProcessingCircle.h"

namespace gui {

	class CheckOptionItem : public OptionItem {
	public:
		CheckOptionItem();
		virtual ~CheckOptionItem();
		void setCheckState(bool state);
		bool getCheckState();
		void setCheckHandler(NotiHandler handler);
		void setCheckPendingState(bool pendingState);
		bool getCheckPendingState();

	protected:
		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) override;
		void onCheckChanged();
		void onCheckboxDestroyed();
		void onOptionSelected();

	protected:
		Checkbox *m_pCheckbox;
		NotiHandler m_checkHandler;

		bool m_checkState;
		bool m_checkPending;

	private:
		friend class WidgetItem;
	};
}

#endif /* _GUI_CHECK_OPTION_ITEM_H_ */
