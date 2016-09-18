#pragma once

using namespace jop;

const float SpawnSpeed[] =
{
    8.f,
    4.f
};

class Spawn : public Drawable
{
public:

    enum class Dir
    {
        Left, Right, Up, Down,

        __Count // keep last, do not use
    };

    enum class Type
    {
        Car,
        Pedestrian,

        __Count //< -||-
    };

private:

    const Type m_type;
    const Dir m_dir;
    const glm::vec2 m_limBL;
    const glm::vec2 m_limTR;

public:

    Spawn(Object& obj, const glm::vec2& limbl, const glm::vec2& limtr, Renderer& rend, World2D& world, const Type type, const Dir direction)
        : Drawable  (obj, rend),
          m_type    (type),
          m_dir     (direction),
          m_limBL   (limbl),
          m_limTR   (limtr)
    {
        std::string name;
        if (type == Type::Pedestrian)
        {
            setColor(Color::Green);
            name = "spawn_ped";
        }
        else
            name = "spawn_car";

        obj.addTag(name);

        obj.createComponent<RigidBody2D>(world, RigidBody2D::ConstructInfo2D(ResourceManager::getNamed<CircleShape2D>(name + "_shape", getLocalBounds().second.x), RigidBody::Type::KinematicSensor));
    }

    void update(const float deltaTime) override
    {
        const float speed = SpawnSpeed[static_cast<int>(m_type)];

        const glm::vec3 dirs[] =
        {
            glm::vec3(-speed, 0.f, 0.f),
            glm::vec3(speed, 0.f, 0.f),
            glm::vec3(0.f, speed, 0.f),
            glm::vec3(0.f, -speed, 0.f),
        };

        auto o = getObject();
        auto& pos = o->move(dirs[static_cast<int>(m_dir)] * deltaTime).getGlobalPosition();

        if (pos.x <= m_limBL.x || pos.x >= m_limTR.x || pos.y <= m_limBL.y || pos.y >= m_limTR.y)
        {
            o->removeSelf();
            return;
        }
    }
};

class SpawnManager : public Component
{
private:

    static const int TypeCount = static_cast<int>(Spawn::Type::__Count);

    std::array<std::pair<WeakReference<Mesh>, WeakReference<Material>>, TypeCount> m_res;
    const glm::vec2 m_bl; // bottom left
    const glm::vec2 m_tr; // top right
    float m_timer;
    Renderer& m_rend;
    World2D& m_world;
    Randomizer m_rand;

public:

    SpawnManager(Object& obj, const glm::vec2& bottomLeft, const glm::vec2& topRight)
        : Component (obj, 0),
          m_res     (),
          m_bl      (bottomLeft),
          m_tr      (topRight),
          m_timer   (0.f),
          m_rend    (obj.getScene().getRenderer()),
          m_world   (obj.getScene().getWorld<2>()),
          m_rand    ()
    {
        using RM = ResourceManager;

        // Car resources
        {
            auto& resPair = m_res[static_cast<int>(Spawn::Type::Car)];

            resPair.first = static_ref_cast<Mesh>(RM::getNamed<RectangleMesh>("car_spawn_mesh", glm::vec2(1.f, 2.f)).getReference());
            resPair.second = static_ref_cast<Material>(RM::getEmpty<Material>("car_spawn_mat").getReference());
            resPair.second->setMap(Material::Map::Diffuse0, RM::get<Texture2D>("car.jpg")).setLightingModel(Material::LightingModel::BlinnPhong).setReflection(Material::Reflection::Ambient, Color::White * 0.8f);
        }

        // Pedestrian resources
        {
            auto& resPair = m_res[static_cast<int>(Spawn::Type::Pedestrian)];

            resPair.first = static_ref_cast<Mesh>(RM::getNamed<CircleMesh>("ped_spawn_mesh", 0.25f, 30).getReference());
            resPair.second = static_ref_cast<Material>(RM::getEmpty<Material>("ped_spawn_mat").getReference());
            resPair.second->setLightingModel(Material::LightingModel::BlinnPhong).setReflection(Material::Reflection::Ambient, Color::White * 0.8f);
        }
    }

    template<typename T>
    T randEnum(const T maxPlus1) const
    {
        return static_cast<T>(m_rand.range(0u, static_cast<unsigned int>(maxPlus1) - 1));
    }

    void update(const float deltaTime) override
    {
    #if 1 // Auto-spawn

        if ((m_timer += deltaTime) >= 0.5f)
        {
            const auto dir = randEnum<Spawn::Dir>(Spawn::Dir::__Count);
            spawn(randEnum<Spawn::Type>(Spawn::Type::__Count), dir, m_rand.range(m_bl.x, m_tr.y));
            m_timer -= 0.5f;
        }

    #endif
    }

    void spawn(const Spawn::Type type, const Spawn::Dir direction, const float offset)
    {
        const int te = static_cast<int>(type); // type enum
        const int de = static_cast<int>(direction); // direction enum

        auto o = getObject()->createChild("");
        o->createComponent<Spawn>(m_bl, m_tr, m_rend, m_world, type, direction).
        
            setModel(m_res[te].first, m_res[te].second);

        const glm::vec3 startPositions[] =
        {
            /* direction */
            /* Left      */ glm::vec3(m_tr.x, offset, 0.2f),
            /* Right     */ glm::vec3(m_bl.x, offset, 0.2f),
            /* Up        */ glm::vec3(offset, m_bl.y, 0.2f),
            /* Down      */ glm::vec3(offset, m_tr.y, 0.2f),
        };
        const float startRotations[] =
        {
            -glm::half_pi<float>(),
            glm::half_pi<float>(),
            0.f,
            glm::pi<float>()
        };

        o->setPosition(startPositions[de]).setRotation(0.f, 0.f, startRotations[de]);
    }
};