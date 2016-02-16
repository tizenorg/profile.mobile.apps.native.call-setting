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

#include <app_common.h>
#include <stdlib.h>
#include "Utils/Path.h"

const std::string &Utils::getLocaleDir()
{
	static std::string localeDir;
	if (localeDir.empty()) {
		localeDir = getResourcePath("locale/");
	}

	return localeDir;
}

const std::string &Utils::getResourceDir()
{
	static std::string resDir;
	if (resDir.empty()) {
		char *path = app_get_resource_path();
		resDir = path;
		free(path);
	}

	return resDir;
}

std::string Utils::getResourcePath(const std::string &relativePath)
{
	return std::string(getResourceDir()).append(relativePath);
}
