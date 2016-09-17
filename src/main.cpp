// For info on the uncommented lines, see the spinning_box example

#include <Jopnal/Jopnal.hpp>
#include "MapGenerator.h"

class MyScene : public jop::Scene
{
private:

    jop::WeakReference<jop::Object> m_object;

public:
	float steeringAngle = 0.0f;
	bool isDrifting = false;
    MyScene()
        : jop::Scene("MyScene"),
        m_object()
    {
		using namespace jop;
		typedef ResourceManager rm;
        createChild("cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic);
		auto cam = findChild("cam")->getComponent<jop::Camera>();
		cam->setSize(cam->getSize()*10.f);

		createChild("car")->createComponent<Sprite>(getRenderer()).setSize(glm::vec2(1.f,2.f)).
			setTexture(rm::get<Texture2D>("car.jpg"),false).
			getObject()->createComponent<RigidBody2D>(getWorld<2>(),RigidBody2D::ConstructInfo2D(rm::getNamed<RectangleShape2D>("car", 1.f,2.f),RigidBody::Type::Dynamic,1.f));

		MapGenerator map = MapGenerator(*this);
		getWorld<2>().setGravity(glm::vec2());
		getWorld<2>().setDebugMode(true);


		createChild("debugDot")->createComponent<Sprite>(getRenderer()).setSize(glm::vec2(0.1f));
	}

	void HandleKey(const int key){
		switch (key)
		{
			case jop::Keyboard::Up:
			{
				
			}
		}
	}

	void zoomCamera(float deltaZoom)
	{
		auto cam = findChild("cam")->getComponent<jop::Camera>();
		auto camSize = cam->getSize();
		if ((camSize.x < 100 && deltaZoom > 0) || (camSize.x > 5 && deltaZoom < 0))
			cam->setSize(camSize + glm::vec2(deltaZoom, deltaZoom/(camSize.x/camSize.y)));
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
	//	JOP_DEBUG_DIAG(angleBetween);

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

		glm::vec2 driveDirection = glm::vec2(findChild("car")->getLocalUp());

		static jop::DynamicSetting<float> rearAcceleration("game@rearAccel", 200.f);
		static jop::DynamicSetting<float> rotateTorgue("game@rotateTorgue", 10.f);
		static jop::DynamicSetting<float> rotationFriction("game@rotationFriction", 500.f);
		static jop::DynamicSetting<float> linearFriction("game@linearFriction", 200.f);
		static jop::DynamicSetting<float> normalFriction("game@normalFriction", 200.f);
		static jop::DynamicSetting<float> driveControl("game@driveControl", 12.f);
		static jop::DynamicSetting<float> driveControlActivationSpeed("game@driveControlActivationSpeed", 8.f);
		static jop::DynamicSetting<float> maxSpeed("game@maxSpeed", 18.f);
		

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

		JOP_DEBUG_DIAG(glm::length(carObj->getLinearVelocity()));

		findChild("debugDot")->setPosition(carObj->getObject()->getLocalPosition() - glm::vec3(driveDirection, 0.f));
		if (jop::Keyboard::isKeyDown(jop::Keyboard::Up))
		{
			//rearPower
			if (glm::length(carObj->getLinearVelocity()) < maxSpeed)
			{
				carObj->applyForce((rearAcceleration.value + accelBoost(*carObj))*glm::normalize(driveDirection)*deltaTime,
					glm::vec2(carObj->getObject()->getLocalPosition() - glm::vec3(driveDirection, 0.f)));
				//frontPower
				carObj->applyForce((rearAcceleration.value + accelBoost(*carObj))*glm::normalize(driveDirection)*deltaTime,
					glm::vec2(carObj->getObject()->getLocalPosition() + glm::vec3(driveDirection, 0.f)));
			}
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::Down))
		{
			//back brake
			carObj->applyForce(-linearFriction.value*glm::normalize(carObj->getLinearVelocity())*deltaTime, glm::vec2(carObj->getObject()->getLocalPosition() - glm::vec3(driveDirection, 0.f)));
			//front brake
			carObj->applyForce(-linearFriction.value*glm::normalize(carObj->getLinearVelocity())*deltaTime, glm::vec2(carObj->getObject()->getLocalPosition() + glm::vec3(driveDirection, 0.f)));
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
				carObj->applyTorque(rotateTorgue);
			}
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::Right))
		{
			if (glm::length(carObj->getLinearVelocity()) > 1)
			{
				carObj->applyTorque(-rotateTorgue);
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


		// sliding makes car go slower

		//slideAccelerator();

		//carObj->applyCentralForce(slideAccelerator()*deltaTime);
		
		///////////////////////////////////////////////////////////////////////////////////////////////// friction /////////////////////////////////

		

		//JOP_DEBUG_DIAG(glm::length(angleBetweenVeloAndDir));


		if (angleBetweenVeloAndDir > 0.07)
		{
			isDrifting = true;
		}
		else
		{
			isDrifting = false;
		}
			

		if (angleBetweenVeloAndDir>0)
		{
			carObj->applyForce(-sin(angleBetweenVeloAndDir)*sin(angleBetweenVeloAndDir)*normalFriction.value*glm::normalize(carObj->getLinearVelocity())*deltaTime,
				glm::vec2(carObj->getObject()->getLocalPosition() - glm::vec3(driveDirection, 0.f)));

			carObj->applyForce(-sin(angleBetweenVeloAndDir)*sin(angleBetweenVeloAndDir)*normalFriction.value*glm::normalize(carObj->getLinearVelocity())*deltaTime,
				glm::vec2(carObj->getObject()->getLocalPosition() + glm::vec3(driveDirection, 0.f)));

			//JOP_DEBUG_DIAG(glm::dot(glm::normalize(glm::length(driveDirection)>0 ? driveDirection : glm::vec2(0, 1)), glm::normalize(glm::length(carObj->getLinearVelocity())>0 ? glm::vec2(-carObj->getLinearVelocity().y, carObj->getLinearVelocity().x) : glm::vec2(0, 1))));
			
		//driveControl	
			float lol = glm::dot(glm::normalize(glm::length(driveDirection) > 0 ? driveDirection : glm::vec2(0, 1)), glm::normalize(glm::length(carObj->getLinearVelocity()) > 0 ? glm::vec2(-carObj->getLinearVelocity().y, carObj->getLinearVelocity().x) : glm::vec2(0, 1)));
			
			if (glm::length(carObj->getLinearVelocity()) > driveControlActivationSpeed.value)
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
    }
};

class EventHandler :public jop::WindowEventHandler{
	EventHandler(jop::Window &w) :jop::WindowEventHandler(w)
	{}
		void keyPressed(const int key, const int, const int) override
		{
			auto &scene = jop::Engine::getCurrentScene();
			static_cast <MyScene&>(scene).HandleKey(key);
		}
	
};

int main(int argc, char* argv[])
{
    JOP_ENGINE_INIT("Skit Cirkel", argc, argv);

    jop::Engine::createScene<MyScene>();

    return JOP_MAIN_LOOP;
}