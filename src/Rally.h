#ifndef RALLY_H_
#define RALLY_H_

#if _WIN32
#define PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#endif

#include <OgreVector3.h>
#include <OgreVector4.h>
#include <OgreQuaternion.h>

// Todo: Move this to shared file.
namespace Rally {
    typedef Ogre::Vector3 Vector3;
    typedef Ogre::Vector4 Vector4;
    typedef Ogre::Quaternion Quaternion;
    typedef Ogre::Degree Degree;
    typedef Ogre::Radian Radian;
}

#endif // RALLY_H_
