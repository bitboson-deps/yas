
// Copyright (c) 2010-2014 niXman (i dot nixman dog gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef _yas__binary_oarchive_hpp
#define _yas__binary_oarchive_hpp

#include <ostream>

#include <yas/detail/type_traits/properties.hpp>
#include <yas/detail/type_traits/has_method_serialize.hpp>
#include <yas/detail/type_traits/has_function_serialize.hpp>
#include <yas/detail/type_traits/selector.hpp>

#include <yas/detail/io/information.hpp>
#include <yas/detail/io/streams.hpp>

#include <yas/detail/base_object.hpp>

#include <yas/serializers/serializer.hpp>
#include <yas/serializers/binary/utility/pod_serializers.hpp>
#include <yas/serializers/binary/utility/enum_serializer.hpp>
#include <yas/serializers/binary/utility/usertype_serializers.hpp>
#include <yas/serializers/binary/utility/autoarray_serializers.hpp>
#include <yas/serializers/binary/utility/buffer_serializers.hpp>

#include <yas/detail/tools/buffers.hpp>
#include <yas/detail/tools/noncopyable.hpp>

namespace yas {

/***************************************************************************/

template<typename OS>
struct binary_oarchive
	:detail::stream<archive_type::binary, direction::out, OS>
	,detail::archive_information<archive_type::binary, direction::out, OS>
	,private detail::noncopyable
{
	using stream_type = OS;

	binary_oarchive(OS &os, header_flag op = with_header)
		:detail::stream<archive_type::binary, direction::out, OS>(os)
		,detail::archive_information<archive_type::binary, direction::out, OS>(os, op)
	{}

	template<typename T>
	binary_oarchive& operator& (const T& v) {
		using namespace detail;
		return serializer<
			type_properties<T>::value,
			serialization_method<T, binary_oarchive<OS>>::value,
			archive_type::binary,
			direction::out,
			T
		>::apply(*this, v);
	}
};

/***************************************************************************/

} // namespace yas

#endif // _yas__binary_oarchive_hpp
