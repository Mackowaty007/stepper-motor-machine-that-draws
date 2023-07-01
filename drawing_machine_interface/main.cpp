#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#define SCREEN_W 700
#define SCREEN_H 700

std::string Gcode_line;
std::vector<float> Xpos;
std::vector<float> Ypos;

float lerp(int a,int b,float t){
    return a + t * (b - a);
}
int pythagoras(int a,int b){
    return sqrt(pow(a,2)+pow(b,2));
}

int main()
{
    //handle file
    std::ifstream myfile;
    myfile.open("test.gcode");
    if(myfile.is_open()){
        while(getline(myfile,Gcode_line)){
            std::istringstream iss(Gcode_line);
            std::string Gcode_instruction;
            while(iss>>Gcode_instruction){
                //TODO: make the G0 be a rapid mode and make the G1 be a linear interpolated mode
                if (Gcode_instruction == "G0" || Gcode_instruction == "G1"){
                    iss >> Gcode_instruction;
                    Gcode_instruction.length();
                    if(Gcode_instruction[0] == 'X'){
                        std::cout << stof(Gcode_instruction.substr(1,Gcode_instruction.length())) << std::endl;

                        Xpos.push_back(stof(Gcode_instruction.substr(1,Gcode_instruction.length())));

                        iss >> Gcode_instruction;
                        if(Gcode_instruction[0] == 'Y') std::cout << stof(Gcode_instruction.substr(1,Gcode_instruction.length())) << std::endl;;
                        Ypos.push_back(stof(Gcode_instruction.substr(1,Gcode_instruction.length())));
                    }

                    if(Gcode_instruction[0] == 'Z')std::cout << stof(Gcode_instruction.substr(1,Gcode_instruction.length())) << std::endl;;
                }
            }
        }
    }


    // Create the main window
    sf::RenderWindow app(sf::VideoMode(SCREEN_W, SCREEN_H), "SFML window");
    sf::VertexArray line(sf::Lines,2);
    line[0].color = sf::Color(255,20,20);
    line[1].color = sf::Color(255,20,20);
    line[0].position = sf::Vector2f(10,10);
    line[1].position = sf::Vector2f(112,200);

    while (app.isOpen())
    {
        sf::Event event;
        while (app.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                app.close();
        }

        //render
        app.clear();
        //draw the leading lines

        for (int i=0;i<Xpos.size()-2;i++){
            line[0].position = {Xpos[i],Ypos[i]};
            line[1].position = {Xpos[i+1],Ypos[i+1]};
            app.draw(line);
        }

        app.display();
    }
    return EXIT_SUCCESS;
}
