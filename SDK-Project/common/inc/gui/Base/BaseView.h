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

#ifndef _GUI_BASE_VIEW_H_
#define _GUI_BASE_VIEW_H_

#include "gui/GuiTypes.h"
#include "gui/Widgets/Layout.h"
#include "gui/Widgets/NaviItem.h"

namespace gui {

	class BaseView {
	public:
		template <class ViewClass>
		static ViewClass *create(NaviItem *naviItem);
		static void destroy(BaseView *view);

		void setDestroyHandler(ViewDestroyHandler handler);

		void enableInputEvents();
		void disableInputEvents();

		const Widget &getContent() const;
		NaviItem &getItem();

	protected:
		BaseView(NaviItem *naviItem);
		virtual ~BaseView();
		bool initialize();
		bool setViewContent(Widget &content);

	protected:
		NaviItem *m_pNaviItem;
		Layout *m_pViewLayout;
		ViewDestroyHandler m_destroyHandler;
		bool m_isDestroying;

	private:
		void onViewLayoutDestroy();
		void onNaviItemDestroy();
	};

	template <class ViewClass>
	ViewClass *BaseView::create(NaviItem *naviItem)
	{
		RETVM_IF(!naviItem, nullptr, "Invalid arguments!");

		ViewClass *viewInstance = new ViewClass(naviItem);

		if (!viewInstance->initialize()) {
			ERR("Failed to create View instance");
			delete viewInstance;
			return nullptr;
		}

		return viewInstance;
	}
}

#endif /* _GUI_BASE_VIEW_H_ */
