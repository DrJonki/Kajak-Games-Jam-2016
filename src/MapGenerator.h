#pragma once

#include <Jopnal/Jopnal.hpp>

class MapGenerator
{

private:
	jop::WeakReference<jop::Object> m_object;
	std::vector<std::vector<glm::vec2>> map;
	glm::vec2 tile;
	float tileSize = 6.f;
	int mapSize = 10;

	void createTiles(jop::Scene &scene, std::vector<std::vector<glm::vec2>> map)
	{
		using namespace jop;
		typedef ResourceManager rm;
		auto vertiCalCenter = map[map.size() / 2];
		auto centerTile = vertiCalCenter[vertiCalCenter.size() / 2];

		for (int i = 0; i < map.size(); i++)
		{
			for (int j = 0; j < map[i].size(); j++) 
			{

				scene.createChild("tile_" + std::to_string(i) + "_" + std::to_string(j))->createComponent<Sprite>(scene.getRenderer()).setSize(glm::vec2(tileSize)).setTexture(rm::get<Texture2D>("car.jpg"), false);
				scene.findChild("tile_" + std::to_string(i) + "_" + std::to_string(j))->setPosition(map[i][j].x, map[i][j].y, -0.1f).addTag("house");
				if (map[i][j].x == centerTile.x || map[i][j].y == centerTile.y)
				{

					scene.findChild("tile_" + std::to_string(i) + "_" + std::to_string(j))->clearTags().addTag("road");
					scene.findChild("tile_" + std::to_string(i) + "_" + std::to_string(j))->getComponent<Sprite>()->setTexture(rm::get<Texture2D>("road.jpg"), false);
				}
			}
		}

	}

	void createWallCollisions(jop::Scene &scene, std::vector<std::vector<glm::vec2>> map)
	{
		using namespace jop;
		typedef ResourceManager rm;
		auto houses = scene.findChildrenWithTag("house",false);

		for (int i = 0; i < houses.size(); i++)
		{
			houses[i]->createComponent<RigidBody2D>(scene.getWorld<2>(), RigidBody2D::ConstructInfo2D(rm::getNamed<RectangleShape2D>("house", tileSize*0.9f, tileSize*0.9f), RigidBody::Type::Static, 1.f));
		}
	}

public:
	MapGenerator(jop::Scene &scene) 
	{
		glm::vec2 startPos = glm::vec2((-mapSize*tileSize)/2, (mapSize*tileSize)/2);
		for (int i = 1; i < mapSize; i++) 
		{
			std::vector<glm::vec2> tempVec;
			for (int j = 1; j < mapSize; j++)
			{
				tempVec.push_back(glm::vec2(j * tileSize + startPos.x, -i * tileSize + startPos.y));
			}
			map.push_back(tempVec);
		}

		createTiles(scene, map);
		createWallCollisions(scene, map);
	}
};

