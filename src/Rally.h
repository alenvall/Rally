#ifndef RALLY_H_
#define RALLY_H_

#if _WIN32
#define PLATFORM_WINDOWS
#endif

#if PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <OgreVector3.h>
#include <OgreQuaternion.h>

// Todo: Move this to shared file.
namespace Rally {
    typedef Ogre::Vector3 Vector3;
    typedef Ogre::Quaternion Quaternion;
}

#endif // RALLY_H_
