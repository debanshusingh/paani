//
//  SOP_Paani.h
//  paani
//
//  Created by Debanshu on 3/20/15.
//  Copyright (c) 2015 Debanshu. All rights reserved.
//

#ifndef __paani__SOP_Paani__
#define __paani__SOP_Paani__

#include <SOP/SOP_Node.h>
#include "scene.h"

namespace HDK_Sample {
    class SOP_Paani : public SOP_Node
    {
    public:
        static OP_Node		*myConstructor(OP_Network*, const char *,
                                           OP_Operator *);
        
        /// Stores the description of the interface of the SOP in Houdini.
        /// Each parm template refers to a parameter.
        static PRM_Template		 myTemplateList[];
        
        /// This optional data stores the list of local variables.
        static CH_LocalVariable	 myVariables[];
        
    protected:
        SOP_Paani(OP_Network *net, const char *name, OP_Operator *op);
        virtual ~SOP_Paani();
        
        
        /// cookMySop does the actual work of the SOP computing, in this
        /// case, a star shape.
        virtual OP_ERROR		 cookMySop(OP_Context &context);
        
        /// This function is used to lookup local variables that you have
        /// defined specific to your SOP.
        virtual bool		 evalVariableValue(
                                               fpreal &val,
                                               int index,
                                               int thread);
        // Add virtual overload that delegates to the super class to avoid
        // shadow warnings.
        virtual bool		 evalVariableValue(
                                               UT_String &v,
                                               int i,
                                               int thread)
        {
            return evalVariableValue(v, i, thread);
        }
        
    private:
        /// The following list of accessors simplify evaluating the parameters
        /// of the SOP.
        void FILENAME(UT_String &s, fpreal t) { evalString(s, "file", 0, t); }
        int TOGGLE(fpreal t) { return evalInt ("tapToggle", 0, t); }
        float SMOOTHINGRADIUS(fpreal t) { return evalFloat ("sphSmoothingRadius", 0, t); }
        int TIMESCALE(fpreal t) { return evalFloat ("timescale", 0, t); }
        int ITERATIONS(fpreal t) { return evalInt ("solverIterations", 0, t); }
        int TAPSIZE(fpreal t) { return evalInt ("tapSize", 0, t); }
        float TAPSEPARATION(fpreal t) { return evalFloat ("tapParticleSeparation", 0, t); }
        
        /// Member variables are stored in the actual SOP, not with the geometry
        /// In this case these are just used to transfer data to the local 
        /// variable callback.
        /// Another use for local data is a cache to store expensive calculations.
        int		myCurrPoint;
        int		myTotalPoints;
    };
} // End HDK_Sample namespace


#endif /* defined(__paani__SOP_Paani__) */
