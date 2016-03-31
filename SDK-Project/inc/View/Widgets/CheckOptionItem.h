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

#ifndef CHECK_OPTION_ITEM_H
#define CHECK_OPTION_ITEM_H

#include "View/Widgets/OptionItem.h"
#include "View/Widgets/Checkbox.h"

namespace Widgets {

	class CheckOptionItem : public OptionItem {
	public:
		CheckOptionItem();
		virtual ~CheckOptionItem();
		void setCheckState(bool state);
		bool getCheckState();
		void setCheckHandler(NotifyHandler handler);

	protected:
		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) override;
		void onCheckChanged();
		void onCheckboxDestroyed();
		void onOptionSelected();

	protected:
		Checkbox *m_pCheckbox;
		NotifyHandler m_checkHandler;
		bool m_checkState;

	private:
		friend class WidgetItem;
	};
}

#endif /* CHECK_OPTION_ITEM_H */
