#pragma once

#include <Jopnal/Jopnal.hpp>

using namespace jop;

class UI : public Component
{
public:

    UI(Object& obj)
        : Component(obj, 0)
    {

    }

    void update(const float deltaTime) override
    {

    }
};