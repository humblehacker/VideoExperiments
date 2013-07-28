//
//  main.cc
//  VideoExperiments
//
//  Created by David Whetstone on 7/27/13.
//  Copyright (c) 2013 David Whetstone. All rights reserved.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include "decode.hh"
#include "rotate.hh"
#include "testpattern.hh"

using std::cout;
using std::endl;

void render();
void renderYUV420spNV21(sf::RenderWindow &window, ByteVector const & input, unsigned int width, unsigned int height);
void speedTest();

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        std::string command(argv[1]);
        if (command == "speedtest")
            speedTest();
    }
    else
    {
        render();
    }
    return 0;
}

void render()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "Esc to exit");

    window.clear();
    renderYUV420spNV21(window, testInput, 32, 30);
    window.display();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }
    }
}

void speedTest()
{
    cout << "Running speed tests" << endl;

    int width = 32, height = 30, iterations = 100000;

    RGBAVector rgbaOutput(testInput.size());
    ByteVector byteOutput(testInput.size());

    sf::Clock clock; // starts the clock

    for (int i = 0; i < iterations; ++i)
        decodeYUV420spNV21ToRGBA(testInput, width, height, rgbaOutput);
    cout << "unoptimized decoding: " << iterations << " iterations in " << clock.getElapsedTime().asSeconds() << "s" << endl;

    clock.restart();
    for (int i = 0; i < iterations; ++i)
        decodeYUV420spNV21ToRGBA_optimized(testInput, width, height, rgbaOutput);
    cout << "optimized decoding: " << iterations << " iterations in " << clock.getElapsedTime().asSeconds() << "s" << endl;

    clock.restart();
    for (int i = 0; i < iterations; ++i)
        rotateYUV420spNV21(testInput, width, height, byteOutput);
    cout << "unoptimized rotation: " << iterations << " iterations in " << clock.getElapsedTime().asSeconds() << "s" << endl;
}

void renderYUV420spNV21(sf::RenderWindow &window, ByteVector const & input, unsigned int width, unsigned int height)
{

//    RGBAVectorPtr rgba = decodeYUV420spNV21ToRGBA(input, width, height);
//    RGBAVectorPtr rgba = decodeYUV420spNV21ToRGBA_optimized(input, width, height);

    ByteVectorPtr rotated = rotateYUV420spNV21(input, width, height);
    std::swap(height, width);
    RGBAVectorPtr rgba = decodeYUV420spNV21ToRGBA(*rotated, width, height);
//    RGBAVectorPtr rgba = decodeYUV420spNV21ToRGBA_optimized(*rotated, width, height);

    sf::Image image;
    image.create(width, height, (byte*)&(*rgba)[0]);
    image.saveToFile("/Users/david/image.png");
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);
    window.draw(sprite);
}

