#include "catch.hpp"
#include "WalkableArea.hpp"

TEST_CASE("WalkableArea::isPointInside", "[WalkableArea]") {
    WalkableArea squareArea({
        Vector2<>(0.f, 0.f),
        Vector2<>(10.f, 0.f),
        Vector2<>(10.f, 10.f),
        Vector2<>(0.f, 10.f)
    });

    SECTION("Point strictly inside") {
        REQUIRE(squareArea.isPointInside(Vector2<>(5.f, 5.f)) == true);
    }

    SECTION("Point strictly outside") {
        REQUIRE(squareArea.isPointInside(Vector2<>(15.f, 5.f)) == false);
        REQUIRE(squareArea.isPointInside(Vector2<>(-5.f, 5.f)) == false);
        REQUIRE(squareArea.isPointInside(Vector2<>(5.f, 15.f)) == false);
        REQUIRE(squareArea.isPointInside(Vector2<>(5.f, -5.f)) == false);
    }

    WalkableArea L_Shape({
        Vector2<>(0.f, 0.f),
        Vector2<>(10.f, 0.f),
        Vector2<>(10.f, 5.f),
        Vector2<>(5.f, 5.f),
        Vector2<>(5.f, 10.f),
        Vector2<>(0.f, 10.f)
    });

    SECTION("Point in the cutout of a concave polygon") {
        REQUIRE(L_Shape.isPointInside(Vector2<>(7.5f, 7.5f)) == false);
    }

    SECTION("Point inside concave polygon") {
        REQUIRE(L_Shape.isPointInside(Vector2<>(2.5f, 2.5f)) == true);
        REQUIRE(L_Shape.isPointInside(Vector2<>(7.5f, 2.5f)) == true);
        REQUIRE(L_Shape.isPointInside(Vector2<>(2.5f, 7.5f)) == true);
    }
}

TEST_CASE("WalkableArea::getClosestPoint", "[WalkableArea]") {
    WalkableArea squareArea({
        Vector2<>(0.f, 0.f),
        Vector2<>(10.f, 0.f),
        Vector2<>(10.f, 10.f),
        Vector2<>(0.f, 10.f)
    });

    SECTION("Point outside closest to a flat edge") {
        Vector2<> closest = squareArea.getClosestPoint(Vector2<>(5.f, -5.f));
        REQUIRE(closest.x == 5.f);
        REQUIRE(closest.y == 0.f);
    }

    SECTION("Point outside closest to a vertex") {
        Vector2<> closest = squareArea.getClosestPoint(Vector2<>(15.f, 15.f));
        REQUIRE(closest.x == 10.f);
        REQUIRE(closest.y == 10.f);
    }

    SECTION("Point already inside should return itself") {
        Vector2<> p(5.f, 5.f);
        Vector2<> closest = squareArea.getClosestPoint(p);
        REQUIRE(closest.x == p.x);
        REQUIRE(closest.y == p.y);
    }
}
