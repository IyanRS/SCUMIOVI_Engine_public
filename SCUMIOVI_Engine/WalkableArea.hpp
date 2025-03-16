#pragma once

#include <vector>
#include "Vector2.hpp"
#include "Interactable.hpp"
#include <tuple>
#include <initializer_list>

class WalkableArea : public Interactable {
	// TODO: add hole polygons to represent more complex shapes
	std::vector<Vector2<>> walkablePolygon;
	std::vector<std::tuple<Vector2<>, float>> depthGradient;
	Vector2<> getClosestPointToSegment(Vector2<>, Vector2<>, Vector2<>);
	bool lineIntersectsLine(Vector2<> p1, Vector2<> q1, Vector2<> p2, Vector2<> q2);
	bool lineIntersectsPolygon(Vector2<> lineStart, Vector2<> lineEnd);

	bool isDirectPathValid(const Vector2<>& start, const Vector2<>& target, const std::vector<Vector2<>>& polygon);
	std::vector<Vector2<>> getVisibilityGraph(Vector2<> start, Vector2<> end);
	std::vector<Vector2<>> reconstructPath(std::unordered_map<Vector2<>, Vector2Hash>& cameFrom, Vector2<> start, Vector2<> end);
	bool isVisible(Vector2<> a, Vector2<> b);
	int getNIntersections(Vector2<> a, Vector2<> b);
public:
	/// <summary>
	/// Adds debug lines to the screen to show the shape of the walk area.
	/// </summary>
	void showDebug();
	WalkableArea(std::vector<Vector2<>>);
	WalkableArea(std::initializer_list<Vector2<>>);
	/// <summary>
	/// Checks if the point is inside of the walk area polygon.
	/// </summary>
	/// <param name="point"></param>
	bool isPointInside(Vector2<> point);
	/// <summary>
	/// Returns the closest point insde the walk area to the given point.
	/// </summary>
	/// <param name="point"></param>
	Vector2<> getClosestPoint(Vector2<>);

	/// <summary>
	/// Calculates and returns a path from one point to another that is fully
	/// inside the walkable area. Returns a zero size vector if it is not possible.
	/// </summary>
	/// <param name="point"></param>
	std::vector<Vector2<>> findPath(Vector2<>, Vector2<>);
};