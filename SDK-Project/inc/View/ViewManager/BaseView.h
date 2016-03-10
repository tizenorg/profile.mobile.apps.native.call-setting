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

#ifndef BASE_VIEW_H_
#define BASE_VIEW_H_

#include "View/Widgets/Layout.h"
#include "View/Widgets/NaviItem.h"
#include "Utils/Common.h"

namespace View {
	using namespace Widgets;

	class BaseView;
	typedef Delegate<void(BaseView *)> ViewDestroyHandler;

	class BaseView {

	public:
		template <class ViewClass>
		static ViewClass *create(NaviItem *naviItem);
		static void destroy(BaseView *view);
		void setDestroyHandler(ViewDestroyHandler handler);

	protected:
		BaseView(Widgets::NaviItem *naviItem);
		virtual ~BaseView();
		virtual bool createViewContent() { return false; }
		bool initialize();
		bool setViewContent(Widget &content);

	protected:
		Layout *m_pViewLayout;
		NaviItem *m_pNaviItem;
		ViewDestroyHandler m_destroyHandler;
		bool m_isDestroying;

	private:
		BaseView() = delete;
		bool createBaseLayout();
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

#endif /* BASE_VIEW_H_ */
