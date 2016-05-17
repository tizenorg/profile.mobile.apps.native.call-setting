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

#ifndef _CONTROLLER_TEL_NUMBER_PICKER_H_
#define _CONTROLLER_TEL_NUMBER_PICKER_H_

#include "Common.h"
#include "appfw/AppControl.h"

namespace CallSettings { namespace Controller {
	class TelNumberPicker : private util::NonCopyable {
	public:
		typedef util::Delegate<void(bool)> ResultEventHandler;

		static TelNumberPicker *create();
		~TelNumberPicker();

		bool launch(ResultEventHandler resultHandler);
		void terminate();
		bool getTelNumber(std::string &telNumber);

	private:
		TelNumberPicker();
		bool initialize();
		void contactLauncherHandler(appfw::AppControl::AppLaunchResult result);

	private:
		appfw::AppControl *m_pAppControl;
		ResultEventHandler m_resultHandler;
	};
} }

#endif /* _CONTROLLER_TEL_NUMBER_PICKER_H_ */
