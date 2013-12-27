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

#ifndef _yas__text__buffer_serializer_hpp
#define _yas__text__buffer_serializer_hpp

#include <yas/detail/tools/buffers.hpp>
#include <yas/detail/type_traits/type_traits.hpp>
#include <yas/detail/type_traits/selector.hpp>
#include <yas/detail/io/serialization_exception.hpp>

namespace yas {
namespace detail {

/***************************************************************************/

template<>
struct serializer<
	type_prop::not_a_pod,
	ser_method::use_internal_serializer,
	archive_type::text,
	direction::out,
	intrusive_buffer
> {
	template<typename Archive>
	static Archive& apply(Archive& ar, const intrusive_buffer& buf) {
		ar & (std::uint32_t)buf.size;
		ar.write(space_sep);
		ar.write(buf.data, buf.size);
		return ar;
	}
};

/***************************************************************************/

template<>
struct serializer<
	type_prop::not_a_pod,
	ser_method::use_internal_serializer,
	archive_type::text,
	direction::out,
	shared_buffer
> {
	template<typename Archive>
	static Archive& apply(Archive& ar, const shared_buffer& buf) {
		ar & (std::uint32_t)buf.size;
		ar.write(space_sep);
		ar.write(buf.data.get(), buf.size);
		return ar;
	}
};

template<>
struct serializer<
	type_prop::not_a_pod,
	ser_method::use_internal_serializer,
	archive_type::text,
	direction::in,
	shared_buffer
> {
	template<typename Archive>
	static Archive& apply(Archive& ar, shared_buffer& buf) {
		std::uint32_t size = 0;
		ar & size;
		if ( ar.getch() != space_sep ) YAS_THROW_SPACE_IS_EXPECTED();
		buf.data.reset(new char[size], &shared_buffer::deleter);
		ar.read(buf.data.get(), size);
		buf.size = size;
		return ar;
	}
};

/***************************************************************************/

} // namespace detail
} // namespace yas

#endif // _yas__text__buffer_serializer_hpp
