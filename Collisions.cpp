#include <SFML/System/Vector2.hpp>
#include <cmath>

#include "Collisions.h"
#include <SFML/Window/Clipboard.hpp>

using namespace sf;


bool Collisions::IntersectCircles(Vector2f centerA, float radiusA, Vector2f centerB, float radiusB, float* depth,
                                  Vector2f* normal)
{
	*depth = 0;
	*normal = Vector2f(0.f, 0.f);

	float dist = distance(centerA, centerB);
	float radii = radiusA + radiusB;

	if (dist >= radii)
	{
		return false;
	}
	*depth = radii - dist;
	*normal = normalize(centerB - centerA);
	return true;
}

bool Collisions::IntersectPolygons(std::vector<sf::Vector2f> polygon1Verteces,
                                   std::vector<sf::Vector2f> polygon2Verteces,
                                   float* depth, Vector2f* normal)
{
	*depth = std::numeric_limits<float>::max();
	*normal = Vector2f(0.f, 0.f);

	for (int i = 0; i < polygon1Verteces.size(); i++)
	{
		sf::Vector2f va = polygon1Verteces.at(i);
		sf::Vector2f vb = polygon1Verteces.at((i + 1) % polygon1Verteces.size());

		sf::Vector2f edge = vb - va;
		sf::Vector2f axis = sf::Vector2f(edge.y, -edge.x);

		float min1 = std::numeric_limits<float>::max();
		float max1 = std::numeric_limits<float>::min();
		float min2 = std::numeric_limits<float>::max();
		float max2 = std::numeric_limits<float>::min();

		Collisions::ProjectVertecies(polygon1Verteces, axis, &min1, &max1);
		Collisions::ProjectVertecies(polygon2Verteces, axis, &min2, &max2);

		if (min1 >= max2 || min2 >= max1)
		{
			return false;
		}

		float axisDepth = std::min(max2 - min1, max1 - min2);
		if (axisDepth < *depth)
		{
			*depth = axisDepth;
			*normal = axis;
		}
	}
	for (int i = 0; i < polygon2Verteces.size(); i++)
	{
		sf::Vector2f va = polygon2Verteces.at(i);
		sf::Vector2f vb = polygon2Verteces.at((i + 1) % polygon2Verteces.size());

		sf::Vector2f edge = vb - va;
		sf::Vector2f axis = sf::Vector2f(-edge.y, edge.x);


		float min1 = std::numeric_limits<float>::max();
		float max1 = std::numeric_limits<float>::min();
		float min2 = std::numeric_limits<float>::max();
		float max2 = std::numeric_limits<float>::min();

		Collisions::ProjectVertecies(polygon1Verteces, axis, &min1, &max1);
		Collisions::ProjectVertecies(polygon2Verteces, axis, &min2, &max2);
		if (min1 >= max2 || min2 >= max1)
		{
			return false;
		}

		float axisDepth = std::min(max2 - min1, max1 - min2);
		if (axisDepth < *depth)
		{
			*depth = axisDepth;
			*normal = axis;
		}
	}
	*depth /= vectorLength(*normal);
	*normal = normalize(*normal);

	sf::Vector2f center1 = Collisions::FindArithmeticMean(polygon1Verteces);
	sf::Vector2f center2 = Collisions::FindArithmeticMean(polygon2Verteces);

	sf::Vector2f direction = center2 - center1;
	if (Collisions::dot(direction, *normal) < 0.f)
	{	
		*normal = -*normal;
	}

	return true;
}

void Collisions::ProjectVertecies(std::vector<sf::Vector2f> verteces, sf::Vector2f axis, float* mini, float* maxi)
{
	*mini = std::numeric_limits<float>::max();
	*maxi = std::numeric_limits<float>::min();

	for (int i = 0; i < verteces.size(); i++)
	{
		sf::Vector2f v = verteces.at(i);
		float proj = dot(v, axis);
		if (proj < *mini) *mini = proj;
		if (proj > *maxi) *maxi = proj;
	}
}

float Collisions::distance(sf::Vector2f point1, sf::Vector2f point2)
{
	return sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2));
}

Vector2f Collisions::normalize(sf::Vector2f v)
{
	float length = sqrt(v.x * v.x + v.y * v.y);
	if (length != 0)
	{
		v.x = v.x / length;
		v.y = v.y / length;
	}
	return v;
}

float Collisions::vectorLength(const sf::Vector2f& vec)
{
	return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

float Collisions::dot(sf::Vector2f v1, sf::Vector2f v2)

{
	return v1.x * v2.x + v1.y * v2.y;
}

sf::Vector2f Collisions::FindArithmeticMean(std::vector<sf::Vector2f> verteces)
{
	float sumX = 0.f;
	float sumY = 0.f;
	for (int i = 0; i < verteces.size(); i++)
	{
		sf::Vector2f v = verteces.at(i);
		sumX += v.x;
		sumY += v.y;
	}
	return sf::Vector2f(sumX / float(verteces.size()), sumY / float(verteces.size()));
}

bool Collisions::IntersectCirclePolygon(sf::Vector2f circleCenter, float circleRadius,
	std::vector<sf::Vector2f> vertices,
	float * depth, sf::Vector2f* normal)
{
	*normal = sf::Vector2f(0,0);
	*depth = std::numeric_limits<float>::max();

	sf::Vector2f axis = sf::Vector2f(0, 0);
	float axisDepth = 0.f;
	float minA, maxA, minB, maxB;

	for (int i = 0; i < vertices.size(); i++)
	{
		sf::Vector2f va = vertices[i];
		sf::Vector2f vb = vertices[(i + 1) % vertices.size()];

		sf::Vector2f edge = vb - va;
		axis = sf::Vector2f(edge.y, -edge.x);
		axis = Collisions::normalize(axis);

		Collisions::ProjectVertecies(vertices, axis,&minA, &maxA);
		Collisions::ProjectCircle(circleCenter, circleRadius, axis,  &minB, &maxB);

		if (minA >= maxB || minB >= maxA)
		{
			return false;
		}

		axisDepth = std::min(maxB - minA, maxA - minB);

		if (axisDepth < *depth)
		{
			*depth = axisDepth;
			*normal = axis;
		}
	}

	int cpIndex = Collisions::FindClosestPointOnPolygon(circleCenter, vertices);
	sf::Vector2f cp = vertices[cpIndex];

	axis = cp - circleCenter;
	axis = Collisions::normalize(axis);


	Collisions::ProjectVertecies(vertices, axis, &minA, &maxA);
	Collisions::ProjectCircle(circleCenter, circleRadius, axis, &minB, &maxB);

	if (minA >= maxB || minB >= maxA)
	{
		return false;
	}

	axisDepth = std::min(maxB - minA, maxA - minB);

	if (axisDepth <*depth)
	{
		*depth = axisDepth;
		*normal = axis;
	}

	sf::Vector2f polygonCenter = Collisions::FindArithmeticMean(vertices);

	sf::Vector2f direction = polygonCenter - circleCenter;

	if (Collisions::dot(direction, *normal) < 0.0f)
	{
		*normal = -*normal;
	}

	return true;
}

  int Collisions::FindClosestPointOnPolygon(sf::Vector2f circleCenter,std::vector<sf::Vector2f> vertices)
{
	int result = -1;
	float minDistance = std::numeric_limits<float>::max();

	for (int i = 0; i < vertices.size(); i++)
	{
		sf::Vector2f v = vertices[i];
		float distance = Collisions::distance(v, circleCenter);

		if (distance < minDistance)
		{
			minDistance = distance;
			result = i;
		}
	}

	return result;
}

  void Collisions::ProjectCircle(sf::Vector2f center, float radius, sf::Vector2f axis,float *min,float *max)
{
	  sf::Vector2f direction = Collisions::normalize(axis);
	  sf::Vector2f directionAndRadius = direction * radius;

	  sf::Vector2f p1 = center + directionAndRadius;
	  sf::Vector2f p2 = center - directionAndRadius;

	*min = Collisions::dot(p1, axis);
	*max = Collisions::dot(p2, axis);

	if (*min > *max)
	{
		// swap the min and max values.
		float t = *min;
		*min = *max;
		*max = t;
	}
}