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

#ifndef _VIEW_GENLIST_VIEW_
#define _VIEW_GENLIST_VIEW_

#include "gui/Base/BaseView.h"
#include "gui/Widgets/Genlist.h"

namespace CallSettings { namespace View {

	class GenlistView : public gui::BaseView {
	public:
		gui::Genlist &getGenlist();

	protected:
		friend class gui::BaseView;
		GenlistView(gui::NaviItem *naviItem);
		virtual ~GenlistView();

		bool initialize();

	protected:
		gui::Genlist *m_pGenlist;
	};

}}

#endif /* _VIEW_GENLIST_VIEW_ */
