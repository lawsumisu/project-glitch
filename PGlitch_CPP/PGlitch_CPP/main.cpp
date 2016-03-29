//main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "InputBuffer.h"
#include <SFML/Graphics.hpp>
#include "PillarCollider.h"
#include "SensorCollider.h"
#include "Character.h"
#include <SFML/System/Clock.hpp>

using namespace Physics;

int main()
{

    sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::Green);

    InputBuffer buffer = InputBuffer(5);
    buffer.addBinding(new KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT));
    buffer.addBinding(new KeyboardBinding(sf::Keyboard::Right, InputCode::RIGHT));

    PillarCollider collider = PillarCollider({ -20, 0,-60 }, { 0, 0, 40 }, 20, Vector2f(0, 60));
    SensorCollider sensor = SensorCollider(Vector2f(100,100), Vector2f(16, 16), Vector2f(32,32));
    sensor.setCenter(Vector2f(400,200));
    VertexArray va = collider.toOutline();

    //Testing character
    Clock clock;
    int frames = 0;
    Character player = Character();
    player.setSize(Vector2f(32, 72));
    player.setPosition(Vector2f(200,300));

    PillarCollider ground = PillarCollider::uniformDepth({-2 }, 500, 0, Vector2f(0, 400));
    PillarCollider wall1 = PillarCollider::uniformDepth({ -200 }, 2, 0, Vector2f(0, 400));
    PillarCollider wall2 = PillarCollider::uniformDepth({ -200 }, 2, 0, Vector2f(498, 400));
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //Testing keyboard bindings.       
        buffer.update();
        if (buffer.current().count() > 0) std::cout << buffer.current() << std::endl;

        ++frames;
        float dt = clock.getElapsedTime().asSeconds() / frames;
        //std::cout << "FPS:" << 1.f / dt << std::endl;
        player.update({ ground, wall1, wall2 },dt);
        window.clear();
        window.draw(ground.toOutline());
        window.draw(wall1.toOutline());
        window.draw(wall2.toOutline());
        window.draw(player);
        window.display();
    }

    return 0;
}

