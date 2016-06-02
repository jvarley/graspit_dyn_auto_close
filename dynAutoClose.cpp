#include "dynAutoClose.h"
#include <cmath>
#include <fstream>

#include <include/world.h>
#include <include/body.h>
#include <include/robot.h>
#include <include/graspitGUI.h>
#include <include/ivmgr.h>

#include <include/EGPlanner/egPlanner.h>
#include <include/EGPlanner/simAnnPlanner.h>
#include <include/EGPlanner/onLinePlanner.h>
#include <include/EGPlanner/guidedPlanner.h>
#include <include/EGPlanner/searchState.h>
#include <include/EGPlanner/energy/searchEnergy.h>

#include <include/grasp.h>
#include <include/triangle.h>

#include <cstdlib>
#include <iostream>



GraspGenerationPlugin::GraspGenerationPlugin() :
    mPlanner(NULL),
    plannerStarted(false),
    plannerFinished(false),
    autoClosing(false),
    finished(false)

{

}

GraspGenerationPlugin::~GraspGenerationPlugin()
{
}        


int GraspGenerationPlugin::init(int argc, char **argv)
{
    return 0;
}

//This loop is called over and over again. We do 3 different things
// 1) First step: start the planner
// 2) Middle steps: step the planner
// 3) Last step, save the grasps
int GraspGenerationPlugin::mainLoop()
{
    //start planner
    if (!plannerStarted)
    {
        startPlanner();
    }
    //let planner run.
    else if( (plannerStarted) && !plannerFinished )
    {
        stepPlanner();
    }
    //save grasps
    else if(plannerStarted && plannerFinished && (!autoClosing))
    {
        dynAutoClose();
    }
    else if(mHand->dynamicAutograspComplete() && !finished)
    {
        graspItGUI->getMainWorld()->turnOffDynamics();
        finished=true;

//        mHand->autoGrasp(true, -1);
//        mHand->approachToContact(50);
//        //enable dynamics
//        graspItGUI->getMainWorld()->turnOnDynamics();

//        //dynamic execute
//        mPlanner->getGrasp(0)->dynamicExecute(mHand);

        std::cout << "AutoGrasp Complete!!!!!!!!" << std::endl;

    }
    else if(mHand->dynamicAutograspComplete() && finished)
    {
        graspItGUI->getMainWorld()->turnOffDynamics();
        finished=true;

    }

  return 0;
}

void GraspGenerationPlugin::startPlanner()
{
    mObject = graspItGUI->getMainWorld()->getGB(0);
    mObject->setMaterial(5);//rubber

    mHand = graspItGUI->getMainWorld()->getCurrentHand();
    mHand->getGrasp()->setObjectNoUpdate(mObject);
    mHand->getGrasp()->setGravity(false);

    mHandObjectState = new GraspPlanningState(mHand);
    mHandObjectState->setObject(mObject);
    mHandObjectState->setPositionType(SPACE_AXIS_ANGLE);
    mHandObjectState->setRefTran(mObject->getTran());
    mHandObjectState->reset();

//    mPlanner = new SimAnnPlanner(mHand);
//    ((SimAnnPlanner*)mPlanner)->setModelState(mHandObjectState);

    mPlanner = new GuidedPlanner(mHand);
    ((SimAnnPlanner*)mPlanner)->setModelState(mHandObjectState);

//        mPlanner = new OnLinePlanner(mHand);
//        ((SimAnnPlanner*)mPlanner)->setModelState(mHandObjectState);

    mPlanner->setEnergyType(ENERGY_CONTACT_QUALITY);
    mPlanner->setContactType(CONTACT_PRESET);
    mPlanner->setMaxSteps(50000);

    mPlanner->resetPlanner();

    mPlanner->startPlanner();
    plannerStarted = true;
}

void GraspGenerationPlugin::stepPlanner()
{
    if ( mPlanner->getCurrentStep() >= 50000)
    {
        mPlanner->stopPlanner();
        mPlanner->getGrasp(0)->execute(mHand);
        plannerFinished=true;
    }
}

void GraspGenerationPlugin::dynAutoClose()
{
    autoClosing=true;
    //place hand in planned grasp location
    mPlanner->getGrasp(0)->execute(mHand);

    //open hand without collisions
    graspItGUI->getMainWorld()->toggleCollisions(false, mObject, mHand);
    mHand->autoGrasp(true, -1);
    graspItGUI->getMainWorld()->toggleCollisions(true, mObject, mHand);

    //enable dynamics
    graspItGUI->getMainWorld()->turnOnDynamics();

    //dynamic execute
    mPlanner->getGrasp(0)->dynamicExecute(mHand);
    mHand->autoGrasp(true);

}
