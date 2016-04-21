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
#include "PolygonalCollider.h"

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
    buffer.addBinding(new KeyboardBinding(sf::Keyboard::Escape, InputCode::B7));

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

    Platform ground = Platform(PillarCollider::uniformDepth({-.5f,-2.5f,-4.5f,-4.5f,-4.5f,-4.5f,-2.5f, -.5f }, 
        10, 0, Vector2f(17.5f, 100)));
    //PillarCollider ground2 = PillarCollider::uniformDepth({ -2,-4,-6,-8,-10,-12,-14 }, 10, 0, Vector2f(0, 400));
    Platform ground3 = Platform(PillarCollider::uniformHeight({ 2.5f }, 500, 0, Vector2f(0, 150)));
    Platform wall1 = Platform(PillarCollider::uniformDepth({ -50 }, .5f, 0, Vector2f(0, 100)));
    Platform wall2 = Platform(PillarCollider::uniformDepth({ -50 }, .5f, 0, Vector2f(124.5f, 100)));
    Platform wall3 = Platform(PillarCollider::uniformDepth({ -100 }, 100, 0, Vector2f(250, 150)));

    vector<float> sinValues = {};
    float amplitude = 40;
    size_t length = 400;
    for (size_t i = 0; i < length; ++i) {
        sinValues.push_back(-sinf(i * 2 * pi / length)*amplitude);
    }

    PillarCollider curve1 = PillarCollider::uniformDepth(sinValues, 1, amplitude, Vector2f(-75, 200));
    //Testing platform
    PillarCollider ground4 = PillarCollider::uniformHeight({ 13.f}, 20, 0, Vector2f(0, 150));
    Platform p1 = Platform(ground4, Vector2f(15, 0), 1.f, PlatformType::THICK);

    Platform p2 = Platform(ground4, Vector2f(0, -101.75f), .5f, PlatformType::THICK);
    Platform p3 = Platform(ground4, Vector2f(0, -55.5f), .9f, PlatformType::THICK);
    Platform p4 = Platform(ground4, Vector2f(0, -271.25f), .2f, PlatformType::THICK);

    p1.origin(Vector2f(0, 148));
    p2.origin(Vector2f(150, 137.f));
    p3.origin(Vector2f(180, 100));
    p4.origin(Vector2f(210, 137.5f));

    std::vector<Platform> platforms = { ground, ground3, wall1, wall2, wall3, curve1, p1, p2, p3, p4 };
    std::vector<PillarCollider> pillars = {};

    //PolygonalCollider poly = PolygonalCollider(FloatRect(0, 0, 50, 20));
    PolygonalCollider poly = PolygonalCollider({ { 2,0 },{ 2, -3 },{ 4, -6 },{ 6, -3 },{ 6, 0 } });
    float theta = 0;
    while (window.isOpen())
    {
        buffer.update();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || buffer.current().isDown(InputCode::B7))
                window.close();
        }
        
        //Testing keyboard bindings.       
        
        //if (buffer.current().count() > 0) std::cout << buffer.current() << std::endl;

        //std::cout << Joystick::getAxisPosition(0, Joystick::Axis::X) << std::endl;
        GameState::update();
        std::cout << "FPS:" << 1.f / GameState::time().udt() << std::endl;
        for (Platform& platform : platforms) {
            platform.update();
        }
        player.update(pillars, platforms);
        
        
        window.clear();
        //Update view
        View view = window.getView();
        view.setCenter(player.position()*ppu);
        window.setView(view);
        window.draw(player);
        
        for (Platform& platform : platforms) {
            window.draw(platform);
        }

        theta += GameState::time().dt();
        poly.draw(PolygonalColliderInfo(player.position(), theta, Vector2f()), window, sf::RenderStates::Default, Color::White);
        //Line l = Line(player.position(), 0, 100.f);
        //l.draw(sf::Color::Green, window);
        window.display();
    }

    return 0;
}

