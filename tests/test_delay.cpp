#include "catch.hpp"
#include "../SCUMIOVI_Engine/Delay.hpp"

TEST_CASE("Delay Initialization and getSeconds", "[delay]") {
    Delay delay(2.5f);
    REQUIRE(delay.getSeconds() == Catch::Detail::Approx(2.5f));
}

TEST_CASE("Delay initialization with zero", "[delay]") {
    Delay delay(0.0f);
    REQUIRE(delay.getSeconds() == 0.0f);
}
