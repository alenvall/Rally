// stdafx.h
 
// Windows
#if _WIN32
#include "WinSock2.h"
#include "windows.h"
#endif

// Ogre
#include <Ogre.h>
#include <OgreException.h>
#include <OgreRoot.h>
#include <OgreConfigFile.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreFrameListener.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgreString.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreResourceGroupManager.h>
#include <OgreManualObject.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

// Bullet
#include <btBulletDynamicsCommon.h>
#include <../Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>
