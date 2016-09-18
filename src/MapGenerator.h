#pragma once

#include <Jopnal/Jopnal.hpp>

class MapGenerator
{

private:
	jop::WeakReference<jop::Object> m_object;
	std::vector<std::vector<glm::vec2>> map;
	glm::vec2 tile;
	float tileSize = 10.f;
	int mapSize = 40;
	int minRoadRange = 4;
	int maxRoadRange = 4;
	std::vector<std::vector<bool>> tempVecVec;
	std::vector<int> xRoads;
	std::vector<int> yRoads;

	std::vector<std::vector<int>> getRandomRoads()
	{
		using namespace jop;
		std::vector<std::vector<int>> tempVec;

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
		
		auto roadTexture = m_atlas->addTexture("road.png");
		auto houseTexture = m_atlas->addTexture("house.png");

		auto roadIndex = m_atlas->getCoordinates(roadTexture);
		auto houseIndex = m_atlas->getCoordinates(houseTexture);

		std::vector<jop::Vertex> vertices;

		auto mapObjects = scene.createChild("mapObjects");
        mapObjects->addTag("house");

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
						for (int c = 0; c <= tileCountY; c++)
						{
							if (c < tileCountY)
							{
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x, (-c * tileSize) + housePosition.y, 10.f), glm::vec2(houseIndex.first.x, houseIndex.first.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x, (-c * tileSize) + housePosition.y - tileSize, 10.f), glm::vec2(houseIndex.first.x, houseIndex.second.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x + tileSize, (-c * tileSize) + housePosition.y, 10.f), glm::vec2(houseIndex.second.x, houseIndex.first.y), glm::vec3(0, 0, 1)));

								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x + tileSize, (-c * tileSize) + housePosition.y - tileSize, 10.f), glm::vec2(houseIndex.second.x, houseIndex.second.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x + tileSize, (-c * tileSize) + housePosition.y, 10.f), glm::vec2(houseIndex.second.x, houseIndex.first.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x, (-c * tileSize) + housePosition.y - tileSize, 10.f), glm::vec2(houseIndex.first.x, houseIndex.second.y), glm::vec3(0, 0, 1)));
							}
							else
							{
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x, (-tileCountY * tileSize) + housePosition.y, 0.f), glm::vec2(roadIndex.first.x, roadIndex.first.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x, (-tileCountY * tileSize) + housePosition.y - tileSize, 0.f), glm::vec2(roadIndex.first.x, roadIndex.second.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x + tileSize, (-tileCountY * tileSize) + housePosition.y, 0.f), glm::vec2(roadIndex.second.x, roadIndex.first.y), glm::vec3(0, 0, 1)));

								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x + tileSize, (-tileCountY * tileSize) + housePosition.y - tileSize, 0.f), glm::vec2(roadIndex.second.x, roadIndex.second.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x + tileSize, (-tileCountY * tileSize) + housePosition.y, 0.f), glm::vec2(roadIndex.second.x, roadIndex.first.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((k * tileSize) + housePosition.x, (-tileCountY * tileSize) + housePosition.y - tileSize, 0.f), glm::vec2(roadIndex.first.x, roadIndex.second.y), glm::vec3(0, 0, 1)));
							}
							if (k < tileCountX)
							{
								vertices.push_back(Vertex(glm::vec3((tileCountX * tileSize) + housePosition.x, (-(c) * tileSize) + housePosition.y, 0.f), glm::vec2(roadIndex.first.x, roadIndex.first.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((tileCountX * tileSize) + housePosition.x, (-(c) * tileSize) + housePosition.y - tileSize, 0.f), glm::vec2(roadIndex.first.x, roadIndex.second.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((tileCountX * tileSize) + housePosition.x + tileSize, (-(c) * tileSize) + housePosition.y, 0.f), glm::vec2(roadIndex.second.x, roadIndex.first.y), glm::vec3(0, 0, 1)));

								vertices.push_back(Vertex(glm::vec3((tileCountX * tileSize) + housePosition.x + tileSize, (-(c) * tileSize) + housePosition.y - tileSize, 0.f), glm::vec2(roadIndex.second.x, roadIndex.second.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((tileCountX * tileSize) + housePosition.x + tileSize, (-(c) * tileSize) + housePosition.y, 0.f), glm::vec2(roadIndex.second.x, roadIndex.first.y), glm::vec3(0, 0, 1)));
								vertices.push_back(Vertex(glm::vec3((tileCountX * tileSize) + housePosition.x, (-(c) * tileSize) + housePosition.y - tileSize, 0.f), glm::vec2(roadIndex.first.x, roadIndex.second.y), glm::vec3(0, 0, 1)));
							}
						}
					}

					//JOP_DEBUG_INFO("#### House " + std::to_string(houseCount));
					//JOP_DEBUG_INFO("Position X: " + std::to_string(j * tileSize) + " X tiles: " + std::to_string(tileCountX));
					//JOP_DEBUG_INFO("Position Y: " + std::to_string(i * tileSize) + " Y tiles: " + std::to_string(tileCountY));
					//JOP_DEBUG_INFO("################");

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
		mapObjects->createComponent<jop::Drawable>(scene.getRenderer()).setMesh(houseMesh).setMaterial(rm::getEmpty<jop::Material>("houseMaterial").setMap(jop::Material::Map::Diffuse0, m_atlas->getTexture()).setLightingModel(Material::LightingModel::BlinnPhong));
	}

public:
	MapGenerator(jop::Scene &scene) 
	{
		auto roads = getRandomRoads();
		glm::vec2 startPos = glm::vec2((-mapSize*tileSize) / 2, (mapSize*tileSize) / 2);
		for (int i = 0; i < mapSize; i++) 
		{
			std::string mapString;
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

			for (int i = 0; i < tempVec.size(); i++)
			{
				if (i != 0 )
				{
					mapString.append(std::to_string(tempVec[i]) + " ");
				}

			}
			JOP_DEBUG_INFO(mapString);

		}
		bool spawnSet = false;
		for (int i = 0; i < tempVecVec.size(); i++)
		{
			if (tempVecVec[i][i] && !spawnSet)
			{
				//JOP_DEBUG_INFO(i * tileSize);
				scene.findChild("car")->setPosition(glm::vec3(i * tileSize + (tileSize * 2.5), i*tileSize - tileSize / 2 , 0)).getComponent<jop::RigidBody2D>()->synchronizeTransform();
				spawnSet = true;
			}
		}
		createHouses(tempVecVec, scene);
	}

	std::vector<int> getRandomYRoads()
	{
		std::vector<int> tempRoad;

		for (int i = 0; i < yRoads.size(); i++)
		{
			tempRoad.push_back(yRoads[i] * tileSize - tileSize / 2);
		}

		return tempRoad;
	}

	std::vector<int> getRandomXRoads()
	{
		std::vector<int> tempRoad;

		for (int i = 0; i < xRoads.size(); i++)
		{
			tempRoad.push_back(xRoads[i] * tileSize + (tileSize * 2.5));
		}

		return tempRoad;
	}

	float getTileSize()
	{
		return tileSize;
	}

	glm::vec2 getMapSize()
	{
		return glm::vec2(mapSize * tileSize);
	}
};

