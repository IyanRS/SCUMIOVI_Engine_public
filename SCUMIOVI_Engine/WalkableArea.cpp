#include "WalkableArea.hpp"

#include <queue>
#include <unordered_set>

bool WalkableArea::isPointInside(Vector2<> point) {
    if (walkablePolygon.size() < 3) {
        // TODO: Log error, impossible polygon
        return false;
    }

    unsigned int n = walkablePolygon.size();
    bool inside = false;

    for (unsigned int i = 0; i < n; i++) {
        Vector2<> v1 = walkablePolygon[i];
        Vector2<> v2 = walkablePolygon[(i + 1) % n];
        // TODO: handle division by 0
        if (((v1.y > point.y) != (v2.y > point.y)) && (point.x < (v2.x - v1.x) * (point.y - v1.y) / (v2.y - v1.y) + v1.x)) {
            inside = !inside;
        }
    }

    return inside;
}

Vector2<> WalkableArea::getClosestPointToSegment(Vector2<> segmentStart, Vector2<> segmentEnd, Vector2<> point) {
    Vector2<> segment = Vector2<>(segmentEnd.x - segmentStart.x, segmentEnd.y - segmentStart.y);
    Vector2<> pointRelativeToSegment = Vector2<>(point.x - segmentStart.x, point.y - segmentStart.y);

    float segmentLength = segmentStart.distanceTo(segmentEnd);
    float segmentLengthSquared = segmentLength * segmentLength;

    if (segmentLengthSquared == 0) { // If the segment is a single dot (unlikely)
        return segmentStart;
    }
    float projection = pointRelativeToSegment.dot(segment) / segmentLengthSquared;
    projection = projection < 0 ? 0 : projection > 1 ? 1 : projection;
    Vector2<> closest_point = segmentStart + segment * projection;
    return closest_point;
}


bool WalkableArea::lineIntersectsLine(Vector2<> p1, Vector2<> q1, Vector2<> p2, Vector2<> q2) {
    auto orientation = [](Vector2<> p, Vector2<> q, Vector2<> r) -> int {
        float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
        if (val == 0) return 0;
        return (val > 0) ? 1 : 2;
        };

    auto onSegment = [](Vector2<> p, Vector2<> q, Vector2<> r) -> bool {
        return (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y));
        };

    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if (o1 != o2 && o3 != o4) return true;

    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false;  // Otherwise they don't intersect
}


bool WalkableArea::lineIntersectsPolygon(Vector2<> lineStart, Vector2<> lineEnd) {
    unsigned int n = walkablePolygon.size();
    for (unsigned int i = 0; i < n; i++) {
        Vector2<> p1 = walkablePolygon[i];
        Vector2<> p2 = walkablePolygon[(i + 1) % n];

        if (lineIntersectsLine(lineStart, lineEnd, p1, p2)) {
            return true;
        }
    }
    return false;
}

Vector2<> WalkableArea::getClosestPoint(Vector2<> initialPoint) {
    if (isPointInside(initialPoint))
        return initialPoint;

    Vector2<> closestPoint;
    float min_distance = INFINITY;

    for (int i = 0; i < walkablePolygon.size(); i++) {
        Vector2<> p1 = walkablePolygon[i];
        Vector2<> p2 = walkablePolygon[(i + 1) % walkablePolygon.size()];
        Vector2<> projection = getClosestPointToSegment(p1, p2, initialPoint);

        float distance = initialPoint.distanceTo(projection);
        if (distance < min_distance) {
            min_distance = distance;
            closestPoint = projection;
        }
    }

    return closestPoint;
}

#include "Game.hpp"

void WalkableArea::showDebug() {
    Game& game = Game::getInstance();
    Scene& scene = game.getScene();
    for (int i = 1; i < walkablePolygon.size(); i++) {
        scene.addDebugLine(walkablePolygon[i-1], walkablePolygon[i]);
    }
    scene.addDebugLine(walkablePolygon[0], walkablePolygon[walkablePolygon.size() - 1]);
}

WalkableArea::WalkableArea(std::vector<Vector2<>> verteces) {
    walkablePolygon = verteces;
}

WalkableArea::WalkableArea(std::initializer_list<Vector2<>> verteces) {
    for (Vector2<> vertex : verteces) {
        walkablePolygon.push_back(vertex);
    }
}

#include <SDL3/SDL.h>

struct ComparePair {
    bool operator()(const std::pair<float, Vector2<>>& a, const std::pair<float, Vector2<>>& b) const {
        return a.first > b.first; // We want the priority queue to give priority to the smallest distance
    }
};

int WalkableArea::getNIntersections(Vector2<> a, Vector2<> b) {
    int nIntersections = 0;
    unsigned int n = walkablePolygon.size();
    for (unsigned int i = 0; i < n; i++) {
        Vector2<> p1 = walkablePolygon[i];
        Vector2<> p2 = walkablePolygon[(i + 1) % n];

        if (lineIntersectsLine(a, b, p1, p2)) {
            nIntersections++;
        }
    }
    return nIntersections;
}

bool WalkableArea::isVisible(Vector2<> a, Vector2<> b) {
    // Check if a line from point 'a' to point 'b' intersects the polygon
    return !lineIntersectsPolygon(a, b);
}

std::vector<Vector2<>> WalkableArea::findPath(Vector2<> start, Vector2<> end) {
    if (isVisible(start, end))
        return {end};

    std::vector<Vector2<>> path;

    std::vector<Vector2<>> vertices = walkablePolygon;

    std::unordered_map<Vector2<>, std::vector<Vector2<>>, Vector2Hash> visibilityGraph;

    Game& game = Game::getInstance();

    for (size_t i = 0; i < vertices.size(); ++i) {
        for (size_t j = i + 1; j < vertices.size(); ++j) {
            Vector2<> a = vertices[i];
            Vector2<> b = vertices[j];

            //SDL_Log("N intersections: %d\n", getNIntersections(a, b));
            // Check if the points are visible from each other
            if (getNIntersections(a, b) <= 3) {
                //game.getScene().addDebugLine(a, b);
                visibilityGraph[a].push_back(b);
                visibilityGraph[b].push_back(a);
            }
        }
    }

    // Step 3: Add the start and end points to the graph and check visibility to the vertices
    for (size_t i = 0; i < vertices.size(); ++i) {
        Vector2<> vertex = vertices[i];
        if (getNIntersections(start, vertex) <= 2) {
            //game.getScene().addDebugLine(start, vertex);
            visibilityGraph[start].push_back(vertex);
            visibilityGraph[vertex].push_back(start);
        }
        if (getNIntersections(end, vertex) <= 2) {
            //game.getScene().addDebugLine(end, vertex);
            visibilityGraph[end].push_back(vertex);
            visibilityGraph[vertex].push_back(end);
        }
    }

    // Step 4: Find the shortest path using Dijkstra's algorithm or BFS.
    // Here we'll use Dijkstra for simplicity.

    std::unordered_map<Vector2<>, float, Vector2Hash> distances;
    std::unordered_map<Vector2<>, Vector2<>, Vector2Hash> previous;
    std::priority_queue<std::pair<float, Vector2<>>, std::vector<std::pair<float, Vector2<>>>, ComparePair> pq;

    // Initialize distances
    for (const auto& vertex : vertices) {
        distances[vertex] = std::numeric_limits<float>::infinity();
    }

    distances[end] = std::numeric_limits<float>::infinity();

    distances[start] = 0;
    pq.push({ 0, start });

    while (!pq.empty()) {
        Vector2<> current = pq.top().second;
        pq.pop();

        // If we've reached the target point, reconstruct the path
        if (current == end) {
            Vector2<> temp = end;
            while (temp != start) {
                path.push_back(temp);
                //game.getScene().addDebugLine(temp, previous[temp]);
                temp = previous[temp];
            }
            //path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (const Vector2<>& neighbor : visibilityGraph[current]) {
            float newDist = distances[current] + current.distanceTo(neighbor);
            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                previous[neighbor] = current;
                pq.push({ newDist, neighbor });
            }
        }
    }

    // If no path was found, return an empty path
    return path;
}