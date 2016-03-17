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

#ifndef DELEGATE_H_
#define DELEGATE_H_

#include "Utils/Callback.h"

namespace Utils {

	template <class FuncType>
	class Delegate;

	template <class R, class... Args>
	class Delegate<R(Args...)>
	{
	public:
		Delegate() :
			data(nullptr),
			func(nullptr)
		{
		}

		Delegate(std::nullptr_t) :
			data(nullptr),
			func(nullptr)
		{
		}

		template <class CLASS_TYPE, R(CLASS_TYPE::*method)(Args...)>
		static Delegate wrap(CLASS_TYPE *obj)
		{
			return Delegate((void *)obj, Callback<R(Args...)>::template make<CLASS_TYPE, method>());
		}

		template <class CLASS_TYPE, R(CLASS_TYPE::*method)(Args...) const>
		static Delegate wrap(const CLASS_TYPE *obj)
		{
			return Delegate((void *)obj, Callback<R(Args...)>::template make<CLASS_TYPE, method>());
		}

		template <typename DATA_TYPE, R(*func)(DATA_TYPE, Args...)>
		static Delegate wrap(DATA_TYPE data)
		{
			return Delegate((void *)data, Callback<R(Args...)>::template make<DATA_TYPE, func>());
		}

		template <typename DATA_TYPE, R(*func)(Args..., DATA_TYPE)>
		static Delegate wrap_alt(DATA_TYPE data)
		{
			return Delegate((void *)data, Callback<R(Args...)>::template make_alt<DATA_TYPE, func>());
		}

		template <R(*func)(Args...)>
		static Delegate wrap()
		{
			return Delegate(Callback<R(Args...)>::template make<func>());
		}

		void reset()
		{
			data = nullptr;
			func = nullptr;
		}

		bool assigned() const
		{
			return (func != nullptr);
		}

		R operator()(Args... args) const
		{
			return func(data, std::forward<Args>(args)...);
		}

		bool operator==(std::nullptr_t) const
		{
			return !assigned();
		}

		bool operator!=(std::nullptr_t) const
		{
			return assigned();
		}

		bool operator==(const Delegate &rv) const
		{
			return ((data == rv.data) && (func == rv.func));
		}

		bool operator!=(const Delegate &rv) const
		{
			return ((data != rv.data) && (func != rv.func));
		}

		bool operator<(const Delegate &rv) const
		{
			return ((data < rv.data) || ((data == rv.data) && (func < rv.func)));
		}

		bool operator<=(const Delegate &rv) const
		{
			return ((data < rv.data) || ((data == rv.data) && (func <= rv.func)));
		}

		bool operator>(const Delegate &rv) const
		{
			return ((data > rv.data) || ((data == rv.data) && (func > rv.func)));
		}

		bool operator>=(const Delegate &rv) const
		{
			return ((data > rv.data) || ((data == rv.data) && (func >= rv.func)));
		}

	private:
		Delegate(void *data, R(*func)(void *data, Args...)) :
			data(data),
			func(data ? func : nullptr)
		{
		}

		Delegate(R(*func)(void *data, Args...)) :
			data(nullptr),
			func(func)
		{
		}

	private:
		void *data;
		R(*func)(void *data, Args...);
	};

}
#endif /* DELEGATE_H_ */
