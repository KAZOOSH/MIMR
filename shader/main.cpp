#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    sf::Shader shader;
    
    sf::Texture tex;
    tex.create(800, 600);
    sf::Sprite spr(tex);


    if (!sf::Shader::isAvailable()) {
        std::cout << "shader not suported !!!! \n";
    }
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Shader",sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    if (!shader.loadFromFile("test2.frag", sf::Shader::Fragment)) {
        std::cout << "shader loading failed\n";
    }

    //shader.setUniform("texture", sf::Shader::CurrentTexture);

    //shader.setUniform("color", clk.getElapsedTime().asSeconds()); //0.5,0.5,0.8,1.0
    //shader.setUniform("color", clk.getElapsedTime().asSeconds());

    

    sf::Clock clk;
    while (window.isOpen()) {
        //shader.setUniform("iTime", clk.getElapsedTime().asSeconds());
        shader.setUniform("depth", clk.getElapsedTime().asSeconds());
        //shader.setUniform("depth",(float)127.0);

        shader.setUniform("isIdle", (float)1.0);
        shader.setUniform("colorSet", (int)1);




        window.clear(sf::Color(255, 128, 0));
        window.draw(spr,&shader);
        window.display();
    }

    return EXIT_SUCCESS;
}
