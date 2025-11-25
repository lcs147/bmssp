#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include "../bmssp.hpp"
TEST_CASE("Basic Math Operations", "[math]") {
    SECTION("Addition Test") {
        REQUIRE(1 + 2 == 3);
    }
}