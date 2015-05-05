//
//  SOP_Paani.C
//  paani
//
//  Created by Debanshu on 3/20/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#include "SOP_Paani.h"

#include <GU/GU_Detail.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_AutoLockInputs.h>
#include <PRM/PRM_Include.h>
#include <CH/CH_LocalVariable.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <SYS/SYS_Math.h>
#include <limits.h>
#include <GU/GU_PrimPart.h>
#include <stddef.h>

#include <cstring>

using namespace HDK_Sample;
Scene* scene;
bool initSceneBool;

///
/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case we add ourselves
/// to the specified operator table.
///
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
                                       "hdk_paani",                 // Internal name
                                       "Paani",                     // UI name
                                       SOP_Paani::myConstructor,    // How to build the SOP
                                       SOP_Paani::myTemplateList,   // My parameters
                                       0,                          // Min # of sources
                                       0,                          // Max # of sources
                                       SOP_Paani::myVariables,      // Local variables
                                       OP_FLAG_GENERATOR));        // Flag it as generator
}

static PRM_Name		sphSmoothingRadiusName("sphSmoothingRadius", "Sph Smoothing Radius");
static PRM_Default	sphSmoothingRadiusDefault(1.5);
static PRM_Range    sphSmoothingRadiusRange(PRM_RANGE_RESTRICTED, 1.0, PRM_RANGE_UI, 3.0);

static PRM_Name		timeStepName("timescale", "Time Scale");
static PRM_Default	timeStepDefault(1);
static PRM_Range    timeStepRange(PRM_RANGE_RESTRICTED, 0.1, PRM_RANGE_UI, 10);

static PRM_Name		iterationsName("solverIterations", "Solver Iterations");
static PRM_Default	iterationsDefault(10);
static PRM_Range    iterationsRange(PRM_RANGE_RESTRICTED, 1, PRM_RANGE_UI, 20);

static PRM_Name     fileName("file", "Open file");
static PRM_Default  fileNameDefault(0, "");

static PRM_Name		tapSizeName("tapSize", "Tap Size");
static PRM_Default	tapSizeDefault(2);
static PRM_Range    tapSizeRange(PRM_RANGE_RESTRICTED, 2, PRM_RANGE_UI, 5);

static PRM_Name		tapSeparationName("tapParticleSeparation", "Tap Particle Separation");
static PRM_Default	tapSeparationDefault(0.7);
static PRM_Range    tapSeparationRange(PRM_RANGE_RESTRICTED, 0.1, PRM_RANGE_UI, 1);

//static PRM_Name		tapVelocityXName("tapVelocityX", "Tap Velocity X");
//static PRM_Default	tapVelocityXDefault(0.0);
//static PRM_Range    tapVelocityXRange(PRM_RANGE_RESTRICTED, -10.0, PRM_RANGE_UI, 10.0);

//static PRM_Name		tapVelocityYName("tapVelocityY", "Tap Velocity Y");
//static PRM_Default	tapVelocityYDefault(0.0);
//static PRM_Range    tapVelocityYRange(PRM_RANGE_RESTRICTED, -10.0, PRM_RANGE_UI, 10.0);

//static PRM_Name		tapVelocityXName("tapVelocityX", "Tap Velocity X");
//static PRM_Default	tapVelocityXDefault(0.0);
//static PRM_Range    tapVelocityXRange(PRM_RANGE_RESTRICTED, -10.0, PRM_RANGE_UI, 10.0);

static PRM_Name     pourToggle("tapToggle", "Check to pour water");

PRM_Template
SOP_Paani::myTemplateList[] = {
    PRM_Template(PRM_FLT,	PRM_Template::PRM_EXPORT_MIN, 1, &sphSmoothingRadiusName, &sphSmoothingRadiusDefault, 0, &sphSmoothingRadiusRange),
    PRM_Template(PRM_FLT,	PRM_Template::PRM_EXPORT_MIN, 1, &timeStepName, &timeStepDefault, 0, &timeStepRange),
    PRM_Template(PRM_INT,	PRM_Template::PRM_EXPORT_MIN, 1, &iterationsName, &iterationsDefault, 0, &iterationsRange),
    PRM_Template(PRM_FILE, 1, &fileName, &fileNameDefault),
    
    //Tap parameters
    PRM_Template(PRM_INT,	PRM_Template::PRM_EXPORT_MIN, 1, &tapSizeName, &tapSizeDefault, 0, &tapSizeRange),
    PRM_Template(PRM_FLT,	PRM_Template::PRM_EXPORT_MIN, 1, &tapSeparationName, &tapSeparationDefault, 0, &tapSeparationRange),
    PRM_Template(PRM_TOGGLE, 1, &pourToggle, PRMzeroDefaults),
    PRM_Template()
};


// Here's how we define local variables for the SOP.
enum {
    VAR_PT,		// Point number of the star
    VAR_NPT		// Number of points in the star
};

CH_LocalVariable
SOP_Paani::myVariables[] = {
    { "PT",	VAR_PT, 0 },		// The table provides a mapping
    { "NPT",	VAR_NPT, 0 },		// from text string to integer token
    { 0, 0, 0 },
};

bool
SOP_Paani::evalVariableValue(fpreal &val, int index, int thread)
{
    // myCurrPoint will be negative when we're not cooking so only try to
    // handle the local variables when we have a valid myCurrPoint index.
    if (myCurrPoint >= 0)
    {
        // Note that "gdp" may be null here, so we do the safe thing
        // and cache values we are interested in.
        switch (index)
        {
            case VAR_PT:
                val = (fpreal) myCurrPoint;
                return true;
            case VAR_NPT:
                val = (fpreal) myTotalPoints;
                return true;
            default:
                /* do nothing */;
        }
    }
    // Not one of our variables, must delegate to the base class.
    return SOP_Node::evalVariableValue(val, index, thread);
}

OP_Node *
SOP_Paani::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_Paani(net, name, op);
}

SOP_Paani::SOP_Paani(OP_Network *net, const char *name, OP_Operator *op)
: SOP_Node(net, name, op)
{
    // This SOP always generates fresh geometry, so setting this flag
    // is a bit redundant, but one could change it to check for the old
    // star and only bump relevant data IDs, (P and the primitive list),
    // depending on what parameters changed.
    mySopFlags.setManagesDataIDs(true);

    scene = new Scene();
    scene->init();
    
    initSceneBool = true;
    myCurrPoint = -1; // To prevent garbage values from being returned
}

SOP_Paani::~SOP_Paani() {
    delete scene;
}

OP_ERROR
SOP_Paani::cookMySop(OP_Context &context)
{
    flags().setTimeDep(1);
    fpreal now = context.getTime();
    
    if(initSceneBool)
    {
        
        UT_String s;
        FILENAME(s, now);
        std::string fileName_s = s.toStdString();
        
        if(strlen(fileName_s.c_str())){
            initSceneBool = false;
            scene->createContainer(fileName_s.c_str());
        }
    }

    if(TOGGLE(now)) { scene->pourFluid(TAPSIZE(now), TAPSEPARATION(now)); }
    scene->setSmoothingRadius(SMOOTHINGRADIUS(now));
    scene->setTimeStep(TIMESCALE(now));
    scene->setSolverIterations(ITERATIONS(now));
    
    scene->update();
    
    // Check to see that there hasn't been a critical error in cooking the SOP.
    if (error() >= UT_ERROR_ABORT)
    {
        myCurrPoint = -1;
        return error();
    }
    
    //    if (divisions < 4)
    //    {
    // With the range restriction we have on the divisions, this
    // is actually impossible, but it shows how to add an error
    // message or warning to the SOP.
    //        addWarning(SOP_MESSAGE, "Invalid divisions");
    //        divisions = 4;
    //    }
    
    // In addition to destroying everything except the empty P
    // and topology attributes, this bumps the data IDs for
    // those remaining attributes, as well as the primitive list
    // data ID.
    gdp->clearAndDestroy();
    
    // Start the interrupt server
    UT_AutoInterrupt boss("Building Paani");
    if (boss.wasInterrupted())
    {
        myCurrPoint = -1;
        return error();
    }
    
    GU_PrimParticle *part = GU_PrimParticle::build(gdp, 0);
    std::vector<Particle> particles = scene->particleSystem->getAllParticles();
    
    for (std::vector<Particle>::iterator it=particles.begin(); it < particles.end(); it++)
    {
        // Check to see if the user has interrupted us...
        if (boss.wasInterrupted())
            break;
        
        // Build a sphere instead of this poly
        Particle particle = *it;
        glm::vec3 newPos = particle.getPosition();
        
        GA_Offset ptoff = gdp->appendPointOffset();
        gdp->setPos3(ptoff, newPos[0],newPos[1],newPos[2]);
        
        part->appendParticle(ptoff);
    }
    
    // Highlight the star which we have just generated.  This routine
    // call clears any currently highlighted geometry, and then it
    // highlights every primitive for this SOP.
    select(GU_SPrimitive);
    
    myCurrPoint = -1;
    return error();
}