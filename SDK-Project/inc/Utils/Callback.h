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

#ifndef CALLBACK_H_
#define CALLBACK_H_

#include <utility>

namespace Utils {

	template <class FuncType>
	class Callback;

	template <class R, class... Args>
	class Callback<R(Args...)>
	{
	public:
		typedef R(*Type)(void *data, Args... args);

	public:
		Callback() = delete;

		template <class CLASS_TYPE, R(CLASS_TYPE::*method)(Args...)>
		static Type make()
		{
			return &stub<CLASS_TYPE, method>;
		}

		template <class CLASS_TYPE, R(CLASS_TYPE::*method)(Args...) const>
		static Type make()
		{
			return &stub<const CLASS_TYPE, method>;
		}

		template <typename DATA_TYPE, R(*func)(DATA_TYPE, Args...)>
		static Type make()
		{
			return &stub<DATA_TYPE, func>;
		}

		template <typename DATA_TYPE, R(*func)(Args..., DATA_TYPE)>
		static Type make_alt()
		{
			return &stub_alt<DATA_TYPE, func>;
		}

		template <R(*func)(Args...)>
		static Type make()
		{
			return &stub<func>;
		}

	private:
		template <class CLASS_TYPE, R(CLASS_TYPE::*method)(Args...)>
		static R stub(void *data, Args... args)
		{
			return (static_cast<CLASS_TYPE *>(data)->*method)(std::forward<Args>(args)...);
		}

		template <typename DATA_TYPE, R(*func)(DATA_TYPE, Args...)>
		static R stub(void *data, Args... args)
		{
			return func(static_cast<DATA_TYPE>(data), std::forward<Args>(args)...);
		}

		template <typename DATA_TYPE, R(*func)(Args..., DATA_TYPE)>
		static R stub_alt(void *data, Args... args)
		{
			return func(std::forward<Args>(args)..., static_cast<DATA_TYPE>(data));
		}

		template <R(*func)(Args...)>
		static R stub(void *data, Args... args)
		{
			return func(std::forward<Args>(args)...);
		}
	};

	template <class FuncType>
	class CallbackAlt;

	template <class R, class... Args>
	class CallbackAlt<R(Args...)>
	{
	public:
		typedef R(*Type)(Args... args, void *data);

	public:
		CallbackAlt() = delete;

		template <class CLASS_TYPE, R(CLASS_TYPE::*method)(Args...)>
		static Type make()
		{
			return &stub<CLASS_TYPE, method>;
		}

		template <class CLASS_TYPE, R(CLASS_TYPE::*method)(Args...) const>
		static Type make()
		{
			return &stub<const CLASS_TYPE, method>;
		}

		template <typename DATA_TYPE, R(*func)(DATA_TYPE, Args...)>
		static Type make()
		{
			return &stub<DATA_TYPE, func>;
		}

		template <typename DATA_TYPE, R(*func)(Args..., DATA_TYPE)>
		static Type make_alt()
		{
			return &stub_alt<DATA_TYPE, func>;
		}

		template <R(*func)(Args...)>
		static Type make()
		{
			return &stub<func>;
		}

	private:
		template <class CLASS_TYPE, R(CLASS_TYPE::*method)(Args...)>
		static R stub(Args... args, void *data)
		{
			return (static_cast<CLASS_TYPE *>(data)->*method)(std::forward<Args>(args)...);
		}

		template <typename DATA_TYPE, R(*func)(DATA_TYPE, Args...)>
		static R stub(Args... args, void *data)
		{
			return func(static_cast<DATA_TYPE>(data), std::forward<Args>(args)...);
		}

		template <typename DATA_TYPE, R(*func)(Args..., DATA_TYPE)>
		static R stub_alt(Args... args, void *data)
		{
			return func(std::forward<Args>(args)..., static_cast<DATA_TYPE>(data));
		}

		template <R(*func)(Args...)>
		static R stub(Args... args, void *data)
		{
			return func(std::forward<Args>(args)...);
		}
	};
}

#endif /* CALLBACK_H_ */
