
#include <iostream>
#include <fstream>
#include <cstdio>

#include <yas/binary_oarchive.hpp>
#include <yas/binary_iarchive.hpp>
#include <yas/text_oarchive.hpp>
#include <yas/text_iarchive.hpp>
#include <yas/json_oarchive.hpp>
#include <yas/json_iarchive.hpp>

#include <yas/mem_streams.hpp>
#include <yas/file_streams.hpp>

#include <yas/detail/tools/hexdumper.hpp>

#include <yas/serializers/std_types_serializers.hpp>

#if defined(YAS_SERIALIZE_BOOST_TYPES)
#include <yas/serializers/boost_types_serializers.hpp>
#include <boost/fusion/container/generation/make_vector.hpp>
#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/sequence/comparison.hpp>
#include <boost/fusion/include/comparison.hpp>
#endif // defined(YAS_SERIALIZE_BOOST_TYPES)

#if defined(YAS_SERIALIZE_QT_TYPES)
#include <yas/serializers/qt_types_serializers.hpp>
#endif

#include "include/array.hpp"
#include "include/auto_array.hpp"
#include "include/bitset.hpp"
#include "include/buffer.hpp"
#include "include/enum.hpp"
#include "include/base_object.hpp"
#include "include/forward_list.hpp"
#include "include/fusion_list.hpp"
#include "include/fusion_map.hpp"
#include "include/fusion_pair.hpp"
#include "include/fusion_set.hpp"
#include "include/fusion_tuple.hpp"
#include "include/fusion_vector.hpp"
#include "include/list.hpp"
#include "include/map.hpp"
#include "include/multimap.hpp"
#include "include/multiset.hpp"
#include "include/pair.hpp"
#include "include/pod.hpp"
#include "include/set.hpp"
#include "include/string.hpp"
#include "include/tuple.hpp"
#include "include/types.hpp"
#include "include/unordered_map.hpp"
#include "include/unordered_multimap.hpp"
#include "include/unordered_multiset.hpp"
#include "include/unordered_set.hpp"
#include "include/vector.hpp"
#include "include/endian.hpp"
#include "include/version.hpp"
#include "include/wstring.hpp"
#include "include/one_function.hpp"
#include "include/one_method.hpp"
#include "include/split_functions.hpp"
#include "include/split_methods.hpp"

/***************************************************************************/

template<bool, typename, typename>
struct concrete_archive_traits;

// mem archives traits
template<typename OA, typename IA>
struct concrete_archive_traits<true, OA, IA> {
	typedef OA oarchive_type;
	typedef IA iarchive_type;

	/** output archive */
	struct oarchive {
		oarchive():oa(0) {}
		~oarchive() { delete oa; }

		oarchive_type* operator->() { return oa; }

		template<typename T>
		oarchive_type& operator& (const T& v) { return (*(oa) & v); }

		std::uint32_t size() const { return stream.get_intrusive_buffer().size; }
		void dump() {
			const yas::intrusive_buffer buf = stream.get_intrusive_buffer();
			std::cout << yas::hex_dump(buf.data, buf.size) << std::endl;
		}

		bool compare(const void* ptr, std::uint32_t size) const {
			const yas::intrusive_buffer buf = stream.get_intrusive_buffer();
			return size == buf.size ? (0 == std::memcmp(buf.data, ptr, size)) : false;
		}

		typename oarchive_type::stream_type stream;
		oarchive_type* oa;
	};
	static void ocreate(oarchive& oa, const char* archive_type, const char* io_type) {
		((void)archive_type);
		((void)io_type);
		oa.oa = new oarchive_type(oa.stream);
	}

	/** input archive */
	struct iarchive {
		iarchive():stream(0),ia(0) {}
		~iarchive() { delete ia; delete stream; }

		iarchive_type* operator->() { return ia; }

		template<typename T>
		iarchive_type& operator& (T& v) { return (*(ia) & v); }

		typename iarchive_type::stream_type *stream;
		iarchive_type* ia;
	};
	static void icreate(iarchive& ia, oarchive& oa, const char* archive_type, const char* io_type) {
		((void)archive_type);
		((void)io_type);
		ia.stream = new typename iarchive_type::stream_type(oa.stream.get_intrusive_buffer());
		ia.ia = new iarchive_type(*(ia.stream));
	}
};

/***************************************************************************/

static std::uint32_t oa_cnt = 0;

// file archives traits
template<typename OA, typename IA>
struct concrete_archive_traits<false, OA, IA> {
	typedef OA oarchive_type;
	typedef IA iarchive_type;

	struct oarchive {
		oarchive():oa(0) {++oa_cnt;}
		~oarchive() {
			delete stream;
			delete oa;
			--oa_cnt;
			std::remove(fname.c_str());
		}

		oarchive_type* operator->() { return oa; }

		template<typename T>
		oarchive_type& operator& (const T& v) { return (*(oa) & v); }

		std::uint32_t size() {
			stream->flush();
			std::ifstream f(fname);
			f.seekg(0, std::ios::end);
			return f.tellg();
		}
		void dump() {
			std::string str(size(), 0);
			std::ifstream f(fname, std::ios::binary);
			assert(f);
			f.read(&str[0], size());
			std::cout << yas::hex_dump(str.c_str(), size()) << std::endl;
		}

		bool compare(const void* ptr, std::uint32_t size) {
			if ( this->size() != size ) return false;
			std::string str(size, 0);
			std::ifstream f(fname, std::ios::binary);
			assert(f);
			f.read(&str[0], size);
			return memcmp(str.c_str(), ptr, size) == 0;
		}

		std::string fname;
		typename oarchive_type::stream_type *stream;
		oarchive_type* oa;
	};
	static void ocreate(oarchive& oa, const char* archive_type, const char* io_type) {
		((void)io_type);

		oa.fname += archive_type;
		oa.fname += "_";
		oa.fname += std::to_string(oa_cnt);
		oa.fname += ".bin";
		oa.stream = new typename oarchive_type::stream_type(oa.fname, yas::file_trunc);
		oa.oa = new oarchive_type(*(oa.stream));
	}

	struct iarchive {
		iarchive():ia(0) {}
		~iarchive() { delete stream; delete ia; }

		iarchive_type* operator->() { return ia; }

		template<typename T>
		iarchive_type& operator& (T& v) { return (*(ia) & v); }

		std::string fname;
		typename iarchive_type::stream_type *stream;
		iarchive_type* ia;
	};
	static void icreate(iarchive& ia, oarchive& oa, const char* archive_type, const char* io_type) {
		((void)archive_type);
		((void)io_type);
		oa.stream->flush();
		ia.fname = oa.fname;
		ia.stream = new typename iarchive_type::stream_type(oa.fname);
		ia.ia = new iarchive_type(*(ia.stream));
	}
};

/***************************************************************************/

#define YAS_RUN_TEST(testname, passcnt, failcnt) { \
	static const char * artype = \
		(yas::is_binary_archive<OA>::value ? "binary" \
			: yas::is_text_archive<OA>::value ? "text" \
				: yas::is_json_archive<OA>::value ? "json" \
		: "unknown" \
	); \
	static const char *iotype = (mem ? "mem" : "file"); \
	printf( \
		 "%-6s %-4s: %-18s -> %s\n" \
		,artype /* 1 */ \
		,iotype /* 2 */ \
		,#testname /* 3 */ \
		,(testname##_test<concrete_archive_traits<mem, OA, IA>>(artype, iotype) \
			?(++passcnt,"passed") \
			:(++failcnt,"failed!") \
		) /* 4 */ \
	); \
}

template<bool mem, typename OA, typename IA>
void tests(std::uint32_t& p, std::uint32_t& e) {
	YAS_RUN_TEST(endian              , p, e);
	YAS_RUN_TEST(version					, p, e);
	YAS_RUN_TEST(pod						, p, e);
	YAS_RUN_TEST(enum						, p, e);
	YAS_RUN_TEST(base_object			, p, e);
	YAS_RUN_TEST(auto_array				, p, e);
	YAS_RUN_TEST(array					, p, e);
	YAS_RUN_TEST(bitset					, p, e);
	YAS_RUN_TEST(buffer					, p, e);
	YAS_RUN_TEST(string					, p, e);
	YAS_RUN_TEST(wstring					, p, e);
	YAS_RUN_TEST(pair						, p, e);
	YAS_RUN_TEST(tuple					, p, e);
	YAS_RUN_TEST(vector					, p, e);
	YAS_RUN_TEST(list						, p, e);
	YAS_RUN_TEST(forward_list			, p, e);
	YAS_RUN_TEST(map						, p, e);
	YAS_RUN_TEST(set						, p, e);
	YAS_RUN_TEST(multimap				, p, e);
	YAS_RUN_TEST(multiset				, p, e);
	YAS_RUN_TEST(unordered_map			, p, e);
	YAS_RUN_TEST(unordered_set			, p, e);
	YAS_RUN_TEST(unordered_multimap	, p, e);
	YAS_RUN_TEST(unordered_multiset	, p, e);
#if defined(YAS_HAS_BOOST_FUSION)
	YAS_RUN_TEST(fusion_pair			, p, e);
	YAS_RUN_TEST(fusion_tuple			, p, e);
	YAS_RUN_TEST(fusion_vector			, p, e);
	YAS_RUN_TEST(fusion_list			, p, e);
	YAS_RUN_TEST(fusion_set				, p, e);
	YAS_RUN_TEST(fusion_map				, p, e);
#endif
	YAS_RUN_TEST(one_function			, p, e);
	YAS_RUN_TEST(split_functions		, p, e);
	YAS_RUN_TEST(one_method				, p, e);
	YAS_RUN_TEST(split_methods			, p, e);
}

/***************************************************************************/

int main() {
	setvbuf(stdout, 0, _IONBF, 0);

	types_test();

	std::uint32_t passed = 0;
	std::uint32_t failed = 0;

	try {
		tests<true, yas::binary_oarchive<yas::mem_ostream>, yas::binary_iarchive<yas::mem_istream>>(passed, failed);
		tests<false, yas::binary_oarchive<yas::file_ostream>, yas::binary_iarchive<yas::file_istream>>(passed, failed);
		tests<true, yas::text_oarchive<yas::mem_ostream>, yas::text_iarchive<yas::mem_istream>>(passed, failed);
		tests<false, yas::text_oarchive<yas::file_ostream>, yas::text_iarchive<yas::file_istream>>(passed, failed);
	} catch (const std::exception &ex) {
		std::cout << "[exception]: " << ex.what() << std::endl;
	}

	std::cout << std::endl
	<< "/***************************************************/" << std::endl
	<< "> platform bits: " << (YAS_PLATFORM_BITS()) << std::endl
	<< "> passed tests : " << passed << std::endl
	<< "> failed tests : " << failed << std::endl
	<< "/***************************************************/" << std::endl;
}

/***************************************************************************/
