#ifndef GRASPGENERATOR_H
#define GRASPGENERATOR_H

#include <map>
#include <QObject>
#include <QtGui>

//GraspIt! includes
#include <include/plugin.h>


class EGPlanner;
class GraspPlanningState;
class GraspableBody;
class Hand;



//! Main class, combining a ROS node with a GraspIt! interface
/*! Note that this class inherits from GraspIt's Plugin class and implements the necessary functions to serve
  as a GraspIt plugin. See include/plugin.h in the GraspIt code for the base class.
  Provides a number of ROS services that directly operate on the GraspIt world, such as loading objects or
  obstacles, simulating 3D scans of objects, etc.
  In particular, note that this class uses the mainLoop() function to perform the ROS event management calls.
*/
class GraspGenerationPlugin : public QObject, public Plugin
{

    Q_OBJECT

public:
  //! Inits ROS, but (for now) without passing any arguments
  GraspGenerationPlugin();
  //! Deletes the node handle and the db manager
  ~GraspGenerationPlugin();
  //! Creates the node handles, advertises services, connects to the database
  virtual int init(int argc, char **argv);
  //! Simply calls ros::spinOnce() to process the ROS event loop
  virtual int mainLoop();

protected:
  void startPlanner();
  void dynAutoClose();
  void stepPlanner();

private:

  EGPlanner *mPlanner;
  GraspPlanningState *mHandObjectState;
  GraspableBody *mObject;
  Hand *mHand;
  bool plannerStarted;
  bool plannerFinished;
  bool autoClosing;
  bool finished;
  int myInteger;

  bool render_it;


};


#endif // GRASPGENERATOR_H
