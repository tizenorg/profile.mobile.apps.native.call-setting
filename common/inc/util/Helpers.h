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

#ifndef _UTIL_HELPERS_H_
#define _UTIL_HELPERS_H_

namespace util {

	inline const char *nz(const char *value, const char *zValue = "")
	{
		return (value ? value : zValue);
	}

	inline bool isEmpty(const char *value)
	{
		return (!value || (value[0] == '\0'));
	}

	inline bool isNotEmpty(const char *value)
	{
		return !isEmpty(value);
	}

	inline const char *ne(const char *value, const char *eValue = nullptr)
	{
		return (isNotEmpty(value) ? value : eValue);
	}

	inline char *strDupSafe(const char *value)
	{
		return (value ? strdup(value) : nullptr);
	}
}

#endif /*_UTIL_HELPERS_H_*/
