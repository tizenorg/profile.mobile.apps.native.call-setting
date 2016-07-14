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

#ifndef _APPFW_PATH_H_
#define _APPFW_PATH_H_

#include "AppTypes.h"

namespace appfw {

	/*
	 * @return Get application resource directory path
	 */
	const std::string &getResourceDir();

	/**
	 * @return Localization files directory path
	 */
	const std::string &getLocaleDir();

	/**
	 * @brief Get resource file path.
	 * @param[in]	relativePath	File path relative to resource directory
	 * @return Full file path
	 */
	std::string getResourcePath(const std::string &relativePath);
}

#endif /* _APPFW_PATH_H_ */
