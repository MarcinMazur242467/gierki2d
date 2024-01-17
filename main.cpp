#include <fstream>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <sstream>
#include <chrono>
#include <thread>
#include <cstdlib> 

#include "Collisions.h"
#include <vector>

const float MinFov = 400.f;
const float MaxFov = 100.f;
const float CameraSmooth = 0.04f;
const float acc = 1.1;
bool restart = true;
bool circleWin = false;
bool squareWin = false;


class Rectangle
{
private:
	void updateVerteces()
	{
		Vertices.clear();
		sf::Vector2f globalPoint0 = Square.getTransform().transformPoint(Square.getPoint(0));
		sf::Vector2f globalPoint1 = Square.getTransform().transformPoint(Square.getPoint(1));
		sf::Vector2f globalPoint2 = Square.getTransform().transformPoint(Square.getPoint(2));
		sf::Vector2f globalPoint3 = Square.getTransform().transformPoint(Square.getPoint(3));

		Vertices.push_back(globalPoint0);
		Vertices.push_back(globalPoint1);
		Vertices.push_back(globalPoint2);
		Vertices.push_back(globalPoint3);
	}

public:
	std::vector<sf::Vector2f> Vertices;
	sf::RectangleShape Square;

	Rectangle(const sf::Vector2f size, const sf::Vector2f initialPosition)
	{
		Square.setSize(size);
		Square.setPosition(initialPosition);
		updateVerteces();
	}

	void Move(sf::Vector2f value)
	{
		Square.move(value);
		updateVerteces();
	}

	void Rotate(float angle)
	{
		Square.rotate(angle);
		updateVerteces();
	}
	void SetPosition(sf::Vector2f pos)
	{
		Square.setPosition(pos);
		updateVerteces();
	}
};


float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}


void renderMap(const std::string& mapPath, const std::string& texturePath, sf::RenderWindow& window,
               std::vector<Rectangle>& borders)
{
	std::ifstream file(mapPath);
	if (!file.is_open())
	{
		std::cerr << "Failed to open text file" << std::endl;
		exit(1);
	}

	std::string fileContent;
	std::string line;

	while (std::getline(file, line))
	{
		fileContent += line + "\n";
	}

	file.close();

	sf::Vector2f borderPos(0.0, 0.0);
	const float blockSize = 32;

	for (char c : fileContent)
	{
		if (c == '-')
		{
			Rectangle border(sf::Vector2f(32, 32), borderPos);
			borders.push_back(border);
		}

		borderPos.x += blockSize;
		if (c == '\n')
		{
			borderPos.x = 0;
			borderPos.y += blockSize;
		}
	}

	for (const Rectangle& border : borders)
	{
		window.draw(border.Square);
	}
}

std::random_device rd;
std::default_random_engine generator(rd());
std::uniform_real_distribution<double> distribution(50.0f, 440.0f);


int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "SFML Collision Detection");
	window.setFramerateLimit(60);

	sf::View camera = window.getDefaultView();
	sf::Clock clock;

	std::vector<Rectangle> borders;


	sf::CircleShape circle(5);
	circle.setFillColor(sf::Color::Green);
	circle.setOrigin(circle.getRadius(), circle.getRadius()); // Set origin to center
	circle.setPosition(sf::Vector2f(distribution(generator), distribution(generator)));


	Rectangle rect1(sf::Vector2f(10, 10), sf::Vector2f(distribution(generator), distribution(generator)));
	rect1.Square.setFillColor(sf::Color::Red);

	Rectangle target(sf::Vector2f(25, 25), sf::Vector2f(distribution(generator), distribution(generator)));
	target.Square.setFillColor(sf::Color::Yellow);
	int circleWins = 0;
	int squareWins = 0;
	int i = 0;

	sf::VertexArray arrow(sf::Triangles, 4);



	while (window.isOpen())
	{

		

		if(restart)
		{
			system("CLS");
			if (squareWin)
			{
				squareWins++;
				std::cout << "Square won!" << std::endl;
				squareWin = false;

			} 
				
			if (circleWin)
			{
				circleWins++;
				std::cout << "Circle won!" << std::endl;
				circleWin = false;
			} 
			if(circleWins == 3)
			{
				std::cout << "Game over! CIRCLE WON!" << std::endl;
				i = 0;
				circleWins = 0;
				squareWins = 0;
				for (int i = 0; i < 5; i++)
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));
					std::cout << 5 - i << std::endl;
				}
				system("CLS");

			}
			if(squareWins==3)
			{
				std::cout << "Game over! SQUARE WON!" << std::endl;
				i = 0;
				circleWins = 0;
				squareWins = 0;
				for (int i = 0; i < 5; i++)
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));
					std::cout << 5 - i << std::endl;
				}
				system("CLS");
			}

			

			i++;
			std::cout << "Cirlce wins:" << circleWins << std::endl;
			std::cout <<"Square wins:"<< squareWins << std::endl;
			
			rect1.SetPosition(sf::Vector2f(distribution(generator), distribution(generator)));
			borders.clear();
			std::ostringstream oss;
			oss << "Assets/map" << i <<".txt";
			std::string s = oss.str();
			std::cout << s<< std::endl;
			renderMap(s, "Assets/cobblestone.png", window, borders);

			sf::Vector2f normal = sf::Vector2f(0, 0);
			float depth = 0;
			bool targetCollidesWithBorder = true;
			do
			{
				target.SetPosition(sf::Vector2f(distribution(generator), distribution(generator)));
				for (auto border : borders)
				{
					targetCollidesWithBorder = Collisions::IntersectPolygons(target.Vertices, border.Vertices, &depth, &normal);
					if (targetCollidesWithBorder) break;
				}
			
			} while (targetCollidesWithBorder);
			
			bool circlePlayerCollidesWithBorder = true;
			do
			{
				circle.setPosition(sf::Vector2f(distribution(generator), distribution(generator)));
				sf::Vector2f center = circle.getPosition();
				for (auto border : borders)
				{
					circlePlayerCollidesWithBorder = Collisions::IntersectCirclePolygon(center, circle.getRadius(), border.Vertices, &depth, &normal);
					if (circlePlayerCollidesWithBorder) break;
				}
			
			} while (circlePlayerCollidesWithBorder);
			
			bool squarePlayerCollidesWithBorder = true;
			do
			{
				rect1.SetPosition(sf::Vector2f(distribution(generator), distribution(generator)));
				for (auto border : borders)
				{
					squarePlayerCollidesWithBorder = Collisions::IntersectPolygons(rect1.Vertices, border.Vertices, &depth, &normal);
					if (squarePlayerCollidesWithBorder) break;
			
				}
			
			} while (squarePlayerCollidesWithBorder);
			
			restart = false;
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}


		window.clear(sf::Color(32, 32, 32));

		window.setView(camera);

		sf::Vector2f playerPosMean = (circle.getPosition() + rect1.Square.getPosition()) / 2.0f;
		float distance = std::sqrt(
			std::pow(circle.getPosition().x - rect1.Square.getPosition().x, 2) + std::pow(
				circle.getPosition().y - rect1.Square.getPosition().y, 2));
		float targetFov = lerp(MinFov, MaxFov, 1 - (distance / 200.f));
		float currentFov = camera.getSize().x;
		currentFov = lerp(currentFov, targetFov, CameraSmooth);
		camera.setSize(currentFov, currentFov * (window.getSize().y / window.getSize().x));
		camera.setCenter(playerPosMean);
		window.setView(camera);

		
		sf::Vector2f sizePlayer = rect1.Square.getSize();
		sf::Vector2f sizeTarget = target.Square.getSize();

		sf::Vector2f centerPlayer = rect1.Square.getPosition() + sizePlayer / 2.0f;
		sf::Vector2f centerTarget = target.Square.getPosition() + sizeTarget / 2.0f;

		arrow[0].position = sf::Vector2f(circle.getPosition());
		arrow[0].color = sf::Color::Green;
		arrow[1].position = sf::Vector2f(centerPlayer);
		arrow[1].color = sf::Color::Red;
		arrow[2].position = sf::Vector2f(centerTarget);
		arrow[2].color = sf::Color::Yellow;
		arrow[3].position = sf::Vector2f(centerTarget);
		arrow[2].color = sf::Color::Yellow;

		sf::Vector2f normal = sf::Vector2f(0, 0);
		float depth = 0;
		sf::Vector2f center = circle.getPosition();


		for (Rectangle border : borders)
		{
			if (Collisions::IntersectCirclePolygon(center, circle.getRadius(), border.Vertices, &depth, &normal))
			{
				circle.move(-normal * depth);
			}
			if (Collisions::IntersectPolygons(rect1.Vertices, border.Vertices, &depth, &normal))
			{
				rect1.Move(-normal * depth);
			}
			if (Collisions::IntersectPolygons(target.Vertices, border.Vertices, &depth, &normal))
			{
				target.Move(-normal * depth);
			}
			if (Collisions::IntersectPolygons(target.Vertices, rect1.Vertices, &depth, &normal))
			{
				squareWin = true;
				restart = true;
			}
			if(Collisions::IntersectCirclePolygon(center,circle.getRadius(),target.Vertices,&depth,&normal))
			{
				restart = true;
				circleWin = true;

			}
		}


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) rect1.Move(sf::Vector2f(0, -acc));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))rect1.Move(sf::Vector2f(0, acc));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) rect1.Move(sf::Vector2f(-acc, 0));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) rect1.Move(sf::Vector2f(acc, 0));

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) circle.move(sf::Vector2f(0, -acc));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))circle.move(sf::Vector2f(0, acc));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))circle.move(sf::Vector2f(-acc, 0));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))circle.move(sf::Vector2f(acc, 0));


		window.clear();
		window.draw(circle);
		window.draw(rect1.Square);
		window.draw(arrow);
		window.draw(target.Square);

		for (Rectangle border : borders)
		{
			border.Square.setFillColor(sf::Color(128, 128, 128, 128));
			window.draw(border.Square);
		}

		window.display();
	}

	return 0;
}
