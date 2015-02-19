//call this from maincontroller?
	//try initialising the inputmanager from sceneview to get the renderwindow straight?
	//maybe skip the part with the windowlistener and add mouse/keyboardlisteners instead


#include "BaseInputManager.h"

//-------------------------------------------------------------------------------------
BaseInputManager::BaseInputManager(void)
    : mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mCursorWasVisible(false),
    mShutDown(false),
    mInputManager(0),
    mMouse(0),
    mKeyboard(0)
{
	root = Ogre::Root::getSingletonPtr();
}

//-------------------------------------------------------------------------------------
BaseInputManager::~BaseInputManager(void)
{

    //Remove ourself as a Window listener
  //  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete root;
}

//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
void BaseInputManager::createFrameListener(Ogre::RenderWindow* rw)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");

    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

	std::cout << rw->getName() + " 2 ";
    rw->getCustomAttribute("WINDOW", &windowHnd);
	std::cout << "efter getcustomattribute";
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

	std::cout << " inputsystem created ";

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

	std::cout << " inputobjects created ";


    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(rw);

	std::cout << " something bout mouse clipping size ";

    //Register as a Window listener
    //Ogre::WindowEventUtilities::addWindowEventListener(rw, this);

	//std::cout << " windoweventlistener added ";

	root = Ogre::Root::getSingletonPtr();
    root->addFrameListener(this);
	std::cout << " framelistener added ";
}
//-------------------------------------------------------------------------------------
void BaseInputManager::destroyScene(void)
{
}
//-------------------------------------------------------------------------------------

void BaseInputManager::createResourceListener(void)
{

}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
void BaseInputManager::go(void)
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

//    if (!setup())
//        return;

    root->startRendering();

    // clean up
    destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseInputManager::setup(Ogre::RenderWindow* rw)
{

    createFrameListener(rw);

    return true;
};
//-------------------------------------------------------------------------------------
bool BaseInputManager::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mWindow = (Ogre::RenderWindow*)(Ogre::Root::getSingletonPtr()->getRenderTarget("Rally Sport Racing Game"));
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

	//Fix for 1.9
	//Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    return true;
}
//-------------------------------------------------------------------------------------
bool BaseInputManager::keyPressed( const OIS::KeyEvent &arg )
{
     if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
    return true;
}

bool BaseInputManager::keyReleased( const OIS::KeyEvent &arg )
{
	std::cout << " User pressed " << arg.key << std::endl;
    return true;
}

bool BaseInputManager::mouseMoved( const OIS::MouseEvent &arg )
{
    return true;
}

bool BaseInputManager::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
   
    return true;
}

bool BaseInputManager::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    
    return true;
}

//Adjust mouse clipping area
void BaseInputManager::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseInputManager::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}
