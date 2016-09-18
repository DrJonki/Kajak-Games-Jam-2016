// For info on the uncommented lines, see the spinning_box example

#include <Jopnal/Jopnal.hpp>
#include "MapGenerator.h"
#include "SpawnManager.hpp"
#include "Helo.hpp"

namespace sc
{
    class ContactListener : public jop::ContactListener2D
    {
    public:

        WeakReference<Object> m_car;

        ContactListener()
            : jop::ContactListener2D()
        {

        }

        void beginContact(Collider2D& collider, const ContactInfo2D& ci) override
        {
            auto o = collider.getObject();
           
            if (o->hasTag("house"))
            {
                m_car->getComponent<SoundEffect>(2)->playReset();
            }
            else if (o->hasTag("spawn_ped"))
            {
                m_car->getComponent<SoundEffect>(3)->playReset();
				o->getComponent<Spawn>()->crash();
            }
            else if(o->hasTag("spawn_car"))
            {
                m_car->getComponent<SoundEffect>(4)->playReset();
				o->getComponent<Spawn>()->crash();
            }
        }

    };
}

class MyScene : public jop::Scene
{
private:

    jop::WeakReference<jop::Object> m_object;
	MapGenerator *map;
    sc::ContactListener m_listener;

public:
	float steeringAngle = 0.0f;
	bool isDrifting = false;
	float m_timer;
    MyScene()
        : jop::Scene("MyScene"),
        m_object(),
        m_listener()
    {
		using namespace jop;
		typedef ResourceManager rm;
        createChild("cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic);
		auto cam = findChild("cam")->getComponent<jop::Camera>();
		cam->setSize(cam->getSize()*10.f);
        cam->setClippingPlanes(-26.f, 1.f);

		createChild("car")->move(10.f, 25.f, 1.f).createComponent<Drawable>(getRenderer()).setModel(rm::getNamed<RectangleMesh>("car_mesh", glm::vec2(1.f, 2.f)), rm::getEmpty<Material>("car_mat").setMap(Material::Map::Diffuse0, rm::get<Texture2D>("car.png")).setLightingModel(Material::LightingModel::BlinnPhong)).
            getObject()->createComponent<RigidBody2D>(getWorld<2>(), RigidBody2D::ConstructInfo2D(rm::getNamed<RectangleShape2D>("car", 1.f, 2.f), RigidBody::Type::Dynamic, 1.f)).registerListener(m_listener);

        // Car lights
        {
            auto light1 = findChild("car")->createChild("carlight1");
            auto light2 = findChild("car")->createChild("carlight2");

            light1->move(-0.2f, -5.f, 0.5f).rotate(glm::half_pi<float>(), 0.0f, 0.f).createComponent<LightSource>(getRenderer(), LightSource::Type::Spot).setIntensity(LightSource::Intensity::Diffuse, Color::White * 100.f).setAttenuation(10.f).setCutoff(glm::radians(8.f), glm::radians(10.f));
            light2->move(0.2f, -5.f, 0.5f).rotate(glm::half_pi<float>(), 0.0f, 0.f).createComponent<LightSource>(getRenderer(), LightSource::Type::Spot).setIntensity(LightSource::Intensity::Diffuse, Color::White * 100.f).setAttenuation(10.f).setCutoff(glm::radians(8.f), glm::radians(10.f));
        }

        // Car sounds
        {
            auto car = findChild("car");
            m_listener.m_car = car;
			car->createComponent<jop::Listener>();

            // Engine
			car->createComponent<SoundEffect>().setBuffer(rm::get<SoundBuffer>("audio/car_loop.wav")).setLoop(true).play().setID(0);

            // drift
            car->createComponent<SoundEffect>().setBuffer(rm::get<SoundBuffer>("audio/driftLoop.wav")).setLoop(true).play().setVolume(0.f).setID(1);

            // crash
            car->createComponent<SoundEffect>().setBuffer(rm::get<SoundBuffer>("audio/metalHit.wav")).setID(2);

            // pedestrian
            car->createComponent<SoundEffect>().setBuffer(rm::get<SoundBuffer>("audio/auts2.wav")).setID(3);

            // explosion
            car->createComponent<SoundEffect>().setBuffer(rm::get<SoundBuffer>("audio/carHit.wav")).setID(4);

			// music
			car->createComponent<SoundEffect>().setBuffer(rm::get<SoundBuffer>("audio/song_2.wav")).setLoop(true).play().setID(5);

			car->getComponent<jop::SoundEffect>(0)->setVolume(50.f);
			car->getComponent<jop::SoundEffect>(1)->setVolume(50.f);
			car->getComponent<jop::SoundEffect>(2)->setVolume(50.f);
			car->getComponent<jop::SoundEffect>(3)->setVolume(50.f);
			car->getComponent<jop::SoundEffect>(4)->setVolume(50.f);


        }

		getWorld<2>().setGravity(glm::vec2());
		//getWorld<2>().setDebugMode(true);

        createComponent<LightSource>(getRenderer(), LightSource::Type::Directional).setIntensity(Color::Gray, Color::Black, Color::Black);
		map = new MapGenerator(*this);
		JOP_DEBUG_INFO("Map size :" +  std::to_string(map->getMapSize().x));
        createComponent<SpawnManager>(glm::vec2(20,-10), map->getMapSize());
        createChild("helo")->createComponent<Helo>(getRenderer(), static_ref_cast<const jop::Object>(findChild("car")));
	}


	void zoomCamera(float zoom)
	{
		auto cam = findChild("cam")->getComponent<jop::Camera>();
		auto camSize = cam->getSize();
		//if ((camSize.x < 100 && deltaZoom > 0) || (camSize.x > 5 && deltaZoom < 0))
			cam->setSize(glm::vec2(zoom, zoom/(camSize.x/camSize.y)));
	}

	//glm::vec2 slideAccelerator()
	//{
	//	auto carObj = findChild("car")->getComponent<jop::RigidBody2D>();
	//	glm::vec2 driveDirection = glm::vec2(findChild("car")->getLocalUp());
	//	static jop::DynamicSetting<float> linearFriction("game@linearFriction", 200.f);
	//	glm::vec2 speedDirection = carObj->getLinearVelocity();
	//	float angleBetween = glm::dot(glm::normalize(driveDirection), glm::normalize(speedDirection));

	//	//(driveDirection.x << ", " << driveDirection.y);
	//	//JOP_DEBUG_DIAG(speedDirection.x << ", " << speedDirection.y);
		//JOP_DEBUG_DIAG(angleBetween);

	//	return speedDirection; // -linearFriction*sin(angleBetween)*speedDirection;
	//	
	//}

	float accelBoost(jop::RigidBody2D &car)
	{
		float boost = 0;
		float currentSpeed = glm::length(car.getLinearVelocity());
		if (currentSpeed > 4)
		{
			boost = (currentSpeed - 4.f)*(currentSpeed - 4.f);
		}
		return boost;
	}

    void preUpdate(const float deltaTime) override
    {
		auto carObj = findChild("car")->getComponent<jop::RigidBody2D>();
		jop::SoundEffect* sounds[2] = { findChild("car")->getComponent<jop::SoundEffect>(0), findChild("car")->getComponent<jop::SoundEffect>(1) };

		glm::vec2 driveDirection = glm::vec2(findChild("car")->getLocalUp());

		static jop::DynamicSetting<float> rearAcceleration("game@rearAccel", 600.f);
		static jop::DynamicSetting<float> rotateTorgue("game@rotateTorgue", 21.f);
		static jop::DynamicSetting<float> rotationFriction("game@rotationFriction", 1000.f);
		static jop::DynamicSetting<float> linearFriction("game@linearFriction", 1000.f);
		static jop::DynamicSetting<float> normalFriction("game@normalFriction", 1000.f);
		static jop::DynamicSetting<float> driveControl("game@driveControl", 12.f);
		static jop::DynamicSetting<float> driveControlActivationSpeed("game@driveControlActivationSpeed", 5.f);
		static jop::DynamicSetting<float> maxSpeed("game@maxSpeed", 40.f);
		

		float angleBetweenVeloAndDir = acos(glm::dot(glm::normalize(glm::length(driveDirection)>0 ? driveDirection : glm::vec2(0, 1)), glm::normalize(glm::length(carObj->getLinearVelocity())>0 ? carObj->getLinearVelocity() : glm::vec2(0, 1))));

		float angularVelo = carObj->getAngularVelocity();
		if (angularVelo > 0.04)
		{
			carObj->applyTorque(-deltaTime*rotationFriction);
		}
		else if (angularVelo < - 0.04)
		{
			carObj->applyTorque(deltaTime*rotationFriction);
		}
		else
		{
			carObj->setAngularVelocity(0);
		}

		//JOP_DEBUG_DIAG(glm::length(carObj->getLinearVelocity()));

		//findChild("debugDot")->setPosition(carObj->getObject()->getLocalPosition() - glm::vec3(driveDirection, 0.f));
		if (jop::Keyboard::isKeyDown(jop::Keyboard::Up))
		{
			//rearPower
			if (glm::length(carObj->getLinearVelocity()) < maxSpeed)
			{
				carObj->applyForce((rearAcceleration.value /*+ accelBoost(*carObj)*/)*glm::normalize(driveDirection)*deltaTime,
					glm::vec2(carObj->getObject()->getLocalPosition() - glm::vec3(driveDirection, 0.f)));
				//frontPower
				/*carObj->applyForce((rearAcceleration.value + accelBoost(*carObj))*glm::normalize(driveDirection)*deltaTime,
					glm::vec2(carObj->getObject()->getLocalPosition() + glm::vec3(driveDirection, 0.f)));*/
			}
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::Down))
		{
			if (!jop::Keyboard::isKeyDown(jop::Keyboard::Up))
			{
				carObj->applyForce((-0.4f*rearAcceleration.value + accelBoost(*carObj))*glm::normalize(driveDirection)*deltaTime,
				glm::vec2(carObj->getObject()->getLocalPosition() - glm::vec3(driveDirection, 0.f)));
			}
		}

		if (jop::Keyboard::isKeyDown(jop::Keyboard::LControl))
		{
			//back brake
			carObj->applyForce(-linearFriction.value*glm::normalize(carObj->getLinearVelocity())*deltaTime, glm::vec2(carObj->getObject()->getLocalPosition() - glm::vec3(driveDirection, 0.f)));
		}

		if (jop::Keyboard::isKeyDown(jop::Keyboard::LAlt))
		{
			//front brake
			carObj->applyForce(-linearFriction.value*glm::normalize(carObj->getLinearVelocity())*deltaTime, glm::vec2(carObj->getObject()->getLocalPosition() + glm::vec3(driveDirection, 0.f)));
		}

		if (jop::Keyboard::isKeyDown(jop::Keyboard::Left))
		{
			if (glm::length(carObj->getLinearVelocity()) > 1)
			{
				carObj->applyTorque((jop::Keyboard::isKeyDown(jop::Keyboard::Down) ? -1.f : 1.f)*rotateTorgue);
			}
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::Right))
		{
			if (glm::length(carObj->getLinearVelocity()) > 1)
			{
				carObj->applyTorque((jop::Keyboard::isKeyDown(jop::Keyboard::Down) ? -1.f:1.f)*- rotateTorgue);
			}
			
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::KeypadAdd))
		{
			zoomCamera(-1.f);
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::KeypadSubtract))
		{
			zoomCamera(1.f);
		}

		if (jop::Keyboard::isKeyDown(jop::Keyboard::F))
		{
			JOP_DEBUG_INFO("Car pos: " + std::to_string(findChild("car")->getLocalPosition().x) + " : " + std::to_string(findChild("car")->getLocalPosition().y));
		}

		if ((m_timer += deltaTime) >= 1.5f)
		{

			int roadIndexY = rand() % map->getRandomYRoads().size();

			getComponent<SpawnManager>()->spawn(Spawn::Type::Car, Spawn::Dir::Right, map->getRandomYRoads()[roadIndexY] - map->getTileSize() / 6);
			getComponent<SpawnManager>()->spawn(Spawn::Type::Car, Spawn::Dir::Left, map->getRandomYRoads()[roadIndexY] + map->getTileSize() / 6);
			getComponent<SpawnManager>()->spawn(Spawn::Type::Pedestrian, Spawn::Dir::Right, map->getRandomYRoads()[roadIndexY] - map->getTileSize() / 3);
			getComponent<SpawnManager>()->spawn(Spawn::Type::Pedestrian, Spawn::Dir::Left, map->getRandomYRoads()[roadIndexY] + map->getTileSize() / 3);

			int roadIndexX = rand() % map->getRandomXRoads().size();
			getComponent<SpawnManager>()->spawn(Spawn::Type::Car, Spawn::Dir::Up, map->getRandomXRoads()[roadIndexX] - map->getTileSize() / 6);
			getComponent<SpawnManager>()->spawn(Spawn::Type::Car, Spawn::Dir::Down, map->getRandomXRoads()[roadIndexX] + map->getTileSize() / 6);
			getComponent<SpawnManager>()->spawn(Spawn::Type::Pedestrian, Spawn::Dir::Up, map->getRandomXRoads()[roadIndexX] - map->getTileSize() / 3);
			getComponent<SpawnManager>()->spawn(Spawn::Type::Pedestrian, Spawn::Dir::Down, map->getRandomXRoads()[roadIndexX] + map->getTileSize() / 3);

			m_timer -= 1.5f;
		}

		// x
		if (findChild("car")->getLocalPosition().x <= 20)
		{
			findChild("car")->setPosition(glm::vec3(map->getMapSize().x - 30, findChild("car")->getLocalPosition().y, 0)).getComponent<jop::RigidBody2D>()->synchronizeTransform();
		}
		if (findChild("car")->getLocalPosition().x >= map->getMapSize().x)
		{
			findChild("car")->setPosition(glm::vec3(40, findChild("car")->getLocalPosition().y, 0)).getComponent<jop::RigidBody2D>()->synchronizeTransform();
		}
		
		// y
		if (findChild("car")->getLocalPosition().y <= -10)
		{
			findChild("car")->setPosition(glm::vec3(findChild("car")->getLocalPosition().x, map->getMapSize().y - 40, 0)).getComponent<jop::RigidBody2D>()->synchronizeTransform();
		}
		if (findChild("car")->getLocalPosition().y >= map->getMapSize().y -20)
		{
			findChild("car")->setPosition(glm::vec3(findChild("car")->getLocalPosition().x, 40, 0)).getComponent<jop::RigidBody2D>()->synchronizeTransform();
		}
		
		// sliding makes car go slower

		//slideAccelerator();

		//carObj->applyCentralForce(slideAccelerator()*deltaTime);
		
		///////////////////////////////////////////////////////////////////////////////////////////////// friction /////////////////////////////////
		findChild("cam")->setPosition(carObj->getObject()->getGlobalPosition());

		

		//JOP_DEBUG_DIAG(glm::length(angleBetweenVeloAndDir));

		if (angleBetweenVeloAndDir > 0.55f)
		{
			isDrifting = true;
		}
		else
		{
			isDrifting = false;
		}

		// Drift sound
		if (isDrifting)
		{
			float driftVolume = (80.f) - angleBetweenVeloAndDir * 50;
			float driftPitch = (0.9f) + angleBetweenVeloAndDir / 8;
			sounds[1]->setVolume(driftVolume);
			sounds[1]->setPitch(driftPitch);
		}
		else
		{
			sounds[1]->setVolume(0.f);
		}

		// Car sound
		float speedFloat = glm::length(carObj->getLinearVelocity());
		sounds[0]->setPitch(speedFloat/10 + 0.5f);


		// Camera zoom
		float minZoom = 15.f;
		float maxZoom = 45.f;

		zoomCamera(minZoom + maxZoom*speedFloat/maxSpeed.value);

		/*if (speedFloat > maxSpeed.value/2)
		{
			zoomCamera(1.f);
		}
		else
		{
			zoomCamera(-1.f);
		}*/
			

		if (angleBetweenVeloAndDir>0 && glm::length(carObj->getLinearVelocity()) > 0.1f)
		{
			carObj->applyForce(-sin(angleBetweenVeloAndDir)*sin(angleBetweenVeloAndDir)*normalFriction.value*glm::normalize(carObj->getLinearVelocity())*deltaTime,
				glm::vec2(carObj->getObject()->getLocalPosition() - glm::vec3(driveDirection, 0.f)));

			/*carObj->applyForce(-sin(angleBetweenVeloAndDir)*sin(angleBetweenVeloAndDir)*normalFriction.value*glm::normalize(carObj->getLinearVelocity())*deltaTime,
				glm::vec2(carObj->getObject()->getLocalPosition() + glm::vec3(driveDirection, 0.f)));*/

			//JOP_DEBUG_DIAG(glm::dot(glm::normalize(glm::length(driveDirection)>0 ? driveDirection : glm::vec2(0, 1)), glm::normalize(glm::length(carObj->getLinearVelocity())>0 ? glm::vec2(-carObj->getLinearVelocity().y, carObj->getLinearVelocity().x) : glm::vec2(0, 1))));
			
		//driveControl	
			float lol = glm::dot(glm::normalize(glm::length(driveDirection) > 0 ? driveDirection : glm::vec2(0, 1)), glm::normalize(glm::length(carObj->getLinearVelocity()) > 0 ? glm::vec2(-carObj->getLinearVelocity().y, carObj->getLinearVelocity().x) : glm::vec2(0, 1)));
			
			if (glm::length(carObj->getLinearVelocity()) > driveControlActivationSpeed.value && jop::Keyboard::isKeyDown(jop::Keyboard::Up))
			{
				if (lol > 0)
				{
					if (!jop::Keyboard::isKeyDown(jop::Keyboard::Right) && !jop::Keyboard::isKeyDown(jop::Keyboard::Left) && abs(lol) > 0.15)
					{
						carObj->applyTorque(-driveControl.value);
					}
				}
				else
				{
					if (!jop::Keyboard::isKeyDown(jop::Keyboard::Right) && !jop::Keyboard::isKeyDown(jop::Keyboard::Left) && abs(lol) > 0.15)
					{
						carObj->applyTorque(driveControl.value);
					}
				}
			}
		}
		else if (!jop::Keyboard::isKeyDown(jop::Keyboard::Up) && !jop::Keyboard::isKeyDown(jop::Keyboard::Down))
		{
			carObj->setLinearVelocity(glm::vec2());
			carObj->setAngularVelocity(0);
		}
    }
};

class EventHandler :public jop::WindowEventHandler{
	EventHandler(jop::Window &w) :jop::WindowEventHandler(w)
	{}
		void keyPressed(const int key, const int, const int) override
		{
			auto &scene = jop::Engine::getCurrentScene();
			//static_cast <MyScene&>(scene).HandleKey(key);
		}
	
};

int main(int argc, char* argv[])
{
    jop::SettingManager::setDefaultDirectory("defconf");

    JOP_ENGINE_INIT("Skit Cirkel", argc, argv);

    jop::Engine::createScene<MyScene>();

    return JOP_MAIN_LOOP;
}