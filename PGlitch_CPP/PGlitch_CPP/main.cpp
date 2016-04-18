//main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "InputBuffer.h"
#include <SFML/Graphics.hpp>
#include "PillarCollider.h"
#include "SensorCollider.h"
#include "Character.h"
#include "GameState.h"
#include "Platform.h"
#include "GameInfo.h"

using namespace Physics;

int main()
{

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "SFML works!");
    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::Green);

    InputBuffer buffer = InputBuffer(5);
    buffer.addBinding(new KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT));
    buffer.addBinding(new KeyboardBinding(sf::Keyboard::Right, InputCode::RIGHT));
    buffer.addBinding(new ButtonBinding(0, 0, InputCode::B1));

    PillarCollider collider = PillarCollider({ -20, 0,-60 }, { 0, 0, 40 }, 20, Vector2f(0, 60));
    SensorCollider sensor = SensorCollider(Vector2f(100,100), Vector2f(16, 16), Vector2f(32,32));
    sensor.setCenter(Vector2f(400,200));
    VertexArray va = collider.toOutline();

    //Testing character
    float ppu = GameInfo::pixelsPerUnit;
    GameState::reset();
    Character player = Character();
    player.setSize(Vector2f(32/ppu, 72/ppu));
    player.setPosition(Vector2f(50,60));

    PillarCollider ground = PillarCollider::uniformDepth({-.5f,-2.5f,-4.5f,-4.5f,-4.5f,-4.5f,-2.5f, -.5f }, 
        10, 0, Vector2f(17.5f, 100));
    //PillarCollider ground2 = PillarCollider::uniformDepth({ -2,-4,-6,-8,-10,-12,-14 }, 10, 0, Vector2f(0, 400));
    PillarCollider ground3 = PillarCollider::uniformDepth({ 2.5f }, 500, 0, Vector2f(0, 150));
    PillarCollider wall1 = PillarCollider::uniformDepth({ -50 }, .5f, 0, Vector2f(0, 100));
    PillarCollider wall2 = PillarCollider::uniformDepth({ -50 }, .5f, 0, Vector2f(124.5f, 100));

    //Testing platform
    PillarCollider ground4 = PillarCollider::uniformDepth({ 12.5f}, 20, 0, Vector2f(0, 150));
    Platform p1 = Platform(ground4, Vector2f(15, 0), 1.f);

    Platform p2 = Platform(ground4, Vector2f(0, -18.75f), 1.f);
    Platform p3 = Platform(ground4, Vector2f(0, -26.5f), 2.5f);
    Platform p4 = Platform(ground4, Vector2f(0, -6.25f), 4.f);

    p1.origin(Vector2f(0, 148));
    p2.origin(Vector2f(150, 137.5f));
    p3.origin(Vector2f(180, 100));
    p4.origin(Vector2f(210, 137.5f));

    std::vector<Platform> platforms = { p1, p2, p3, p4 };
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        //Testing keyboard bindings.       
        //buffer.update();
        //if (buffer.current().count() > 0) std::cout << buffer.current() << std::endl;

        //std::cout << Joystick::getAxisPosition(0, Joystick::Axis::X) << std::endl;
        GameState::update();
        //std::cout << "FPS:" << 1.f / GameState::time().dt() << std::endl;
        for (Platform& platform : platforms) {
            platform.update();
        }
        player.update({ ground, ground3, wall1, wall2 }, platforms);
        
        
        window.clear();
        //Update view
        View view = window.getView();
        view.setCenter(player.position()*ppu);
        window.setView(view);
        window.draw(ground.toOutline());
        //window.draw(ground2.toOutline());
        window.draw(ground3.toOutline());
        window.draw(ground.toPhysicalOutline());
        window.draw(wall1.toOutline());
        window.draw(wall2.toOutline());
        window.draw(player);
        
        for (Platform& platform : platforms) {
            window.draw(platform);
        }

        //Line l = Line(player.position(), 0, 100.f);
        //l.draw(sf::Color::Green, window);
        window.display();
    }

    return 0;
}

