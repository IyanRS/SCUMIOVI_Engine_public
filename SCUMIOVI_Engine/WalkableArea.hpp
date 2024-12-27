#pragma once

#include <vector>
#include "Vector2.hpp"
#include "Interactable.hpp"
#include <tuple>
#include <initializer_list>

class WalkableArea : public Interactable {
	std::vector<Vector2<>> walkablePolygon;
	std::vector<std::tuple<Vector2<>, float>> depthGradient;
	Vector2<> getClosestPointToSegment(Vector2<>, Vector2<>, Vector2<>);
	bool lineIntersectsLine(Vector2<> p1, Vector2<> q1, Vector2<> p2, Vector2<> q2);
	bool lineIntersectsPolygon(Vector2<> lineStart, Vector2<> lineEnd);
public:
	void showDebug();
	WalkableArea(std::vector<Vector2<>>);
	WalkableArea(std::initializer_list<Vector2<>>);
	bool isPointInside(Vector2<> point);
	Vector2<> getClosestPoint(Vector2<>);
	std::vector<Vector2<>> findPath(Vector2<>, Vector2<>);
};