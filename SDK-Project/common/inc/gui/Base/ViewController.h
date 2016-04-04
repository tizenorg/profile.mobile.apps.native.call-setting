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

#ifndef _GUI_VIEW_CONTROLLER_H_
#define _GUI_VIEW_CONTROLLER_H_

#include "gui/GuiTypes.h"
#include "appfw/BaseApplication.h"

namespace gui {

	class ViewController : private util::NonCopyable {
	public:
		template <typename CONTROLLER_T, typename APP_T, typename ...TYPE_ARGS>
		static CONTROLLER_T *create(APP_T &app, NotiHandler handler, TYPE_ARGS&&... args);

		virtual ~ViewController();
		void makeDestroyReqeuest();

	protected:
		typedef enum {
			UF_INITIAL = 1,
			UF_LANGUAGE = 2,
			UF_REGION_FORMAT = 4,
			UF_ORIENTATION = 8,
			UF_WAS_PAUSED = 16,
		} UpdateFlag;



		ViewController (appfw::BaseApplication &app, NotiHandler destroyHandler);
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void updateView(int updateFlag) {}
		virtual void onActivate() {}
		virtual void onDeactivate() {}
		virtual void onMenuKeyPressed() {}
		virtual void onBackKeyPressed() {}
		bool initialize();
		void setBaseView(BaseView *view);

	protected:
		appfw::BaseApplication &m_Core;
		NotiHandler m_destroyRequestHandler;
		bool m_isActivated;
		bool m_isVisible;
		bool m_isDestroying;
		int m_updateFlag;

	private:
		void onSystemEvent(appfw::SystemEvent event);
		void onViewEvent(ViewEvent event);

		void handleViewChangeBegin();
		void handleViewChangeEnd();
		void handleMenuKeyPress();
		void handleBackKeyPress();
		void handlePause();
		void handleResume();

		void doUpdate();
		void doActivate();
		void doDeactivate();
		void doShow();
		void doHide();

	private:
		BaseView *m_pView;

	};

	template <typename CONTROLLER_T, typename APP_T, typename ...TYPE_ARGS>
	CONTROLLER_T *ViewController::create(APP_T &app, NotiHandler handler, TYPE_ARGS&&... args)
	{
		CONTROLLER_T *instance = new CONTROLLER_T(app, handler);
		if (!instance->initialize(std::forward<TYPE_ARGS>(args)...) && !instance->m_pView) {
			ERR("Failed to create controller instance!");
			delete instance;
			return nullptr;
		}

		return instance;
	}
}

#endif /* _GUI_VIEW_CONTROLLER_H_ */
