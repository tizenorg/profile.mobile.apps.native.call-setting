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

#include <functional>

#include "View/Widgets/Widget.h"
#include "View/Widgets/Layout.h"
#include "View/Widgets/NaviItem.h"
#include "View/ViewManager/ViewManager.h"

namespace View {
	class BaseView {
	public:
		template <class ViewClass>
		static ViewClass *create(Widgets::NaviItem *naviItem);

		static void destroy(BaseView *view);
		void onNaviItemDestroy();
		virtual void show() {};
		virtual void hide() {};
		virtual void activate() {};
		virtual void deactivate() {};
		void onMenuKeyPressed();
		void onBackKeyPressed();
		bool setMenuKeyPressedCb(std::function<void()> callback);
		bool setBackKeyPressedCb(std::function<void()> callback);
	protected:
		virtual bool createViewContent() = 0;
		virtual ~BaseView();
	private:
		BaseView() = delete;
		BaseView(Widgets::NaviItem *naviItem);
		bool createBaseLayout();
		void onViewLayoutDestroy();
	private:
		Widgets::Layout *m_pViewLayout;
		Widgets::NaviItem *m_pNaviItem;
		std::function<void()> m_OnMenuPressedCb;
		std::function<void()> m_OnBackPressedCb;
	};

}

#endif /* BASE_VIEW_H_ */
