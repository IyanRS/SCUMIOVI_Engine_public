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

#define A_STAR_STEP 5.0f
#define MAXIMUM_TARGET_DISTANCE A_STAR_STEP+1

#include <SDL3/SDL.h>

//// A* for pathfinding
//
//struct Node {
//    Vector2<> point;  // Position of the node
//    float g;           // Cost from the start node to this node
//    float h;           // Heuristic estimate from this node to the target
//    float f;           // Total cost (f = g + h)
//    Node* parent;      // Parent node for path reconstruction
//
//    Node(Vector2<> pt, float gCost, float hCost, Node* p)
//        : point(pt), g(gCost), h(hCost), f(gCost + hCost), parent(p) {
//    }
//
//    bool operator>(const Node& other) const {
//        return f > other.f;  // Priority queue should give priority to the lowest f cost
//    }
//};
//
//// Standard Euclidean distance heuristic
//float calculateHeuristic(Vector2<> current, Vector2<> target) {
//    return current.distanceTo(target);  // Euclidean distance
//}

//// Pathfinding function for A*
//std::vector<Vector2<>> WalkableArea::findPath(Vector2<> start, Vector2<> target) {
//    std::vector<Vector2<>> path;
//
//    if (!isPointInside(start)) {
//        return path;
//    }
//
//    if (!isPointInside(target)) {
//        target = getClosestPoint(target);
//    }
//
//    std::priority_queue<Node*, std::vector<Node*>, std::greater<Node*>> openList;
//    std::unordered_set<Vector2<>, Vector2Hash> visited;  // Use unordered_set for faster comparison
//
//    Node* startNode = new Node(start, 0.0f, calculateHeuristic(start, target), nullptr);
//    openList.push(startNode);
//    visited.insert(start);  // Track visited points directly
//
//    std::vector<Vector2<> > directions = {
//        Vector2<>(A_STAR_STEP, 0),   // Right
//        Vector2<>(-A_STAR_STEP, 0),  // Left
//        Vector2<>(0, A_STAR_STEP),   // Up
//        Vector2<>(0, -A_STAR_STEP),  // Down
//        Vector2<>(A_STAR_STEP, A_STAR_STEP),  // Diagonal up-right
//        Vector2<>(A_STAR_STEP, -A_STAR_STEP), // Diagonal down-right
//        Vector2<>(-A_STAR_STEP, A_STAR_STEP), // Diagonal up-left
//        Vector2<>(-A_STAR_STEP, -A_STAR_STEP) // Diagonal down-left
//    };
//
//    while (!openList.empty()) {
//        Node* currentNode = openList.top();
//        openList.pop();
//
//        // Check if we've reached the target
//        if (currentNode->point.distanceTo(target) < MAXIMUM_TARGET_DISTANCE) {
//            while (currentNode != nullptr) {
//                path.push_back(currentNode->point);
//                currentNode = currentNode->parent;
//            }
//            std::reverse(path.begin(), path.end());  // Reverse the path to start -> target
//            return path;
//        }
//
//        // Try expanding in all directions
//        for (const Vector2<>& dir : directions) {
//            Vector2<> nextPoint = currentNode->point + dir;
//
//            if (isPointInside(nextPoint) && visited.find(nextPoint) == visited.end()) {
//                visited.insert(nextPoint);
//
//                float gCost = currentNode->g + currentNode->point.distanceTo(nextPoint);
//                float hCost = calculateHeuristic(nextPoint, target);  // Euclidean distance heuristic
//                Node* nextNode = new Node(nextPoint, gCost, hCost, currentNode);
//
//                openList.push(nextNode);
//            }
//        }
//    }
//
//    SDL_Log("Path could not be found.\n");
//    return path;  // Return empty path if no path is found
//}


////Visibility graph algorithm
//// Pathfinding function for A*
//std::vector<Vector2<>> WalkableArea::findPath(Vector2<> start, Vector2<> target) {
//    std::vector<Vector2<>> path;
//
//    if (!isPointInside(start)) {
//        return path;
//    }
//
//    if (!isPointInside(target)) {
//        target = getClosestPoint(target);
//    }
//
//    if (start == target) {
//        return path;
//    }
//
//    std::priority_queue<Node*, std::vector<Node*>, std::greater<Node*>> openList;
//    std::unordered_set<Vector2<>, Vector2Hash> visited;  // Use unordered_set for faster comparison
//
//    Node* startNode = new Node(start, 0.0f, calculateHeuristic(start, target), nullptr);
//    openList.push(startNode);
//    visited.insert(start);  // Track visited points directly
//
//    std::vector<Vector2<> > directions = {
//        Vector2<>(A_STAR_STEP, 0),   // Right
//        Vector2<>(-A_STAR_STEP, 0),  // Left
//        Vector2<>(0, A_STAR_STEP),   // Up
//        Vector2<>(0, -A_STAR_STEP),  // Down
//        Vector2<>(A_STAR_STEP, A_STAR_STEP),  // Diagonal up-right
//        Vector2<>(A_STAR_STEP, -A_STAR_STEP), // Diagonal down-right
//        Vector2<>(-A_STAR_STEP, A_STAR_STEP), // Diagonal up-left
//        Vector2<>(-A_STAR_STEP, -A_STAR_STEP) // Diagonal down-left
//    };
//
//    while (!openList.empty()) {
//        Node* currentNode = openList.top();
//        openList.pop();
//
//        // Check if we've reached the target
//        if (currentNode->point.distanceTo(target) < MAXIMUM_TARGET_DISTANCE) {
//            while (currentNode != nullptr) {
//                path.push_back(currentNode->point);
//                currentNode = currentNode->parent;
//            }
//            std::reverse(path.begin(), path.end());  // Reverse the path to start -> target
//            return path;
//        }
//
//        // Try expanding in all directions
//        for (const Vector2<>& dir : directions) {
//            Vector2<> nextPoint = currentNode->point + dir;
//
//            if (isPointInside(nextPoint) && visited.find(nextPoint) == visited.end()) {
//                visited.insert(nextPoint);
//
//                float gCost = currentNode->g + currentNode->point.distanceTo(nextPoint);
//                float hCost = calculateHeuristic(nextPoint, target);  // Euclidean distance heuristic
//                Node* nextNode = new Node(nextPoint, gCost, hCost, currentNode);
//
//                openList.push(nextNode);
//            }
//        }
//    }
//
//    SDL_Log("Path could not be found.\n");
//    return path;  // Return empty path if no path is found
//}

//#define PATH_EPSILON 2.5f
//
//// Implement Dijkstra’s Algorithm to find the shortest path in the visibility graph
//std::vector<Vector2<>> WalkableArea::findPath(Vector2<> start, Vector2<> target) {
//
//    if (start.distanceTo(target) < PATH_EPSILON) {
//        return std::vector<Vector2<>>();
//    }
//
//    //if (!isPointInside(start)) {
//    //    return std::vector<Vector2<>>();
//    //}
//
//    //if (!isPointInside(target)) {
//    //    return std::vector<Vector2<>>();
//    //}
//
//    // Visibility graph initialization
//    std::vector<Vector2<>> graphVertices = walkablePolygon;
//    graphVertices.push_back(start);
//    graphVertices.push_back(target);
//
//    // Create an adjacency list to store graph connections
//    std::vector<std::vector<int>> adjacencyList(graphVertices.size());
//
//    // Construct the visibility graph (edges between visible vertices)
//    for (size_t i = 0; i < graphVertices.size(); ++i) {
//        for (size_t j = i + 1; j < graphVertices.size(); ++j) {
//            if (!lineIntersectsPolygon(graphVertices[i], graphVertices[j])) {
//                adjacencyList[i].push_back(j);
//                adjacencyList[j].push_back(i);  // Undirected graph
//            }
//        }
//    }
//
//    // Dijkstra's Algorithm to find the shortest path
//    int n = graphVertices.size();
//    std::vector<float> dist(n, std::numeric_limits<float>::infinity());
//    std::vector<int> prev(n, -1);
//    std::vector<bool> visited(n, false);
//
//    // Min-heap priority queue to store vertices with their distances
//    auto compare = [&dist](int left, int right) { return dist[left] > dist[right]; };
//    std::priority_queue<int, std::vector<int>, decltype(compare)> pq(compare);
//
//    // Start at the start vertex
//    dist[graphVertices.size() - 2] = 0;
//    pq.push(graphVertices.size() - 2);  // The index of the start point
//
//    while (!pq.empty()) {
//        int u = pq.top();
//        pq.pop();
//
//        if (visited[u]) continue;
//        visited[u] = true;
//
//        // Explore neighbors
//        for (int v : adjacencyList[u]) {
//            float alt = dist[u] + graphVertices[u].distanceTo(graphVertices[v]);
//            if (alt < dist[v]) {
//                dist[v] = alt;
//                prev[v] = u;
//                pq.push(v);
//            }
//        }
//    }
//
//    // Reconstruct the path from the target to the start
//    std::vector<Vector2<>> path;
//    int u = graphVertices.size() - 1;  // Target index
//    while (u != -1) {
//        path.push_back(graphVertices[u]);
//        u = prev[u];
//    }
//
//    // Reverse the path to go from start to target
//    std::reverse(path.begin(), path.end());
//
//    for (Vector2<> point : path) {
//        SDL_Log("Point %f, %f", point.x, point.y);
//    }
//
//    return path;
//}

#define PATH_EPSILON 2.5f

// Check if the direct line between start and target is inside the polygon
bool WalkableArea::isDirectPathValid(const Vector2<>& start, const Vector2<>& target, const std::vector<Vector2<>>& polygon) {
    // Check if the direct line from start to target intersects the polygon's edges
    return !lineIntersectsPolygon(start, target);
}

// Implement Dijkstra’s Algorithm to find the shortest path in the visibility graph
std::vector<Vector2<>> WalkableArea::findPath(Vector2<> start, Vector2<> target) {
    if (start.distanceTo(target) < PATH_EPSILON) {
        return { target }; // If the distance is small enough, just return the target
    }

    // Ensure both start and target are inside the polygon
    if (!isPointInside(start) || !isPointInside(target)) {
        return {}; // Return empty path if start or target are not inside the polygon
    }

    // Visibility graph initialization
    std::vector<Vector2<>> graphVertices = walkablePolygon;
    graphVertices.push_back(start);
    graphVertices.push_back(target);

    // Create an adjacency list to store graph connections
    std::vector<std::vector<int>> adjacencyList(graphVertices.size());

    // Construct the visibility graph (edges between visible vertices)
    for (size_t i = 0; i < graphVertices.size(); ++i) {
        for (size_t j = i + 1; j < graphVertices.size(); ++j) {
            // Check if the direct line from graphVertices[i] to graphVertices[j] intersects the polygon
            if (!lineIntersectsPolygon(graphVertices[i], graphVertices[j])) {
                adjacencyList[i].push_back(j);
                adjacencyList[j].push_back(i);  // Undirected graph
            }
        }
    }

    // Debugging: Check the adjacency list and ensure it's being populated correctly
    for (size_t i = 0; i < graphVertices.size(); ++i) {
        SDL_Log("Vertex %d: (%f, %f)", i, graphVertices[i].x, graphVertices[i].y);
        for (int j : adjacencyList[i]) {
            SDL_Log("  Edge to: (%f, %f)", graphVertices[j].x, graphVertices[j].y);
        }
    }

    // Dijkstra's Algorithm to find the shortest path
    int n = graphVertices.size();
    std::vector<float> dist(n, std::numeric_limits<float>::infinity());
    std::vector<int> prev(n, -1);
    std::vector<bool> visited(n, false);

    // Min-heap priority queue to store vertices with their distances
    auto compare = [&dist](int left, int right) { return dist[left] > dist[right]; };
    std::priority_queue<int, std::vector<int>, decltype(compare)> pq(compare);

    // Start at the start vertex (the second-to-last index in graphVertices)
    dist[graphVertices.size() - 2] = 0;
    pq.push(graphVertices.size() - 2);  // The index of the start point

    while (!pq.empty()) {
        int u = pq.top();
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;

        // Explore neighbors
        for (int v : adjacencyList[u]) {
            float alt = dist[u] + graphVertices[u].distanceTo(graphVertices[v]);
            if (alt < dist[v]) {
                dist[v] = alt;
                prev[v] = u;
                pq.push(v);
            }
        }
    }

    // Debugging: Ensure that Dijkstra's algorithm runs correctly
    for (size_t i = 0; i < dist.size(); ++i) {
        SDL_Log("Dist to vertex %d: %f", i, dist[i]);
    }

    // Reconstruct the path from the target to the start
    std::vector<Vector2<>> path;
    int u = graphVertices.size() - 1;  // Target index
    while (u != -1) {
        path.push_back(graphVertices[u]);
        u = prev[u];
    }

    // Reverse the path to go from start to target
    std::reverse(path.begin(), path.end());

    // Debugging: Ensure the reconstructed path is correct
    for (const Vector2<>& point : path) {
        SDL_Log("Path point: (%f, %f)", point.x, point.y);
    }

    // Remove the start point if it is the first point in the path and the path is valid
    if (path.front() == start) {
        path.erase(path.begin());
    }

    return path;
}