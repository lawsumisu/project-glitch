//main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "InputBuffer.h"
#include <SFML/Graphics.hpp>
#include "PillarCollider.h"

int main()
{

    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::Green);

    InputBindingSet bindings = {};
    int count = 0;
    bindings.insert(InputBindingPtr(new KeyboardBinding(sf::Keyboard::Right, InputCode::RIGHT)));
    bindings.insert(InputBindingPtr(new KeyboardBinding(sf::Keyboard::Left, InputCode::LEFT)));
    InputBuffer buffer = InputBuffer(5, bindings);

    PillarCollider collider = PillarCollider({ -20, 0,-60 }, { 0, 0, 40 }, 20, Vector2f(0, 60));
    VertexArray va = collider.toOutline();
    while (window.isOpen())
    {
        count++;
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //Testing keyboard bindings.
        
        
        buffer.update();
        if (buffer.current().count() > 0) std::cout << buffer.current() << std::endl;

        window.clear();
        window.draw(va);
        window.display();
    }

    return 0;
}

