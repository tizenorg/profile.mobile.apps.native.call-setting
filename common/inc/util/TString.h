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

#ifndef _UTIL_T_STRING_H_
#define _UTIL_T_STRING_H_

#include <string>
#include <utility>

#include "Helpers.h"

namespace util {

	class TString {
	public:
		inline TString();

		// str: text string or string id if translatable
		// domain: "" - default domain; nullptr - not translatable
		inline TString(const std::string &str, const char *domain = "");
		inline TString(const std::string &str, bool translatable);

		inline TString(std::string &&str, const char *domain = "");
		inline TString(std::string &&str, bool translatable);

		inline TString(const char *str, const char *domain = "");
		inline TString(const char *str, bool translatable);

		inline TString(const TString &src);
		inline TString(TString &&tmp);
		inline TString &operator=(const TString &src);
		inline TString &operator=(TString &&tmp);

		inline bool isEmpty() const;
		inline bool isNotEmpty() const;

		inline bool isTranslatable() const;
		inline bool hasDomain() const;
		inline const char *getDomain() const;

		inline const std::string &getStr() const;
		inline operator const std::string &() const;

		inline const char *getCStr() const;
		inline operator const char *() const;

		inline const char *translate() const;

		template <typename... ARGS>
		TString format(ARGS... args) const;

	private:
		inline const char *doTranslate(const char *strId) const;

	private:
		std::string m_str;
		const char *m_domain;
	};

	// INLINE IMPLEMENTATION //

	TString::TString() :
			m_domain(nullptr)
	{
	}

	TString::TString(const std::string &str, const char *domain) :
			m_str(str),
			m_domain(m_str.empty() ? nullptr : domain)
	{
	}

	TString::TString(const std::string &str, bool translatable) :
			TString(str, (translatable ? "" : nullptr))
	{
	}

	TString::TString(std::string &&str, const char *domain) :
			m_str(std::move(str)),
			m_domain(m_str.empty() ? nullptr : domain)
	{
	}

	TString::TString(std::string &&str, bool translatable) :
			TString(std::move(str), (translatable ? "" : nullptr))
	{
	}

	TString::TString(const char *str, const char *domain) :
			TString(std::string(nz(str)), domain)
	{
	}

	TString::TString(const char *str, bool translatable) :
			TString(std::string(nz(str)), (translatable ? "" : nullptr))
	{
	}

	TString::TString(const TString &src) :
			m_str(src.m_str),
			m_domain(src.m_domain)
	{
	}

	TString::TString(TString &&tmp) :
			m_str(std::move(tmp.m_str)),
			m_domain(tmp.m_domain)
	{
	}

	TString &TString::operator=(const TString &src)
	{
		m_str = src.m_str;
		m_domain = src.m_domain;
		return *this;
	}

	TString &TString::operator=(TString &&tmp)
	{
		m_str = std::move(tmp.m_str);
		m_domain = tmp.m_domain;
		return *this;
	}

	bool TString::isEmpty() const
	{
		return m_str.empty();
	}

	bool TString::isNotEmpty() const
	{
		return !m_str.empty();
	}

	const std::string &TString::getStr() const
	{
		return m_str;
	}

	TString::operator const std::string &() const
	{
		return m_str;
	}

	const char *TString::getCStr() const
	{
		return m_str.c_str();
	}

	TString::operator const char *() const
	{
		return m_str.c_str();
	}

	bool TString::isTranslatable() const
	{
		return (m_domain != nullptr);
	}

	bool TString::hasDomain() const
	{
		return !util::isEmpty(m_domain);
	}

	const char *TString::getDomain() const
	{
		return m_domain;
	}

	const char *TString::translate() const
	{
		if (!m_domain) {
			return getCStr();
		}
		return doTranslate(getCStr());
	}

	const char *TString::doTranslate(const char *strId) const
	{
		if (m_domain[0] == '\0') {
			return i18n_get_text(strId);
		}
		return dgettext(m_domain, strId);
	}

	template <typename... ARGS>
	TString TString::format(ARGS... args) const
	{
		const float STR_LEN_ADJUST = 1.7f;

		const char *fmt = m_str.c_str();
		size_t fmtLen = m_str.length();

		if (m_domain) {
			fmt = doTranslate(fmt);
			fmtLen = strlen(fmt);
		}

		size_t strLen = static_cast<size_t>(fmtLen * STR_LEN_ADJUST);

		std::string result;

		while (1) {
			result.resize(strLen);
			const size_t n = snprintf(&result[0], strLen, fmt, args...);
			if (n < strLen) {
				result.resize(n);
				break;
			}
			strLen = n + 1;
		}

		return TString(result, false);
	}

}

#endif /*_UTIL_T_STRING_H_*/
