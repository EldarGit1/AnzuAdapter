#include "AnzuMath.h"

namespace anzu
{
    float AnzuMath::GetPolygonScreenCoverage(
         const std::vector<Vec2>& polygonPoints,
         const std::vector<Vec2>& screenBounds)
    {
        std::vector<Vec2> clippedPolygon = clipPolygonToBounds(polygonPoints, screenBounds);
        float polygonArea = computePolygonArea(clippedPolygon);
        float screenArea = computePolygonArea(screenBounds);
        float ret = 0;

        if (screenArea > 0)
        {
            ret = polygonArea / screenArea;
        }

        return ret;
    }

    std::vector<Vec3> AnzuMath::GenerateRaycastGridPoints(
      const std::vector<Vec3>& placementCorners)
    {
        std::vector<Vec3> raycastPoints(_raycastScanWidth * _raycastScanHeight);
        Vec3 placementWidth = placementCorners[1] - placementCorners[0];
        Vec3 placementHeight = placementCorners[3] - placementCorners[0];
        size_t pointIndex = 0;

        for (size_t y = 0; y < _raycastScanHeight; y++)
        {
            float yPos = ((float)y + 0.5f) / (float)_raycastScanHeight;

            for (size_t x = 0; x < _raycastScanWidth; x++)
            {
                float xPos = ((float)x + 0.5f) / (float)_raycastScanWidth;
                Vec3 pointOnPlacement = placementWidth * xPos + placementCorners[0] + placementHeight * yPos;

                raycastPoints[pointIndex++] = pointOnPlacement;
            }
        }

        return raycastPoints;
    }

    std::vector<Vec2> AnzuMath::clipPolygonToBounds(
        const std::vector<Vec2>& polygon,
        const std::vector<Vec2>& bounds)
    {
        // Based on 'Sutherland Hodgman algorithm'
        // https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
        std::vector<Vec2> ret = polygon;

        for (size_t i = 0; i < bounds.size(); i++)
        {
            Vec2 boundA = bounds[i];
            Vec2 boundB = bounds[(i + 1) % bounds.size()];

            ret = clipAgainstEdge(ret, boundA, boundB);
        }

        return ret;
    }

    std::vector<Vec2> AnzuMath::clipAgainstEdge(
        const std::vector<Vec2>& polygon,
        const Vec2 &edgeA,
        const Vec2 &edgeB)
    {
        std::vector<Vec2> ret;

        for (size_t i = 0; i < polygon.size(); i++)
        {
            Vec2 current = polygon[i];
            Vec2 prev = polygon[(i - 1 + polygon.size()) % polygon.size()];
            bool isInsideCurrent = isInside(current, edgeA, edgeB);
            bool isInsidePrev = isInside(prev, edgeA, edgeB);

            if (isInsideCurrent)
            {
                if (!isInsidePrev)
                {
                    ret.push_back(
                      getIntersection(
                          prev,
                          current,
                          edgeA,
                          edgeB));
                }

                ret.push_back(current);
            }
            else if (isInsidePrev)
            {
                ret.push_back(
                  getIntersection(
                      prev,
                      current,
                      edgeA,
                      edgeB));
            }
        }

        return ret;
    }

    bool AnzuMath::isInside(
        const Vec2 &point,
        const Vec2 &edgeA,
        const Vec2 &edgeB)
    {
        return ((edgeB.x - edgeA.x) *
                (point.y - edgeA.y) -
                (edgeB.y - edgeA.y) *
                (point.x - edgeA.x)) >= 0;
    }

    Vec2 AnzuMath::getIntersection(
       const Vec2 &pointA,
       const Vec2 &pointB,
       const Vec2 &edgeA,
       const Vec2 &edgeB)
    {
        // Based on the 'Given two points on
        // each line segment' formula algorithm:
        // https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

        Vec2 ret(0, 0);

        float A1 = pointB.y - pointA.y;
        float B1 = pointA.x - pointB.x;
        float C1 = A1 * pointA.x + B1 * pointA.y;

        float A2 = edgeB.y - edgeA.y;
        float B2 = edgeA.x - edgeB.x;
        float C2 = A2 * edgeA.x + B2 * edgeA.y;

        float det = A1 * B2 - A2 * B1;

        if (det != 0) {
            ret.x = (B2 * C1 - B1 * C2) / det;
            ret.y = (A1 * C2 - A2 * C1) / det;
        }

        return ret;
    }

    float AnzuMath::computePolygonArea(const std::vector<Vec2>& polygon)
    {
        // Based on 'Shoelace formula'
        // https://en.wikipedia.org/wiki/Shoelace_formula
        float ret = 0;

        for (size_t i = 0; i < polygon.size(); i++)
        {
            Vec2 p1 = polygon[i];
            Vec2 p2 = polygon[(i + 1) % polygon.size()];

            ret += p1.x * p2.y - p2.x * p1.y;
        }

        return abs(ret) / 2;
    }
}
