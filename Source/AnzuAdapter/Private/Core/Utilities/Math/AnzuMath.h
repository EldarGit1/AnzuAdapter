#pragma once

#include "AnzuAdapter/Private/Core/Utilities/Math/Vec2.h"
#include "AnzuAdapter/Private/Core/Utilities/Math/Vec3.h"

#include <vector>

namespace anzu
{
    class AnzuMath
    {
    public:
        static float GetPolygonScreenCoverage(const std::vector<Vec2>& polygonPoints, const std::vector<Vec2>& screenBounds);
        static std::vector<Vec3> GenerateRaycastGridPoints(const std::vector<Vec3>& placementCorners);
        static unsigned short GetRaycastScanWidth() { return _raycastScanWidth; };
        static unsigned short GetRaycastScanHeight() { return _raycastScanHeight; };

        AnzuMath() = delete;

    private:
        static const unsigned short _raycastScanWidth = 4;
        static const unsigned short _raycastScanHeight = 4;

        static std::vector<Vec2>  clipPolygonToBounds(const std::vector<Vec2>& polygon, const std::vector<Vec2>& bounds);
        static std::vector<Vec2>  clipAgainstEdge(const std::vector<Vec2>& polygon, const Vec2& edgeA, const Vec2& edgeB);
        static bool isInside(const Vec2& point, const Vec2& edgeA, const Vec2& edgeB);
        static Vec2 getIntersection(const Vec2& pointA, const Vec2& pointB, const Vec2& edgeA, const Vec2& edgeB);
        static float computePolygonArea(const std::vector<Vec2>& polygon);
    };
}
