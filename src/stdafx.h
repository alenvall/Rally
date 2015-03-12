// stdafx.h
 
// Windows
#if _WIN32
#include "WinSock2.h"
#include "windows.h"
#include <mmsystem.h>
#endif

// Ogre
#include <Ogre.h>
#include <OgreCamera.h>
#include <OgreCompositorInstance.h>
#include <OgreCompositorManager.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreException.h>
#include <OgreFrameListener.h>
#include <OgreGpuProgramParams.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreManualObject.h>
#include <OgreMaterial.h>
#include <OgreQuaternion.h>
#include <OgreRenderTargetListener.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreString.h>
#include <OgreSubEntity.h>
#include <OgreTechnique.h>
#include <OgreVector3.h>
#include <OgreViewport.h>
#include <OgreWindowEventUtilities.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include "util/rapidxml.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>

// Bullet
#include <btBulletDynamicsCommon.h>
#include <../Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>

// Standard
#include <climits>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
