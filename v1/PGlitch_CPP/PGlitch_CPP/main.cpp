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
    player.setPosition(Vector2f(50,0));

    SimplePlatform ground = SimplePlatform(PillarCollider::uniformDepth({-.5f,-2.5f,-4.5f,-4.5f,-4.5f,-4.5f,-2.5f, -.5f }, 
        10, 0, Vector2f(17.5f, 100)));
    //PillarCollider ground2 = PillarCollider::uniformDepth({ -2,-4,-6,-8,-10,-12,-14 }, 10, 0, Vector2f(0, 400));
    SimplePlatform ground3 = SimplePlatform(PillarCollider::uniformHeight({ 2.5f }, 500, 0, Vector2f(0, 150)));
    SimplePlatform wall1 = SimplePlatform(PillarCollider::uniformDepth({ -50 }, .5f, 0, Vector2f(0, 100)));
    SimplePlatform wall2 = SimplePlatform(PillarCollider::uniformDepth({ -50 }, .5f, 0, Vector2f(124.5f, 100)));
    SimplePlatform wall3 = SimplePlatform(PillarCollider::uniformDepth({ -100 }, 100, 0, Vector2f(250, 150)));

    vector<float> sinValues = {};
    vector<Vector2f> sinValues2 = {};
    float amplitude = 40;
    size_t length = 400;
    for (size_t i = 0; i < length; ++i) {
        sinValues.push_back(-sinf(i * 2 * pi / length)*amplitude);
        sinValues2.push_back(Vector2f((float) i, -sinf(i * 2 * pi / length)*amplitude));
    }

    sinValues2.push_back(sinValues2[sinValues2.size() - 1] + Vector2f(0, amplitude));
    sinValues2.push_back(Vector2f(0, amplitude));
    PillarCollider curve1 = PillarCollider::uniformDepth(sinValues, 1, amplitude, Vector2f(-75, 200));
    //Testing platform
    PillarCollider ground4 = PillarCollider::uniformHeight({ 13.f}, 20, 0, Vector2f(-20, 50));
    SimplePlatform p1 = SimplePlatform(ground4, Vector2f(15, 0), 1.f, PlatformType::THICK);

    SimplePlatform p2 = SimplePlatform(ground4, Vector2f(0, -101.75f), .5f, PlatformType::THICK);
    SimplePlatform p3 = SimplePlatform(ground4, Vector2f(0, -55.5f), .9f, PlatformType::THICK);
    SimplePlatform p4 = SimplePlatform(ground4, Vector2f(0, -271.25f), .2f, PlatformType::THICK);

    p1.position(Vector2f(0, 148));
    p2.position(Vector2f(150, 137.f));
    p3.position(Vector2f(180, 100));
    p4.position(Vector2f(210, 137.5f));

    PlatformPtr g1(new AffinePlatform(FreeformCollider(FloatRect(0, 50, 500, 2.5))));
    PlatformPtr g2(new AffinePlatform(FreeformCollider(FloatRect(0, 0, 100, 100))));
    FreeformCollider windmillCollider = FreeformCollider(FloatRect(0, 0, 50, 10));
    Vector2f windmillPosition(80, 0);
    PlatformPtr windmill(new AffinePlatform(windmillCollider, PlatformType::SOLID, -75.f));
    AffinePlatform* blade1 = new AffinePlatform(windmillCollider, PlatformType::SOLID, -75.f);
    AffinePlatform* blade2 = new AffinePlatform(windmillCollider, PlatformType::SOLID, -75.f);
    AffinePlatform* blade3 = new AffinePlatform(windmillCollider, PlatformType::SOLID, -75.f);
    blade1->angle(180); blade1->position(windmillPosition);
    blade2->angle(90); blade2->position(windmillPosition);
    blade3->angle(270); blade3->position(windmillPosition);


    PlatformPtr curve(new AffinePlatform(sinValues2));
    PlatformPtr g3(new SimplePlatform(ground4, Vector2f(0, 15), 3.f, PlatformType::THICK));
    PlatformPtr g4(new SimplePlatform(ground4, Vector2f(0, -75.f), .8f, PlatformType::THICK));
    PlatformPtr g5(new SimplePlatform(ground4, Vector2f(0, -50.5f), .9f, PlatformType::THICK));
    PlatformPtr g6(new SimplePlatform(ground4, Vector2f(0, -150.25f), .3f, PlatformType::THICK));
    PlatformPtr g7(new SimplePlatform(ground4, Vector2f(250, 0), 1.5f, PlatformType::THICK));
    curve->position(Vector2f(-75, 150));
    windmill->position(windmillPosition);
    g2->position(Vector2f(200, -50));
    g4->position(Vector2f(180, -25));
    g5->position(Vector2f(330, -10.f));
    g6->position(Vector2f(370, -57.5f));
    g7->position(Vector2f(350, 45));

    //std::vector<Platform> platforms = { ground, ground3, wall1, wall2, wall3, curve1, p1, p2, p3, p4 };
    std::vector<PillarCollider> pillars = {};
    std::vector<PlatformPtr> pforms = {g1, curve, windmill, g2, g3, g4, g5, g6, g7 ,PlatformPtr(blade1), PlatformPtr(blade2), PlatformPtr(blade3)};
    //std::vector<PlatformPtr> pforms = { g1, g3 };

       
    //FreeformCollider poly = FreeformCollider({ { 2,0 },{ 2, -3 },{ 4, -6 },{ 6, -3 },{ 6, 0 } });
    GameState::time().timeScale(1);
    float theta = 0;
    float x = 0;
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
        //std::cout << "FPS:" << 1.f / GameState::time().udt() << std::endl;
        theta -= GameState::time().dt()*20.f;
        x += GameState::time().dt() * 5.f;
        //pforms[2].angle(theta);
        //pforms[2].position(Vector2f(x,25));
        for (auto& ptr: pforms) {
            ptr->update();
        }
        player.update(pforms);
        
        
        window.clear();
        //Update view
        View view = window.getView();
        view.setCenter(player.position()*ppu);
        window.setView(view);
        

        for (PlatformPtr& ptr : pforms) {
            window.draw(*ptr);
        }
        window.draw(player);

        
        //poly.draw(FreeformColliderInfo(player.position(), theta, Vector2f()), window, sf::RenderStates::Default, Color::White);
        //Line l = Line(player.position(), 0, 100.f);
        //l.draw(sf::Color::Green, window);
        window.display();
    }

    return 0;
}

