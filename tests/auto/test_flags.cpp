#include <stdcorelib/flags.h>

#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <type_traits>

using namespace stdc;

namespace {

    enum class Permission {
        None = 0,
        Read = 0x01,
        Write = 0x02,
        Execute = 0x04
    };

    STDC_DECLARE_FLAGS(Permissions, Permission)
    STDC_DECLARE_OPERATORS_FOR_FLAGS(Permissions)

    enum class BigPermission : uint64_t {
        None = 0,
        A = 1ull << 40,
        B = 1ull << 41
    };

    STDC_DECLARE_FLAGS(BigPermissions, BigPermission)
    STDC_DECLARE_OPERATORS_FOR_FLAGS(BigPermissions)

}

BOOST_AUTO_TEST_SUITE(test_flags)

BOOST_AUTO_TEST_CASE(test_type_and_construct) {
    static_assert(std::is_same_v<decltype(Permission::Read | Permission::Write), Permissions>);
    static_assert(std::is_same_v<decltype(Permission::Read & Permission::Write), Permissions>);

    Permissions perms;
    BOOST_CHECK(!static_cast<bool>(perms));

    perms = Permission::Read | Permission::Write;
    BOOST_CHECK(perms.test_flag(Permission::Read));
    BOOST_CHECK(perms.test_any_flag(Permission::Write));
    BOOST_CHECK(!perms.test_flag(Permission::Execute));

    Permissions from_list{Permission::Read, Permission::Execute};
    BOOST_CHECK(from_list.test_flags(Permission::Read | Permission::Execute));
}

BOOST_AUTO_TEST_CASE(test_set_and_bitops) {
    Permissions perms = Permission::Read;

    perms.set_flag(Permission::Write);
    BOOST_CHECK(perms.test_flags(Permission::Read | Permission::Write));

    perms.set_flag(Permission::Read, false);
    BOOST_CHECK(!perms.test_flag(Permission::Read));
    BOOST_CHECK(perms == Permission::Write);

    perms ^= Permission::Execute;
    BOOST_CHECK(perms.test_any_flags(Permission::Write | Permission::Execute));

    auto and_result = perms & Permission::Write;
    BOOST_CHECK(and_result == Permission::Write);
}

BOOST_AUTO_TEST_CASE(test_from_to_int_and_64bit_enum) {
    auto combined = BigPermission::A | BigPermission::B;
    BOOST_CHECK(combined.test_flag(BigPermission::A));
    BOOST_CHECK(combined.test_flag(BigPermission::B));

    BigPermissions raw = BigPermissions::fromInt((1ull << 41));
    BOOST_CHECK(raw == BigPermission::B);
    BOOST_CHECK_EQUAL(static_cast<uint64_t>(raw.toInt()), (1ull << 41));
}

BOOST_AUTO_TEST_SUITE_END()
