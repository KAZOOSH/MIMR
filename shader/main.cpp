#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    if (!sf::Shader::isAvailable()) {
        std::cout << "shader not suported !!!! \n";
    }

    //sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Shader",sf::Style::Titlebar | sf::Style::Close);
    sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "SFML Shader",sf::Style::Fullscreen);
    
    window.setFramerateLimit(30);
    window.setVerticalSyncEnabled(true);

    sf::Vector2u size = window.getSize();
    sf::Shader shader;
    sf::Texture tex;    
    sf::Sprite spr;

    tex.create(size.x, size.y);
    spr.setTexture(tex);

    //test
    //sf::Image image;
    //image.create(size.x, size.y, sf::Color::Red);
    //tex.update(image);
    //spr.setColor(sf::Color::Magenta);
    //spr.setPosition(size.x/2+10,size.y/2+10);
    //spr.setOrigin(size.x/2,size.y/2);
    //test


    if (!shader.loadFromFile("test2.frag", sf::Shader::Fragment)) {
        std::cout << "shader loading failed\n";
    }

    //shader.setUniform("sampler2D", sf::Shader::CurrentTexture);

    //shader.setUniform("color", clk.getElapsedTime().asSeconds()); //0.5,0.5,0.8,1.0
    //shader.setUniform("color", clk.getElapsedTime().asSeconds());

   
    

    sf::Clock clk;
     sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
            }
        }
        /*
        uniform int colorSet;
        uniform float iTime;
        uniform float depth;
        uniform float isIdle;
        */


        shader.setUniform("iTime", clk.getElapsedTime().asSeconds());
        shader.setUniform("depth", 127);
        //shader.setUniform("depth",(float)127.0);

        shader.setUniform("isIdle", (float)1.0);
        shader.setUniform("colorSet", (int)1);




        window.clear(sf::Color(255, 128, 0));
        window.draw(spr,&shader);
        //window.draw(spr);
        window.display();
    }

    return EXIT_SUCCESS;
}
