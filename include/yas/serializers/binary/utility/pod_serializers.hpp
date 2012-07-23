
// Copyright (c) 2010-2012 niXman (i dot nixman dog gmail dot com)
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

#ifndef _yas__binary__pod_serializer_hpp__included_
#define _yas__binary__pod_serializer_hpp__included_

#include <yas/detail/type_traits/properties.hpp>
#include <yas/detail/type_traits/selector.hpp>

#include <stdexcept>

namespace yas {
namespace detail {

/***************************************************************************/

template<typename T>
struct serializer<
	type_prop::is_enum,
	ser_method::use_internal_serializer,
	archive_type::binary,
	direction::out,
	T
> {
	template<typename Archive>
	static Archive& apply(Archive& ar, const T& v) {
		const yas::uint8_t size = sizeof(T);
		ar.write(reinterpret_cast<const char*>(&size), sizeof(size));
		ar.write(reinterpret_cast<const char*>(&v), sizeof(v));
		return ar;
	}
};

template<typename T>
struct serializer<
	type_prop::is_enum,
	ser_method::use_internal_serializer,
	archive_type::binary,
	direction::in,
	T
> {
	template<typename Archive>
	static Archive& apply(Archive& ar, T& v) {
		yas::uint8_t size = 0;
		ar.read(reinterpret_cast<char*>(&size), sizeof(size));
		switch ( size ) {
			case sizeof(yas::uint8_t):
				ar.read(reinterpret_cast<char*>(&v), sizeof(yas::uint8_t));
			break;
			case sizeof(yas::uint16_t):
				ar.read(reinterpret_cast<char*>(&v), sizeof(yas::uint16_t));
			break;
			case sizeof(yas::uint32_t):
				ar.read(reinterpret_cast<char*>(&v), sizeof(yas::uint32_t));
			break;
			case sizeof(yas::uint64_t):
				ar.read(reinterpret_cast<char*>(&v), sizeof(yas::uint64_t));
			break;
			default:
				throw std::runtime_error("bad size of enum");
		}
		return ar;
	}
};

/***************************************************************************/

template<typename T>
struct serializer<
	type_prop::is_pod,
	ser_method::use_internal_serializer,
	archive_type::binary,
	direction::out,
	T
> {
	template<typename Archive>
	static Archive& apply(Archive& ar, const T& v) {
		ar.write(reinterpret_cast<const typename Archive::char_type*>(&v), sizeof(T));
		return ar;
	}
};

template<typename T>
struct serializer<
	type_prop::is_pod,
	ser_method::use_internal_serializer,
	archive_type::binary,
	direction::in,
	T
> {
	template<typename Archive>
	static Archive& apply(Archive& ar, T& v) {
		ar.read(reinterpret_cast<typename Archive::char_type*>(&v), sizeof(T));
		return ar;
	}
};

/***************************************************************************/

} // namespace detail
} // namespace yas

#endif // _yas__binary__pod_serializer_hpp__included_
