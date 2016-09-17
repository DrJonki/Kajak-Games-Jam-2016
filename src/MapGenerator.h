#pragma once

#include <Jopnal/Jopnal.hpp>

class MapGenerator
{

private:
	jop::WeakReference<jop::Object> m_object;
	std::vector<std::vector<glm::vec2>> map;
	glm::vec2 tile;
	float tileSize = 10.f;
	int mapSize = 80;
	int minRoadRange = 4;
	int maxRoadRange = 4;

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
				scene.findChild("tile_" + std::to_string(i) + "_" + std::to_string(j))->setPosition(map[i][j].x, map[i][j].y, -0.1f).addTag("tile");
				if (map[i][j].x == centerTile.x || map[i][j].y == centerTile.y)
				{

					scene.findChild("tile_" + std::to_string(i) + "_" + std::to_string(j))->addTag("road");
					scene.findChild("tile_" + std::to_string(i) + "_" + std::to_string(j))->getComponent<Sprite>()->setTexture(rm::get<Texture2D>("road.png"), false);
				}
			}
		}

	}

	void createWallCollisions(jop::Scene &scene, std::vector<std::vector<glm::vec2>> map)
	{
		using namespace jop;
		typedef ResourceManager rm;
		auto tiles = scene.findChildrenWithTag("tile", false);
		auto vertexPositions = getPositions(tiles);

		for (int i = 0; i < vertexPositions.size(); i++)
		{
			tiles[i]->createComponent<RigidBody2D>(scene.getWorld<2>(), RigidBody2D::ConstructInfo2D(rm::getNamed<RectangleShape2D>("house", tileSize*0.9f, tileSize*0.9f), RigidBody::Type::Static, 1.f));
		}
	}

	std::vector<std::vector<glm::vec2>> getPositions(std::vector<jop::WeakReference<jop::Object>> &tileObjects)
	{
		using namespace jop;
		std::vector<std::vector<glm::vec2>> tempVector;
		for (int i = 0; i < tileObjects.size(); i++)
		{
			if (!tileObjects[i]->hasTag("house"))
			{
				glm::vec2 countedSize = glm::vec2(0);
				for (int j = i; j < mapSize; j++)
				{
					if (tileObjects[j]->hasTag("road") && countedSize.x < 0)
					{
						countedSize.x = j;
					}
				}
				for (int k = i; k < mapSize; k++)
				{
					if (tileObjects[k + mapSize]->hasTag("road") && countedSize.y < 0)
					{
						countedSize.y = k;
					}
				}
				tempVector.push_back(getSingleHousePositions(tileObjects, countedSize, i));
			}
		}
		return tempVector;
	}

	// Tags tiles to "houses" for single big house.
	std::vector<glm::vec2> getSingleHousePositions(std::vector<jop::WeakReference<jop::Object>> &tileObjects, glm::vec2 houseSize, int offset)
	{
		for (int i = offset; i < tileObjects.size(); i++)
		{

		}
	}

	std::vector<std::vector<int>> getRandomRoads()
	{
		using namespace jop;
		std::vector<std::vector<int>> tempVec;
		std::vector<int> xRoads;
		std::vector<int> yRoads;

		for (int i = 0; i < mapSize; i++)
		{
			int numberX = Randomizer().range(i + minRoadRange, i + maxRoadRange);
			xRoads.push_back(numberX);
			i = numberX;
		}

		for (int i = 0; i < mapSize; i++)
		{
			int numberY = Randomizer().range(i + minRoadRange, i + maxRoadRange);
			yRoads.push_back(numberY);
			i = numberY;
		}

		tempVec.push_back(xRoads);
		tempVec.push_back(yRoads);

		return tempVec;
	}

	void createHouses(std::vector<std::vector<bool>> tiles, jop::Scene &scene)
	{
		using namespace jop;
		typedef ResourceManager rm;
		int houseCount = 0;
		auto m_atlas = &rm::getNamed<jop::TextureAtlas>("houseBlocks", glm::uvec2(4096));
		auto roadIndex = m_atlas->getCoordinates(m_atlas->addTexture("road.png"));
		auto houseIndex = m_atlas->getCoordinates(m_atlas->addTexture("car.png"));

		std::vector<jop::Vertex> vertices;

		auto mapObjects = scene.createChild("mapObjects");

		for (int i = 0; i < mapSize; i++)
		{
			for (int j = 0; j < mapSize; j++)
			{
				if (i == 0 || tiles[i - 1][j] != false && tiles[i][j] == false)
				{
					glm::vec2 houseSize = glm::vec2(0);
					glm::vec2 housePosition = glm::vec2(0);
					int yk = i;
					bool end = false;
					for (int k = yk; houseSize.y <= 0; k++)
					{
						if (k < mapSize)
						{
							if (houseSize.y <= 0 && tiles[k][j] == true)
							{
								houseSize.y = (k-i)* tileSize;
							}
						}
						else {
							houseSize.y = (k - i)* tileSize;
							break;
						}
					}
					for (int k = j; houseSize.x <= 0; k++)
					{
						if (k < mapSize)
						{
							if (houseSize.x <= 0 && tiles[i][k] == true)
							{
								houseSize.x = (k-j) * tileSize;
								// Add offset for X
								j = k;
							}
						}
						else {
							houseSize.x = (k - j) * tileSize;
							break;
						}
					}
					// Left upper corner position
					housePosition.x = (j * tileSize) - (tileSize * 2);
					housePosition.y = (i * tileSize) + (tileSize * 3);

					int tileCountX = glm::ceil(houseSize.x / tileSize);
					int tileCountY = glm::ceil(houseSize.y / tileSize);
					// Create vertex for single X tiles
					for (int k = 0; k < tileCountX; k++)
					{
						for (int c = 0; c < tileCountY; c++)
						{
							vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x, (-c * tileSize) + housePosition.y, 0.f), glm::vec2(houseIndex.first.x, houseIndex.first.y), glm::vec3(0, 0, 1)));
							vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x, (-c * tileSize) + housePosition.y - tileSize, 0.f), glm::vec2(houseIndex.first.x, houseIndex.second.y), glm::vec3(0, 0, 1)));
							vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x + tileSize, (-c * tileSize) + housePosition.y, 0.f), glm::vec2(houseIndex.second.x, houseIndex.first.y), glm::vec3(0, 0, 1)));

							vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x + tileSize, (-c * tileSize) + housePosition.y - tileSize, 0.f), glm::vec2(houseIndex.second.x, houseIndex.second.y), glm::vec3(0, 0, 1)));
							vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x + tileSize, (-c * tileSize) + housePosition.y, 0.f), glm::vec2(houseIndex.second.x, houseIndex.first.y), glm::vec3(0, 0, 1)));
							vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x, (-c * tileSize) + housePosition.y - tileSize, 0.f), glm::vec2(houseIndex.first.x, houseIndex.second.y), glm::vec3(0, 0, 1)));
						}
					}
					JOP_DEBUG_INFO("#### House " + std::to_string(houseCount));
					JOP_DEBUG_INFO("Position X: " + std::to_string(j * tileSize) + " X tiles: " + std::to_string(tileCountX));
					JOP_DEBUG_INFO("Position Y: " + std::to_string(i * tileSize) + " Y tiles: " + std::to_string(tileCountY));
					JOP_DEBUG_INFO("################");

					//vertices.push_back(Vertex(glm::vec3(j * tileSize, i * tileSize, 0.f), glm::vec2(houseIndex.first.x, houseIndex.first.y), glm::vec3(0, 0, 1)));
					//vertices.push_back(Vertex(glm::vec3(j * tileSize, i * tileSize - tileSize, 0.f), glm::vec2(houseIndex.first.x, houseIndex.second.y), glm::vec3(0, 0, 1)));
					//vertices.push_back(Vertex(glm::vec3(j * tileSize + tileSize, i * tileSize, 0.f), glm::vec2(houseIndex.second.x, houseIndex.first.y), glm::vec3(0, 0, 1)));

					//vertices.push_back(Vertex(glm::vec3(j * tileSize + tileSize, i * tileSize - tileSize, 0.f), glm::vec2(houseIndex.second.x, houseIndex.second.y), glm::vec3(0, 0, 1)));
					//vertices.push_back(Vertex(glm::vec3(j * tileSize + tileSize, i * tileSize, 0.f), glm::vec2(houseIndex.second.x, houseIndex.first.y), glm::vec3(0, 0, 1)));
					//vertices.push_back(Vertex(glm::vec3(j * tileSize, i * tileSize - tileSize, 0.f), glm::vec2(houseIndex.first.x, houseIndex.second.y), glm::vec3(0, 0, 1)));

					mapObjects->setPosition(housePosition.x + houseSize.x / 2, housePosition.y - houseSize.y / 2, -0.1f);
					mapObjects->createComponent<RigidBody2D>(scene.getWorld<2>(), RigidBody2D::ConstructInfo2D(rm::getNamed<RectangleShape2D>("house_", houseSize ), RigidBody::Type::Static, 1.f));
					
					//JOP_DEBUG_INFO("House " + std::to_string(houseCount) + " created!");
					//JOP_DEBUG_INFO("Position X: " + std::to_string(housePosition.x) + " Y: " + std::to_string(housePosition.y));
					//JOP_DEBUG_INFO("Size X: " + std::to_string(houseSize.x) + " Y: " + std::to_string(houseSize.y));
					houseCount++;
				}
			}
		}
		auto &houseMesh = rm::getNamed<jop::Mesh>("houseBlock", vertices, std::vector<unsigned>());
		mapObjects->setPosition(0, 0, -0.5f);
		mapObjects->createComponent<jop::Drawable>(scene.getRenderer()).setMesh(houseMesh).setMaterial(rm::getEmpty<jop::Material>("houseMaterial").setMap(jop::Material::Map::Diffuse0, m_atlas->getTexture()));
	}

public:
	MapGenerator(jop::Scene &scene) 
	{
		auto roads = getRandomRoads();
		glm::vec2 startPos = glm::vec2((-mapSize*tileSize) / 2, (mapSize*tileSize) / 2);
		std::vector<std::vector<bool>> tempVecVec;
		for (int i = 0; i < mapSize; i++) 
		{
			std::vector<bool> tempVec;
			for (int j = 0; j < mapSize; j++)
			{
				bool tileStyle = false;
				for (int k = 0; k < roads[0].size(); k++)
				{
					if (roads[0][k] == i)
					{
						tileStyle = true;
					}
				}

				for (int k = 0; k < roads[1].size(); k++)
				{
					if (roads[1][k] == j)
					{
						tileStyle = true;
					}
				}

				tempVec.push_back(tileStyle);
			}
			tempVecVec.push_back(tempVec);
			std::string mapString;
			for (int i = 0; i < tempVec.size(); i++)
			{
				mapString.append(std::to_string(tempVec[i]) + " ");
			}
			JOP_DEBUG_INFO(mapString);
						
			//tempVec.push_back(glm::vec2(j * tileSize + startPos.x, -i * tileSize + startPos.y));
			//map.push_back(tempVec);

		}

		createHouses(tempVecVec, scene);
		//createTiles(scene, map);
		//createWallCollisions(scene, map);
	}
};

