//
//  ParticleSystem.h
//  paani
//
//  Created by Sanchit Garg on 2/28/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#ifndef __paani__ParticleSystem__
#define __paani__ParticleSystem__

#include "Particle.h"
#include <utility>
#include "utilities.h"
#include <map>
#include "Mesh.h"

//This class stores the list of particles and handles their interactions

struct Container
{
    int numIndices;
    std::vector<glm::vec3> triangles;
    std::vector<glm::vec3> normals;
    glm::vec3 boundingCenter;
    float boundingRadius;
};

class ParticleSystem
{

private:
    std::vector <Particle> particles;   //List of all particles in the system
    std::vector <glm::vec3> particlesPos;
    const float poly6Const = 315.0 / (64 * PI);
    const float spikyConst = 45.0 / (PI);
    const float restDensity = 1000.0; //1000kg/m3
    float smoothingRadius = 1.5f;
    int solverIterations = 4;

    const float relaxation = 0.1f;

    float timeStep = 0.016f;
    
    float s_6 = smoothingRadius*smoothingRadius*smoothingRadius*
                smoothingRadius*smoothingRadius*smoothingRadius;
    float s_9 = (smoothingRadius*smoothingRadius*smoothingRadius)*s_6;
    
    glm::vec3 forces;
    glm::vec3 lowerBounds;
    glm::vec3 upperBounds;
    
    float cellSize;
    std::map<int, std::vector<int>> hashGrid;
    glm::ivec3 gridDim;
    
    struct Container container;
    
    //This will save the list of triangle indices that are in the particular grid.
    // first int maps from the 3D grid location to 1D data.
    std::map<int, std::vector<int> > containerGrid;
    
    //this saves whether the voxel has any triangle or not
    std::vector<bool> containerBool;
    
public:

    typedef std::pair<int, glm::vec3> Neighbor;
    
    //Getter functions
    std::vector<Particle>& getAllParticles();   //Returns the list of all particles in the system
    std::vector<glm::vec3>& getAllParticlesPos();
    float getRestDensity();
    float getSmoothingRadius();
    
    void update();
    glm::vec3 getForces();
    glm::vec3 getLowerBounds();
    glm::vec3 getUpperBounds();
    float getCellSize();
    
    void loadContainer(Mesh& mesh);
    void createContainerGrid();
    
    //setter
    void setForces(glm::vec3 f);
    void setLowerBounds(glm::vec3);
    void setUpperBounds(glm::vec3);
    void setCellSize(float size);
    void setSmoothingRadius(float);
    void setTimeStep(float);
    void setSolverIterations(int);
    
    //Other functions
    void addParticle(Particle);               //Add a particle to the system
    Particle getParticle(int index);            //Returns a particlular add some index
    
    //Function to return a list of all the neighbors within the specified distance
    // Stored as a pair of index and vector to the neighboring particle
    void findNeighbors(int index);
    
    //function that returns the density of a particle
    float getDensity(int index);
    
    float wPoly6Kernel(glm::vec3 distance, float smoothingRadius);
    glm::vec3 gradientWSpikyKernel(glm::vec3 distance, float smoothingRadius);

    glm::vec3 gradientConstraintAtParticle(int index);
    glm::vec3 gradientConstraintForNeighbor(int index, int neighborIndex);
    
    //apply external forces (gravity)
    void applyForces();
    void viscosity(int index);
    
    glm::vec3 findDeltaPosition(int index);
    void findLambda(int index);
    
    //collision detection and resolution
    void particleCollision(int index);
    void particleParticleCollision(int index);
    void particleBoxCollision(int index);
    void particleContainerCollision(int index);
    
    void initialiseHashPositions();         //function that initialises particles hash positions
   
    bool isValidCell(glm::ivec3);
};

#endif
