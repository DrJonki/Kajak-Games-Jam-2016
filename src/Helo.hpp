#pragma once

#include <glm/gtx/rotate_vector.hpp>

using namespace jop;

class Helo : public Drawable
{
private:

    WeakReference<const Object> m_player;
    WeakReference<Object> m_light;
    WeakReference<Object> m_rotor;
    glm::vec2 m_target;
    glm::vec2 m_lightTarget;

public:

    Helo(Object& obj, Renderer& rend, WeakReference<const Object> player)
        : Drawable(obj, rend),
          m_player(player),
          m_light(obj.createChild("light")),
          m_rotor(obj.createChild("rotor")),
          m_target(0.f, 8.f),
          m_lightTarget(0.f, 0.5f)
    {
        using RM = ResourceManager;

        obj.move(-10.f, 0.f, 20.f);

        // Model
        {
            {
                auto& mat = RM::getEmpty<Material>("helo_mat");
                mat.setMap(Material::Map::Diffuse0, RM::get<Texture2D>("helo.png"));
                mat.setLightingModel(Material::LightingModel::BlinnPhong);
                mat.setReflection(Material::Reflection::Ambient, Color::White * 1.25f);

                setModel(RM::getNamed<RectangleMesh>("helo_mesh", glm::vec2(1.f, 3.5f)), mat);
            }

            // Rotor
            {
                auto& mat = RM::getEmpty<Material>("helo_rotor_mat");
                mat.setMap(Material::Map::Diffuse0, RM::get<Texture2D>("rotor.png"));
                mat.setLightingModel(Material::LightingModel::BlinnPhong);
                mat.setReflection(Material::Reflection::Ambient, Color::White * 1.25f);

                auto& d = m_rotor->move(0.f, 0.6f, -0.5f).createComponent<Drawable>(rend);

                d.setModel(RM::getNamed<RectangleMesh>("helo_rotor_mesh", glm::vec2(2.f)), mat);
            }
        }

        // Light
        {
            m_light->setPosition(0.f, 1.f, -0.5f);
            m_light->setIgnoreTransform(Object::TransformRestriction::Rotation);

            auto& light = m_light->createComponent<LightSource>(rend, LightSource::Type::Spot);
            light.setCutoff(glm::radians(3.5f), glm::radians(4.5f));
            light.setAttenuation(25.f);
            light.setIntensity(LightSource::Intensity::Diffuse, Color::White * 25.f);
        }
    }

    void update(const float deltaTime) override
    {
        // Rotor
        m_rotor->rotate(0.f, 0.f, -glm::two_pi<float>() * 2.f * deltaTime);

        // Movement
        const auto playerPos = glm::vec2(m_player->getGlobalPosition());
        const auto selfPos = glm::vec2(getObject()->getGlobalPosition());

        m_target = glm::rotate(m_target, glm::two_pi<float>() / 10.f * deltaTime);

        // Position
        const glm::vec2 toTarget((playerPos + m_target) - selfPos);
        getObject()->move(glm::vec3(toTarget, 0.f) * deltaTime);
        // Rotation
        const auto rotTarget = (playerPos - (playerPos + glm::rotate(m_target, glm::half_pi<float>()))) * 0.5f;
        const auto angle = std::atan2(rotTarget.y, rotTarget.x) - std::atan2(-1.f, 0.f);
        getObject()->setRotation(0.f, 0.f, angle);

        // Light
        m_lightTarget = glm::rotate(m_lightTarget, glm::two_pi<float>() / 2.f * deltaTime);
        m_light->lookAt(m_player->getGlobalPosition() + glm::vec3(m_lightTarget, 0.f));
    }
};