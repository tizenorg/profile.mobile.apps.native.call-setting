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

#ifndef ANSWER_OPTIONS_H
#define ANSWER_OPTIONS_H

#include "View/Widgets/OptionItem.h"

namespace AnswerView {

	class AnswerOption : public Widgets::OptionItem {
		public:
			AnswerOption() {}
			virtual ~AnswerOption() {}

		private:
			virtual char *getText(const char *part);
		};

		class RejectOption : public Widgets::OptionItem {
		public:
			RejectOption() {}
			virtual ~RejectOption() {}

		private:
			virtual char *getText(const char *part);
		};
}

#endif /* ANSWER_OPTIONS_H */
