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

#ifndef _UTIL_DELEGATION_H_
#define _UTIL_DELEGATION_H_

#include "Delegate.h"
#include "NonCopyable.h"

#include <vector>
#include <utility>
#include <algorithm>

namespace util {

	template <class FuncType>
	class Delegation;

	template <class R, class... Args>
	class Delegation<R(Args...)> : private NonCopyable
	{
	public:
		Delegation() :
			m_locked(false),
			m_isFragmented(false)
		{
		}

		void operator+=(const Delegate<R(Args...)> &delegate)
		{
			if (delegate.assigned() &&
				std::find(m_delegates.begin(), m_delegates.end(), delegate) == m_delegates.end()) {
				m_delegates.push_back(delegate);
			}
		}

		void operator-=(const Delegate<R(Args...)> &delegate)
		{
			const typename DelegateVector::iterator it = std::find(
				m_delegates.begin(), m_delegates.end(), delegate);
			if (it != m_delegates.end()) {
				if (m_locked) {
					it->reset();
					m_isFragmented = true;
				} else {
					m_delegates.erase(it);
				}
			}
		}

		bool isEmpty()
		{
			if (m_locked) {
				return false;
			} else {
				return m_delegates.empty();
			}
		}

		template <class Predicate>
		void invoke(Args... args, Predicate pred)
		{
			m_locked = true;
			size_t i = 0;
			while (i < m_delegates.size()) {
				if (m_delegates[i].assigned()) {
					if (!dispatch(m_delegates[i], std::forward<Args>(args)..., pred)) {
						break;
					}
				}
				++i;
			}
			m_locked = false;
			defrag();
		}

		void invoke(Args... args)
		{
			m_locked = true;
			size_t i = 0;
			while (i < m_delegates.size()) {
				if (m_delegates[i].assigned()) {
					(void)m_delegates[i](std::forward<Args>(args)...);
				}
				++i;
			}
			m_locked = false;
			defrag();
		}

	private:
		template <class R2, class Predicate>
		bool dispatch(const Delegate<R2(Args...)> &delegate, Args... args, Predicate pred)
		{
			return pred(delegate(std::forward<Args>(args)...));
		}

		template <class Predicate>
		bool dispatch(const Delegate<void(Args...)> &delegate, Args... args, Predicate pred)
		{
			delegate(std::forward<Args>(args)...);
			return pred();
		}

		void defrag()
		{
			if (m_isFragmented) {
				m_delegates.erase(std::remove_if(m_delegates.begin(), m_delegates.end(),
					[](const Delegate <R(Args...)> &delegate) -> bool {
						return !delegate.assigned();
					}),
					m_delegates.end());

				m_isFragmented = false;
			}
		}

	private:
		typedef std::vector<Delegate<R(Args...)> > DelegateVector;

	private:
		DelegateVector m_delegates;
		bool m_locked;
		bool m_isFragmented;
	};

}
#endif /*_UTIL_DELEGATION_H_*/
