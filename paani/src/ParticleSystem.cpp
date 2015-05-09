//
//  ParticleSystem.cpp
//  paani
//
//  Created by Sanchit Garg on 2/28/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#include "ParticleSystem.h"
#include <iostream>
#include <tbb/tbb.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

int counter = 0;

using namespace tbb;

//Getter functions
std::vector<Particle>& ParticleSystem::getAllParticles()
{
    return particles;
}

std::vector<glm::vec3>& ParticleSystem::getAllParticlesPos(){
    return particlesPos;
}

float ParticleSystem::getRestDensity()
{
    return restDensity;
}

void ParticleSystem::addParticle(Particle p)
{
    particles.push_back(p);
}

Particle ParticleSystem::getParticle(int index)
{
    return particles.at(index);
}

glm::vec3 ParticleSystem::getForces()
{
    return forces;
}

float ParticleSystem::getSmoothingRadius()
{
    return smoothingRadius;
}

void ParticleSystem::setForces(glm::vec3 f)
{
    forces = f;
}

glm::vec3 ParticleSystem::getLowerBounds()
{
    return lowerBounds;
}

glm::vec3 ParticleSystem::getUpperBounds()
{
    return upperBounds;
}

void ParticleSystem::setLowerBounds(glm::vec3 l)
{
    lowerBounds = l;
}

void ParticleSystem::setUpperBounds(glm::vec3 u)
{
    upperBounds = u;
}

float ParticleSystem::getCellSize()
{
    return cellSize;
}

void ParticleSystem::setCellSize(float size)
{
    cellSize = size;
    gridDim = (upperBounds-lowerBounds) / cellSize;
}

void ParticleSystem::setSmoothingRadius(float r)
{
    smoothingRadius = r;
}

void ParticleSystem::setTimeStep(float t)
{
    timeStep = 0.016f * t;
}

void ParticleSystem::setSolverIterations(int i)
{
    solverIterations = i;
}

void ParticleSystem::findNeighbors(int index)
{
    std::vector<Neighbor> neighborsList;
    Particle & currParticle = particles.at(index);
    
    glm::vec3 particlePredictedPos = currParticle.getPredictedPosition();
    
    if (glm::any(glm::isnan(particlePredictedPos))) {
        std::cout<<"error";
    }
    
    glm::ivec3 particleHashPosition = currParticle.getHashPosition();
    int gridLocation = particleHashPosition.x + gridDim.x * (particleHashPosition.y + gridDim.y * particleHashPosition.z);
    
    int i,k;
    Neighbor p;
    
    currParticle.clearNeighbors();
    
    //check neighbors in same cell
    
    glm::vec3 vectorToNeighbor;
    if (hashGrid.find(gridLocation) != hashGrid.end()){
        
        for(i = 0; i<hashGrid.at(gridLocation).size(); i++)
        {
            k = hashGrid.at(gridLocation).at(i);
            
            if(k!=index)
            {
                vectorToNeighbor = particlePredictedPos - particles[k].getPredictedPosition();
                
                if(glm::length(vectorToNeighbor) < smoothingRadius + EPSILON)
                {
                    p.first = k;
                    currParticle.addNeighborIndex(k);
                    
                    p.second = vectorToNeighbor;
                    neighborsList.push_back(p);
                }
            }
        }
    }
    
}

void ParticleSystem::initialiseHashPositions()
{
    hashGrid.clear();
    
    for (int i=0; i<particles.size(); i++)
    {
        glm::ivec3 hashPosition;
        hashPosition = (particles.at(i).getPredictedPosition() + upperBounds) / cellSize;
        particles.at(i).setHashPosition(hashPosition);

        std::vector<glm::ivec3> neighborCells;

        //x
        neighborCells.push_back(hashPosition);
        neighborCells.push_back(glm::ivec3(0,1,0) + hashPosition);
        neighborCells.push_back(glm::ivec3(0,-1,0) + hashPosition);
        neighborCells.push_back(glm::ivec3(0,0,1) + hashPosition);
        neighborCells.push_back(glm::ivec3(0,1,1) + hashPosition);
        neighborCells.push_back(glm::ivec3(0,-1,1) + hashPosition);
        neighborCells.push_back(glm::ivec3(0,0,-1) + hashPosition);
        neighborCells.push_back(glm::ivec3(0,1,-1) + hashPosition);
        neighborCells.push_back(glm::ivec3(0,-1,-1) + hashPosition);
        
        //x+1
        neighborCells.push_back(glm::ivec3(1,0,0) + hashPosition);
        neighborCells.push_back(glm::ivec3(1,1,0) + hashPosition);
        neighborCells.push_back(glm::ivec3(1,-1,0) + hashPosition);
        neighborCells.push_back(glm::ivec3(1,0,1) + hashPosition);
        neighborCells.push_back(glm::ivec3(1,1,1) + hashPosition);
        neighborCells.push_back(glm::ivec3(1,-1,1) + hashPosition);
        neighborCells.push_back(glm::ivec3(1,0,-1) + hashPosition);
        neighborCells.push_back(glm::ivec3(1,1,-1) + hashPosition);
        neighborCells.push_back(glm::ivec3(1,-1,-1) + hashPosition);
        
        //x-1
        neighborCells.push_back(glm::ivec3(-1,0,0) + hashPosition);
        neighborCells.push_back(glm::ivec3(-1,1,0) + hashPosition);
        neighborCells.push_back(glm::ivec3(-1,-1,0) + hashPosition);
        neighborCells.push_back(glm::ivec3(-1,0,1) + hashPosition);
        neighborCells.push_back(glm::ivec3(-1,1,1) + hashPosition);
        neighborCells.push_back(glm::ivec3(-1,-1,1) + hashPosition);
        neighborCells.push_back(glm::ivec3(-1,0,-1) + hashPosition);
        neighborCells.push_back(glm::ivec3(-1,1,-1) + hashPosition);
        neighborCells.push_back(glm::ivec3(-1,-1,-1) + hashPosition);
        
        for(int j = 0; j < neighborCells.size(); j++)
        {
            if(isValidCell(neighborCells.at(j)))
            {
                hashGrid[neighborCells.at(j).x + gridDim.x * (neighborCells.at(j).y + gridDim.y * neighborCells.at(j).z)].push_back(i);
            }
        }
    }
}

bool ParticleSystem::isValidCell(glm::ivec3 cellForCheck)
{
    if(cellForCheck.x >= 0 && cellForCheck.x < gridDim.x)
    {
        if(cellForCheck.y >= 0 && cellForCheck.y < gridDim.y)
        {
            if(cellForCheck.z >= 0 && cellForCheck.z < gridDim.z)
            {
                return true;
            }
        }
    }
    
    return false;
}

float ParticleSystem::getDensity(int index)
{
    float density = 0;
    int i;
    
    std::vector<int> neighbors = particles.at(index).getNeighborIndices();
    
    //Kernel function implementation
    //  Using poly6 kernel function

    //TODO
    //  Currently mass is 1 for all particles
    //  If mass is changed, change the for loop to multiply by mass
    for(i=0; i<neighbors.size(); i++)
    {
        density +=  wPoly6Kernel((particles.at(index).getPredictedPosition() - particles.at(neighbors.at(i)).getPredictedPosition()), smoothingRadius);
    }
    
    return density;
}

float ParticleSystem::wPoly6Kernel(glm::vec3 distance, float smoothingRadius)
{
    float x = (smoothingRadius*smoothingRadius) - (glm::length(distance)*glm::length(distance));
    float x_3 = x*x*x;

    return poly6Const * x_3/s_9;
}

glm::vec3 ParticleSystem::gradientWSpikyKernel(glm::vec3 distance, float smoothingRadius)
{
    float distanceLength = glm::length(distance);

    float x = (smoothingRadius-distanceLength)*(smoothingRadius-distanceLength);
    
    float gradientW = spikyConst * (1.0f/s_6) * x * 1.0f/(distanceLength+EPSILON);

    return gradientW*distance;
}

glm::vec3 ParticleSystem::gradientConstraintAtParticle(int index)
{
    glm::vec3 gradientReturn(0,0,0);
    float restDensityInverse = 1.0/restDensity;
    
    std::vector<int> neighbors = particles.at(index).getNeighborIndices();
    
    for(int i=0; i<neighbors.size(); i++)
    {
        gradientReturn += restDensityInverse *
                        gradientWSpikyKernel((particles.at(index).getPosition() - particles.at(neighbors.at(i)).getPosition()), smoothingRadius);
    }
    
    return gradientReturn;
}

glm::vec3 ParticleSystem::gradientConstraintForNeighbor(int index, int neighborIndex)
{
    glm::vec3 gradientReturn(0,0,0);
    float restDensityInverse = 1.0/restDensity;

    
    gradientReturn = restDensityInverse * gradientWSpikyKernel(particles.at(index).getPosition() - particles[neighborIndex].getPosition(), smoothingRadius);
    
    return (-1.0f * gradientReturn);
}

void ParticleSystem::update()
{
//    counter ++;
    
    applyForces(); // apply forces and predict position
//    initialiseHashPositions();  //initialise hash positions to be used in neighbour search

//    parallel_for<size_t>(0, particles.size()-1, 1, [=](int x)
//    {
//        findNeighbors(x);
//    });

    
    parallel_for<size_t>(0, particles.size(), 1, [=](int i)
    {
        particleCollision(i);
    });

//    for (int k=0; k<solverIterations; ++k)
//    {
//
//        parallel_for<size_t>(0, particles.size(), 1, [=](int i)
//        {
//            findLambda(i);
//        });
//        
//        parallel_for<size_t>(0, particles.size(), 1, [=](int i)
//        {
//            Particle & currParticle = particles.at(i);
//            glm::vec3 deltaPi = findDeltaPosition(i);
//            currParticle.setDeltaPi(deltaPi);
//            particleCollision(i);
//        });
//        
//        parallel_for<size_t>(0, particles.size(), 1, [=](int i)
//        {
//            Particle & currParticle = particles.at(i);
//            currParticle.setPredictedPosition(currParticle.getPredictedPosition() + currParticle.getDeltaPi());
//            currParticle.setDeltaPi(glm::vec3(0.0));
//        });
//    }
    
    parallel_for<size_t>(0, particles.size(), 1, [=](int i)
    {
        Particle & currParticle = particles.at(i);

        currParticle.setVelocity((currParticle.getPredictedPosition() - currParticle.getPosition()) / timeStep);
        currParticle.setPosition(currParticle.getPredictedPosition());
    });
//
//    //opengl only
//    //comment out for houdini
//    particlesPos.clear();
//    for (std::vector<Particle>::iterator it=particles.begin(); it < particles.end(); it++)
//    {
//        Particle particle = *it;
//        glm::vec3 partPos = particle.getPosition();
//        particlesPos.push_back(partPos);
//    }
}

void ParticleSystem::findLambda(int index){
    std::vector<int> neighbors = particles.at(index).getNeighborIndices();
    
    int numNeighbors = static_cast<int>(neighbors.size());
    
    float sumGradientAtParticle = 0.0f;
    float gradientNeighbor = 0.0f;
    
    for (int i=0; i<numNeighbors; i++) {
        gradientNeighbor = glm::length(gradientConstraintForNeighbor(index, i));
        sumGradientAtParticle += gradientNeighbor*gradientNeighbor;
    }
    
    float gradientParticle = glm::length(gradientConstraintAtParticle(index));
    sumGradientAtParticle += gradientParticle*gradientParticle;
    
    float currDensity = this->getDensity(index);
    float densityContraint = (currDensity/restDensity) - 1.0f;
    
    float lambdaI = -1.0f * (densityContraint/(sumGradientAtParticle+relaxation));
//    if (fabs(lambdaI)) std::cout<<lambdaI<<std::endl;

    particles.at(index).setDensity(currDensity);
    particles.at(index).setLambda(lambdaI);
}

glm::vec3 ParticleSystem::findDeltaPosition(int index)
{
    glm::vec3 deltaPi(0,0,0);
    
    Particle & currParticle = particles.at(index);
    
    std::vector<int> neighbors = currParticle.getNeighborIndices();
    float lambda_i = currParticle.getLambda();
    float sCor = 0, k = 0.1, deltaQ = 0.1 * smoothingRadius;
//    int n = 4;
    float spikyTerm;
    
    for(int i=0; i<neighbors.size(); i++)
    {
        float temp = wPoly6Kernel(glm::vec3(deltaQ, 0, 0), smoothingRadius);
        
        spikyTerm = wPoly6Kernel( (currParticle.getPredictedPosition() - particles.at(neighbors.at(i)).getPredictedPosition()), smoothingRadius) / (temp+EPSILON);
        
       
        sCor = -1.0 * k * spikyTerm * spikyTerm * spikyTerm * spikyTerm;
        
        deltaPi += (particles.at(neighbors[i]).getLambda() + lambda_i + sCor) *
                    gradientWSpikyKernel((currParticle.getPredictedPosition() - particles.at(neighbors.at(i)).getPredictedPosition()), smoothingRadius);
    }
    
    return (deltaPi/restDensity);
}

void ParticleSystem::applyForces()
{
//    for(int i=0; i<particles.size(); i++)
    parallel_for<size_t>(0, particles.size(), 1, [=](int i)
    {
        Particle & currParticle = particles.at(i);

        currParticle.setVelocity(currParticle.getVelocity() + timeStep * forces);
        glm::vec3 currPosition = currParticle.getPosition();
        glm::vec3 predictedPosition = currPosition + timeStep * currParticle.getVelocity();
        currParticle.setPredictedPosition(predictedPosition);
//    }
    });
}

void ParticleSystem::viscosity(int index)
{
    Particle & currParticle = particles.at(index);
    std::vector<int> neighbors = currParticle.getNeighborIndices();
    
    glm::vec3 newVelocity(0.0,0.0,0.0);
    glm::vec3 currVelocity = currParticle.getVelocity();
    glm::vec3 currPosition = currParticle.getPredictedPosition();
    
    for(int i=0; i<neighbors.size(); i++)
    {
        if(particles.at(neighbors[i]).getDensity() > EPSILON)
        {
            newVelocity += (1.0f/particles.at(neighbors[i]).getDensity()) * (particles.at(neighbors[i]).getVelocity() - currVelocity) * wPoly6Kernel( (currPosition - particles.at(neighbors.at(i)).getPredictedPosition()), smoothingRadius);
        
//          newVelocity += (particles.at(neighbors.at(i)).getVelocity() - currVelocity) * wPoly6Kernel( (currPosition - particles.at(neighbors.at(i)).getPredictedPosition()), smoothingRadius);
        }
    }
    
    currParticle.setVelocity(currVelocity + 0.01f * newVelocity);
//    currParticle.setVelocity(currVelocity + 0.1f*newVelocity);
}

void ParticleSystem::particleCollision(int index){
    particleBoxCollision(index);
    particleContainerCollision(index);
//    particleParticleCollision(index);
}

void ParticleSystem::particleParticleCollision(int index)
{
//    //as per http://stackoverflow.com/questions/19189322/proper-sphere-collision-resolution-with-different-sizes-and-mass-using-xna-monog
//    
//    std::vector<int> neighbors = particles.at(index).getNeighborIndices();
//    
//    glm::vec3 currentParticlePosition = particles.at(index).getPredictedPosition(),
//                neighborPosition,
//                particleVelocity,
//                neighborVelocity;
//
//    
//    glm::vec3 relativeVelocity,
//                collisionNormal,
//                vCollision;  //components of relative velocity about collision normal and direction
//    
//    
//    float distance, radius = particles.at(index).getRadius();
//    
//    radius= smoothingRadius;
//    for(int i=0; i<neighbors.size(); i++)
//    {
//        particleVelocity = particles.at(index).getVelocity();
//
//        neighborPosition = particles.at(neighbors.at(i)).getPredictedPosition();
//        neighborVelocity = particles.at(neighbors.at(i)).getVelocity();
//        
//        distance = glm::distance(currentParticlePosition, neighborPosition);
//        
//        if(distance < 2 * radius + EPSILON)
//        {
//            //resolve collision
//            relativeVelocity = particleVelocity - neighborVelocity;
//            
//            collisionNormal = glm::normalize(currentParticlePosition - neighborPosition);
//            
//            vCollision = glm::dot(collisionNormal, relativeVelocity) * collisionNormal;
//            
//            particles.at(index).setVelocity(particleVelocity - vCollision);
//            particles.at(neighbors.at(i)).setVelocity(neighborVelocity + vCollision);
//        }
//    }
}

void ParticleSystem::loadContainer(Mesh& mesh)
{
    container.numIndices = mesh.getNumVertices(0);
    container.triangles = mesh.getTriangles(0);
    container.normals = mesh.getNormals(0);
    
    //TODO --------------------------------------
    //  scale triangles as per obj size
    
    glm::vec3 min(0.0), max(0.0);
    for(int i = 0; i<container.numIndices; ++i)
    {
        for(int j=0; j<3; j++)
        {
            if(container.triangles[i][j] < min[j])
                min[j] = container.triangles[i][j];
            if(container.triangles[i][j] > max[j])
                max[j] = container.triangles[i][j];
        }
    }
    
    container.boundingCenter = (min + max)/2.0f;
    container.boundingRadius = glm::distance(min, container.boundingCenter);
    
    createContainerGrid();
}

void ParticleSystem::createContainerGrid()
{
    int size = gridDim.x * gridDim.y * gridDim.z;
    
    for(int i = 0;i<size; ++i)
    {
        containerBool.push_back(false);
    }
    
    //voxel finding algo
    //  1. Determine the range of x, y, z the triangle spans.
    //  2. Mark all voxels with that range as true
    
    glm::vec3 v1, v2, v3;         //triangle vertices
    glm::vec3 min(0), max(0);     //saves the min and max x,y,z (triangle span)
    
    for(int i=0; i<container.triangles.size(); i+=3)
    {
        v1 = container.triangles.at(i) + upperBounds;
        v2 = container.triangles.at(i+1) + upperBounds;
        v3 = container.triangles.at(i+2) + upperBounds;
        
        min = v1;
        max = v1;
        
        for(int j = 0; j<3; ++j)
        {
            if(min[j] > v2[j])
                min[j] = v2[j];
            if(min[j] > v3[j])
                min[j] = v3[j];

            if(max[j] < v2[j])
                max[j] = v2[j];
            if(max[j] < v3[j])
                max[j] = v3[j];
        }
        
        glm::ivec3 rangeMin, rangeMax;
        
        rangeMin = min/cellSize;
        rangeMin -= glm::ivec3(1,1,1);
        rangeMax = max/cellSize;
        rangeMax += glm::ivec3(2,2,2);
        
        for(int x = rangeMin.x; x<=rangeMax.x; ++x)
        {
            for(int y = rangeMin.y; y<=rangeMax.y; ++y)
            {
                for(int z = rangeMin.z; z<=rangeMax.z; ++z)
                {
                    if(isValidCell(glm::ivec3(x,y,z)))
                    {
                        containerBool.at(x + gridDim.x * (y + gridDim.y * z)) = true;
                        containerGrid[x + gridDim.x * (y + gridDim.y * z)].push_back(i);
                    }
                }
            }
        }
    }
//    std::cout<<"gere";
}

void ParticleSystem::particleContainerCollision(int index)
{
    Particle& currParticle = particles.at(index);
    glm::vec3 particlePredictedPos = currParticle.getPredictedPosition() + currParticle.getDeltaPi();
    glm::vec3 particlePos = currParticle.getPosition();
    
    glm::ivec3 hashPosition = currParticle.getHashPosition();
    int gridPosition = hashPosition.x + gridDim.x * (hashPosition.y + gridDim.y * hashPosition.z);
    
    if(gridPosition < containerBool.size())
    {
        if(containerBool.at(gridPosition))
        {
            parallel_for<size_t>(0, containerGrid.at(gridPosition).size(), 1, [=](int i)
             {
                 Particle& currParticle = particles.at(index);
                 glm::vec3 v0, v1, v2, n;
                 float da, db;
                 int triIndex;
                 
                 // triangle-particle collision
                 triIndex = containerGrid.at(gridPosition).at(i);
                 v0 = container.triangles.at(triIndex);
                 v1 = container.triangles.at(triIndex + 1);
                 v2 = container.triangles.at(triIndex + 2);
                 n = container.normals.at(triIndex/3);
                 
                 da = glm::dot((v0-particlePos), n);
                 db = glm::dot((v0-particlePredictedPos), n);
                 
                 if(da*db < ZERO_ABSORPTION_EPSILON)
                 {
                     currParticle.setVelocity(glm::reflect(currParticle.getVelocity(), n) * 1.0f);
                     
                     //if particle goes out of the mesh, increase the multiplying factor of timeStep*n
                     currParticle.setPredictedPosition(particlePos + 3.f * timeStep * n);
                 }
            });
        }
    }
}

void ParticleSystem::particleBoxCollision(int index)
{
    Particle & currParticle = particles.at(index);
    glm::vec3 particlePosition = currParticle.getPredictedPosition() + currParticle.getDeltaPi();

    float radius = currParticle.getRadius();
    float dampingFactor = 0.6f;
    
    if(particlePosition.x - radius < lowerBounds.x + EPSILON || particlePosition.x + radius > upperBounds.x - EPSILON)
    {
        currParticle.setVelocity(currParticle.getVelocity() * glm::vec3(-dampingFactor,1,1));
        currParticle.setPredictedPosition(currParticle.getPosition() + timeStep * currParticle.getVelocity());
    }

    if(particlePosition.y - radius < lowerBounds.y + EPSILON)
    {
        currParticle.setVelocity(currParticle.getVelocity() * glm::vec3(1,-dampingFactor,1));
        currParticle.setPredictedPosition(currParticle.getPosition() + timeStep * currParticle.getVelocity());
    }

    if(particlePosition.y + radius > upperBounds.y - EPSILON)
    {
        currParticle.setVelocity(currParticle.getVelocity() * glm::vec3(1,-dampingFactor,1));
        glm::vec3 pos = currParticle.getPredictedPosition();
        currParticle.setPredictedPosition(glm::vec3(pos.x, upperBounds.y - radius - EPSILON, pos.z));
    }

    if(particlePosition.z - radius < lowerBounds.z + EPSILON || particlePosition.z + radius > upperBounds.z - EPSILON)
    {
        currParticle.setVelocity(currParticle.getVelocity() * glm::vec3(1,1,-dampingFactor));
        currParticle.setPredictedPosition(currParticle.getPosition() + timeStep * currParticle.getVelocity());
    }
}
