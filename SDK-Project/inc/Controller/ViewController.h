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
	using namespace App;
	using namespace View;

	class ViewController : private NonCopyable {

	public:
		typedef Delegate<void(ViewController *)> DestroyRequestHandler;
		template <class ViewController>
		static ViewController *create(App::AppCore &core, DestroyRequestHandler handler);
		virtual ~ViewController();
		void makeDestroyReqeuest();

	protected:
		typedef enum {
			UPDATE_LANGUAGE = 1,
			UPDATE_REGION_FORMAT = 2,
			UPDATE_ORIENTATION = 4,
		} UpdateFlag;

		ViewController (AppCore &core, DestroyRequestHandler destroyRequestHandler);
		virtual bool createView() { return true;}
		virtual void onShow() {}
		virtual void onHide() {}
		virtual void onUpdate(int updateFlag) {}
		virtual void onActivate() {}
		virtual void onDeactivate() {}
		virtual void onMenuKeyPressed() {}
		virtual void onBackKeyPressed() {}

	protected:
		App::AppCore &m_Core;
		DestroyRequestHandler m_destroyRequestHandler;
		View::BaseView *m_pView;

	private:
		bool m_isActivated;
		bool m_isVisible;
		bool m_isDestroying;
		int m_updateFlag;

	private:
		bool initialize();
		void finalize();
		void onSystemEvent(SystemEvent event);
		void onPause();
		void onResume();

		void onViewEvent(ViewEvent event);
		void onViewChangeBegin();
		void onViewChangeEnd();

		void doUpdate();
		void doActivate();
		void doDeactivate();
		void doShow();
		void doHide();
	};

	template <class CONTROLLER_TYPE>
	CONTROLLER_TYPE *create(AppCore &core, ViewController::DestroyRequestHandler destroyRequestHandler)
	{
		CONTROLLER_TYPE *instance = new CONTROLLER_TYPE(core, destroyRequestHandler);
		if (!instance->initialize()) {
			delete instance;
			return nullptr;
		}

		return instance;
	}
}

#endif /* VIEW_CONTROLLER_H_ */
