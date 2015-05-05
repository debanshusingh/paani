//
//  scene.cpp
//  paani
//
//  Created by Sanchit Garg on 3/12/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#include "scene.h"
#include <iostream>
#include "Mesh.h"

using namespace std;

//Cube class functions
Cube::Cube(glm::vec3 c, glm::vec3 d)
{
    center = c;
    dimensions = d;
}

glm::vec3 Cube::getCenter()
{
    return center;
}

glm::vec3 Cube::getDimensions()
{
    return dimensions;
}

glm::vec3 Cube::getHalfDimensions()
{
    return dimensions/2.0f;
}

float Cube::getCellSize()
{
    return cellSize;
}

void Cube::setCenter(glm::vec3 c)
{
    center = c;
}

void Cube::setDimension(glm::vec3 d)
{
    dimensions = d;
}

void Cube::setCellSize(float s)
{
    cellSize = s;
}

Scene::Scene()
{
    //need to add particles
    // create box

    gravity = glm::vec3(0.0,-10.0,0.0);
    
    //number of particles should be a cube (1,8,27...)
    numberOfParticles = 1;
    numberOfParticles *= (numberOfParticles*numberOfParticles);
    
    particleSystem = new ParticleSystem();
    
    cube = new Cube();
    cube->setCenter(glm::vec3(0,0,0));
    cube->setDimension(glm::vec3(22)*particleSystem->getSmoothingRadius());
    cube->setCellSize(particleSystem->getSmoothingRadius());       //depends on cube dimensions and particle radius

}

Scene::~Scene(){
    delete cube;
    delete particleSystem;
}

void Scene::init(){
    glm::vec3 position(0), velocity(0);

    float smoothingRad = particleSystem->getSmoothingRadius() * 1.f;
    int damDim = static_cast <int> (cbrt(numberOfParticles)),

        i,j,k=0;
    
    //set up dam break

    //  start with the highest y, and keep filling squares
    
    for(i=0; i<damDim; i++)
    {
        for(j=0; j<damDim; j++)
        {
            for(k=0; k<damDim; k++)
            {
                position = (glm::vec3(i,j,k)*smoothingRad - glm::vec3(float(damDim) * smoothingRad/2.0f));
//                position.z = 1.0f;
                particleSystem->addParticle(Particle(position, velocity));
            }
        }
    }
    
    particleSystem->setUpperBounds(cube->getCenter() + cube->getHalfDimensions());
    particleSystem->setLowerBounds(cube->getCenter() - cube->getHalfDimensions());
    particleSystem->setCellSize(cube->getCellSize());
    particleSystem->setForces(gravity);
}

void Scene::pourFluid(int damDim, float separationFactor)
{
    int particleCount = damDim * damDim,
        i=0,j=0,k=0;
    
    float smoothingRad = particleSystem->getSmoothingRadius() * separationFactor;
    glm::vec3 position(0), translate(0), velocity(0);
    
    translate = glm::vec3(0,cube->getHalfDimensions().y*0.9,0);
    velocity = glm::vec3(0,-10,0);
    
    for(i=0; i<damDim; i++)
    {
//        for(j=0; j<damDim; j++)
        {
            for(k=0; k<damDim; k++)
            {
                position = (glm::vec3(i,j,k)*smoothingRad - glm::vec3(float(damDim) * smoothingRad/2.0f)) + translate;
                particleSystem->addParticle(Particle(position, velocity));
            }
        }
    }
    
    numberOfParticles += particleCount;
}

void Scene::createContainer(const char* a)
{
    if(a == NULL || strlen(a) == 0)
        return;

    mesh.LoadMesh(a);
    
    particleSystem->loadContainer(mesh);
}

void Scene::update(){

    particleSystem->update();
}

void Scene::setSmoothingRadius(float r)
{
    particleSystem->setSmoothingRadius(r);
}

void Scene::setTimeStep(float t)
{
    particleSystem->setTimeStep(t);
}

void Scene::setSolverIterations(int i)
{
    particleSystem->setSolverIterations(i);
}