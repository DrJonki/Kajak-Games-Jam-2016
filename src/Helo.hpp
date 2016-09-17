#pragma once

#include <Jopnal/Jopnal.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

using namespace jop;

class Helo : public Drawable
{
private:

    WeakReference<const Object> m_player;
    WeakReference<Object> m_light;
    glm::vec2 m_target;

public:

    Helo(Object& obj, Renderer& rend, WeakReference<const Object> player)
        : Drawable(obj, rend),
          m_player(player),
          m_light(obj.createChild("")),
          m_target(0.f, 5.f)
    {
        using RM = ResourceManager;

        obj.move(-10.f, 0.f, 25.f);

        // Model
        {
            auto& mat = RM::getEmpty<Material>("helo_mat");
            mat.setMap(Material::Map::Diffuse0, RM::get<Texture2D>("helo.jpg"));

            setModel(RM::getNamed<RectangleMesh>("helo_mesh", glm::vec2(1.f, 2.f)), mat);
        }

        // Light
        {
            m_light->setPosition(0.f, 1.f, -0.5f);

        }
    }

    void update(const float deltaTime) override
    {
        const auto playerPos = glm::vec2(m_player->getGlobalPosition());
        const auto selfPos = glm::vec2(getObject()->getGlobalPosition());

        m_target = glm::rotate(m_target, glm::two_pi<float>() / 10.f * deltaTime);

        // Position
        const glm::vec2 toTarget((playerPos + m_target) - selfPos);
        getObject()->move(glm::vec3(toTarget, 0.f) * deltaTime);
        // Rotation
        const auto rotTarget = (playerPos - (playerPos + glm::rotate(m_target, glm::half_pi<float>()))) * 0.5f;
        const auto angle = std::atan2(rotTarget.y, rotTarget.x) - std::atan2(-1.f, 0.f)/* + glm::half_pi<float>()*/;
        getObject()->setRotation(0.f, 0.f, angle);

    }
};