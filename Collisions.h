#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>

static class Collisions
{
public:
	static bool IntersectCirclePolygon(sf::Vector2f circleCenter, float radius, std::vector<sf::Vector2f> Verteces,float *depth, sf::Vector2f *normal);
	static bool IntersectCircles(sf::Vector2f centerA, float radiusA, sf::Vector2f centerB, float radiusB, float* depth, sf::Vector2f* normal);
	static bool IntersectPolygons(std::vector<sf::Vector2f> polygon1Verteces, std::vector<sf::Vector2f> polygon2Verteces, float* depth, sf::Vector2f* normal);
private:
	static void ProjectVertecies(std::vector<sf::Vector2f> polygon1Verteces,sf::Vector2f axis, float* min, float* max);
	static float distance(sf::Vector2f point1, sf::Vector2f point2);
	static sf::Vector2f normalize(sf::Vector2f v);
	static float dot(sf::Vector2f v1, sf::Vector2f v2);
	static float vectorLength(const sf::Vector2f& vec);
	static sf::Vector2f FindArithmeticMean(std::vector<sf::Vector2f> verteces);
	static void ProjectCircle(sf::Vector2f centre,float radius, sf::Vector2f axis, float *min,float *max);
	static int FindClosestPointOnPolygon(sf::Vector2f circleCentre, std::vector<sf::Vector2f> verteces);
};

