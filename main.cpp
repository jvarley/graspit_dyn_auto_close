#include "dynAutoClose.h"
#include <iostream>
#include <string>

extern "C" PLUGIN_API Plugin* createPlugin() {
  return new GraspGenerationPlugin();
}


extern "C" PLUGIN_API std::string getType() {
  return "dyn_auto_close";
}
