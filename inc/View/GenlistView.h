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

#include "gui/BaseView.h"
#include "gui/Genlist.h"
#include "gui/Layout.h"

namespace CallSettings { namespace View {

	/*
	 * @brief Represents Simple  view with genlist widget only.
	 * Also is possible to show no contents layout
	 */
	class GenlistView : public gui::BaseView {
	public:
		/*
		 * @brief Get instance of Genlist widget
		 * @return	instance of genlist widget
		 */
		gui::Genlist &getGenlist();

		/*
		 * @brief Show not contents layout widget
		 * @param[in]	mainText	main text in No content layout
		 * @param[in]	helpText	sub text in No content layout
		 */
		void showEmptyView(util::TString mainText, util::TString helpText);

		/*
		 * @brief Show genlist widget and hide no contents layout
		 */
		void showListView();

	protected:
		friend class gui::BaseView;
		GenlistView(gui::NaviItem *naviItem);
		virtual ~GenlistView();

		bool initialize();

	protected:
		gui::Genlist *m_pGenlist;
		gui::Layout *m_pNoContentLayout;
	};

}}

#endif /* _VIEW_GENLIST_VIEW_ */
