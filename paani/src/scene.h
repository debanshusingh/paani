//
//  scene.h
//  paani
//
//  Created by Sanchit Garg on 3/12/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#ifndef __paani__scene__
#define __paani__scene__

//#include "imageloader.h"
#include "ParticleSystem.h"

void init(int argc, char* argv[]);
void display();
void displayParticles();
void handleKeypress(unsigned char key, int x, int y);

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

class Cube;
class Scene;

extern Scene* scene;

class Cube
{
private:
    glm::vec3 dimensions;
    glm::vec3 center;
    float cellSize;     //the size of each hash grid cell
    
public:
    
    //constructors
    Cube(){};
    Cube(glm::vec3, glm::vec3); //Input center and dimensions
    
    // Getters
    glm::vec3 getDimensions();
    glm::vec3 getHalfDimensions();
    glm::vec3 getCenter();
    float getCellSize();
    
    //Setters
    void setDimension(glm::vec3);
    void setCenter(glm::vec3);
    void setCellSize(float);
};

class Scene
{
public:
    
    Scene();
    void init();
    void update();
    void createContainer(const char* a);
    void pourFluid();
    void setSmoothingRadius(float);
    
    glm::vec3 gravity;
    int numberOfParticles;
    Cube* cube;
    ParticleSystem* particleSystem;
    std::string filePath;
    
    Mesh mesh;

};

#endif /* defined(__paani__scene__) */