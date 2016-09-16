// For info on the uncommented lines, see the spinning_box example

#include <Jopnal/Jopnal.hpp>
#include "MapGenerator.h"

class MyScene : public jop::Scene
{
private:

    jop::WeakReference<jop::Object> m_object;

public:

    MyScene()
        : jop::Scene("MyScene"),
        m_object()
    {
		using namespace jop;
		typedef ResourceManager rm;
        createChild("cam")->createComponent<jop::Camera>(getRenderer(), jop::Camera::Projection::Orthographic);
		auto cam = findChild("cam")->getComponent<jop::Camera>();
		cam->setSize(cam->getSize()*10.f);

		createChild("car")->createComponent<Sprite>(getRenderer()).setSize(glm::vec2(1.f)).setTexture(rm::get<Texture2D>("car.jpg"),false);	

		MapGenerator map = MapGenerator(*this);
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
		if ((camSize.x < 50 && deltaZoom > 0) || (camSize.x > 5 && deltaZoom < 0))
			cam->setSize(camSize + glm::vec2(deltaZoom, deltaZoom/(camSize.x/camSize.y)));
	}

    void preUpdate(const float deltaTime) override
    {
		auto carObj = findChild("car");

		if (jop::Keyboard::isKeyDown(jop::Keyboard::Up))
		{
			carObj->move(0, deltaTime, 0);
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::Down))
		{
			carObj->move(0, -deltaTime, 0);
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::Left))
		{
			carObj->move(-deltaTime, 0, 0);
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::Right))
		{
			carObj->move(deltaTime, 0, 0);
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::KeypadAdd))
		{
			zoomCamera(-1.f);
		}
		if (jop::Keyboard::isKeyDown(jop::Keyboard::KeypadSubtract))
		{
			zoomCamera(1.f);
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