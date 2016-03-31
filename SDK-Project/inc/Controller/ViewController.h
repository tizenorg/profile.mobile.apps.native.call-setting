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

#ifndef VIEW_CONTROLLER_H_
#define VIEW_CONTROLLER_H_

#include "App/AppCore.h"
#include "Utils/Common.h"

namespace Controller {

	class ViewController : private NonCopyable {
	public:
		template <typename CONTROLLER_T, typename APP_T, typename ...TYPE_ARGS>
		static CONTROLLER_T *create(APP_T &app, NotifyHandler handler, TYPE_ARGS&&... args);

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

		ViewController (App::AppCore &core, NotifyHandler destroyHandler);
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void updateView(int updateFlag) {}
		virtual void onActivate() {}
		virtual void onDeactivate() {}
		virtual void onMenuKeyPressed() {}
		virtual void onBackKeyPressed() {}
		bool initialize();
		void setBaseView(View::BaseView *view);

	protected:
		App::AppCore &m_Core;
		NotifyHandler m_destroyRequestHandler;
		bool m_isActivated;
		bool m_isVisible;
		bool m_isDestroying;
		int m_updateFlag;

	private:
		void onSystemEvent(App::SystemEvent event);
		void onViewEvent(View::ViewEvent event);

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
		View::BaseView *m_pView;

	};

	template <typename CONTROLLER_T, typename APP_T, typename ...TYPE_ARGS>
	CONTROLLER_T *ViewController::create(APP_T &app, NotifyHandler handler, TYPE_ARGS&&... args)
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

#endif /* VIEW_CONTROLLER_H_ */
