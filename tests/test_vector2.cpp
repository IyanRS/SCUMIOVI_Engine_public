#include "../SCUMIOVI_Engine/Vector2.hpp"
#include "catch.hpp"

TEST_CASE("Vector2 Default Initialization", "[vector2]") {
  Vector2<float> v;
  REQUIRE(v.x == 0.0f);
  REQUIRE(v.y == 0.0f);
}

TEST_CASE("Vector2 Initialization", "[vector2]") {
  Vector2<float> v(3.5f, 4.2f);
  REQUIRE(v.x == 3.5f);
  REQUIRE(v.y == 4.2f);
}

TEST_CASE("Vector2 Equality and Inequality", "[vector2]") {
  Vector2<int> v1(5, 10);
  Vector2<int> v2(5, 10);
  Vector2<int> v3(10, 5);

  REQUIRE(v1 == v2);
  REQUIRE(v1 != v3);
}

TEST_CASE("Vector2 Addition and Subtraction", "[vector2]") {
  Vector2<int> v1(5, 10);
  Vector2<int> v2(3, 2);

  Vector2<int> sum = v1 + v2;
  REQUIRE(sum.x == 8);
  REQUIRE(sum.y == 12);

  Vector2<int> diff = v1 - v2;
  REQUIRE(diff.x == 2);
  REQUIRE(diff.y == 8);
}

TEST_CASE("Vector2 Multiplication and Division", "[vector2]") {
  Vector2<float> v(2.0f, 4.0f);

  Vector2<float> mul = v * 2.5f;
  REQUIRE(mul.x == 5.0f);
  REQUIRE(mul.y == 10.0f);

  Vector2<float> div = v / 2.0f;
  REQUIRE(div.x == 1.0f);
  REQUIRE(div.y == 2.0f);
}

TEST_CASE("Vector2 DistanceTo", "[vector2]") {
  Vector2<float> v1(0.0f, 0.0f);
  Vector2<float> v2(3.0f, 4.0f);

  REQUIRE(v1.distanceTo(v2) == Catch::Detail::Approx(5.0f));
}

TEST_CASE("Vector2 Normalized", "[vector2]") {
  Vector2<float> v(3.0f, 4.0f);
  Vector2<float> norm = v.normalized();

  REQUIRE(norm.x == Catch::Detail::Approx(0.6f));
  REQUIRE(norm.y == Catch::Detail::Approx(0.8f));
  REQUIRE(norm.distanceTo(Vector2<float>(0.0f, 0.0f)) ==
          Catch::Detail::Approx(1.0f));
}

TEST_CASE("Vector2 Dot Product", "[vector2]") {
  Vector2<float> v1(1.0f, 2.0f);
  Vector2<float> v2(3.0f, 4.0f);

  REQUIRE(v1.dot(v2) == Catch::Detail::Approx(11.0f));
}
