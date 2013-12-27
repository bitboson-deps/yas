
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

#ifndef _yas_test__split_functions_hpp__included_
#define _yas_test__split_functions_hpp__included_

/***************************************************************************/
namespace {

bool _binary_type_for_split_function_serializers_save_flag = false;
bool _binary_type_for_split_function_serializers_load_flag = false;

struct _binary_type_for_split_function_serializers {
	_binary_type_for_split_function_serializers() {}

	int x;
	int y;
};

} // ns

namespace yas {

template<typename Archive>
void serialize(Archive& ar, const _binary_type_for_split_function_serializers& t) {
	ar & t.x
		& t.y;
	_binary_type_for_split_function_serializers_save_flag = true;
}

template<typename Archive>
void serialize(Archive& ar, _binary_type_for_split_function_serializers& t) {
	ar & t.x
		& t.y;
	_binary_type_for_split_function_serializers_load_flag = true;
}

} // namespace yas

template<typename archive_traits>
bool split_functions_test(const char* archive_type, const char* io_type) {
	_binary_type_for_split_function_serializers t1, t2;
	t1.x = 33; t1.y = 44;

	typename archive_traits::oarchive oa;
	archive_traits::ocreate(oa, archive_type, io_type);
	oa & t1;

	if ( !_binary_type_for_split_function_serializers_save_flag ) {
		std::cout << "free function serialize() for save is not called! [1]" << std::endl;
		return false;
	}

	typename archive_traits::iarchive ia;
	archive_traits::icreate(ia, oa, archive_type, io_type);
	ia & t2;

	if ( !_binary_type_for_split_function_serializers_load_flag ) {
		std::cout << "free function serialize() for load is not called! [2]" << std::endl;
		return false;
	}

	if ( t1.x != t2.x || t1.y != t2.y ) {
		std::cout << "SPLIT FUNCTION deserialization error!" << std::endl;
		return false;
	}

	return true;
}

/***************************************************************************/

#endif // _yas_test__split_functions_hpp__included_
