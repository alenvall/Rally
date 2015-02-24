#include "InputInit.h"
#include "OISKeyboard.h"

//-------------------------------------------------------------------------------------

InputInit::InputInit(void)
    : mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mCursorWasVisible(false),
    mShutDown(false),
    mInputManager(0),
    mMouse(0),
    mKeyboard(0),
	keyMap(0)
{
	root = Ogre::Root::getSingletonPtr();
}

//-------------------------------------------------------------------------------------
InputInit::~InputInit(void)
{

    //Remove ourself as a Window listener
  //  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete root;
}

//-------------------------------------------------------------------------------------

void InputInit::createFrameListener(Ogre::RenderWindow* rw)
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

	keyMap = new Map();
	keyInit();

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

void InputInit::keyInit(){
	keyMap->at("up") = false;
	keyMap->at("down") = false;
	keyMap->at("left") = false;
	keyMap->at("right") = false;
	keyMap->at("escape") = false;
//	std::cout << "keymap init" << keyMap["up"] << std::endl;
}
//-------------------------------------------------------------------------------------
bool InputInit::setup(Ogre::RenderWindow* rw)
{

    createFrameListener(rw);
    return true;
};
//-------------------------------------------------------------------------------------
bool InputInit::frameRenderingQueued(const Ogre::FrameEvent& evt)
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

//bool InputInit::frameRenderingQueued(const Ogre::FrameEvent& evt)
//{
//    bool ret = frameRenderingQueued1(evt);
 
//    if(!processUnbufferedInput(evt)) return false;
 
//    return ret;
//}

//bool InputInit::processUnbufferedInput(const Ogre::FrameEvent& evt)
//{
  //  static Ogre::Real mToggle = 0.0;    // The time left until next toggle
 
 
    //mToggle -= evt.timeSinceLastFrame;
 
    //if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_1))
    //{
        //mToggle  = 0.5;
    //    Ogre::Light* light = sceneManager->getLight("pointLight");
    //    light->setVisible(! light->isVisible());
    //}
   
    //return true;
//}

bool InputInit::isKeyPressed(const std::string &s){
	std::cout << "the value of key string is: " << s << std::endl;
	return keyMap->at(s);
	
}
//-------------------------------------------------------------------------------------
bool InputInit::keyPressed( const OIS::KeyEvent &arg )
{
	std::cout << " User pressed " << arg.key << std::endl;
    if (arg.key == OIS::KC_UP)
    {
        keyMap->at("up") = true;
    }
	if (arg.key == OIS::KC_DOWN)
    {
        keyMap->at("down") = true;
    }
	if (arg.key == OIS::KC_LEFT)
    {
        keyMap->at("left") = true;
    }
	if (arg.key == OIS::KC_RIGHT)
    {
        keyMap->at("right") = true;
    }
	if (arg.key == OIS::KC_ESCAPE)
    {
        keyMap->at("escape") = true;
		mShutDown = true;

    }
    return true;
}

bool InputInit::keyReleased( const OIS::KeyEvent &arg )
{
	std::cout << " User released " << arg.key << std::endl;
	if (arg.key == OIS::KC_UP)
    {
        keyMap->at("up") = false;
    }
	if (arg.key == OIS::KC_DOWN)
    {
        keyMap->at("down") = false;
    }
	if (arg.key == OIS::KC_LEFT)
    {
        keyMap->at("left") = false;
    }
	if (arg.key == OIS::KC_RIGHT)
    {
        keyMap->at("right") = false;
    }
	if (arg.key == OIS::KC_ESCAPE)
    {
        keyMap->at("escape") = false;

	}
    return true;
}

bool InputInit::mouseMoved( const OIS::MouseEvent &arg )
{
    return true;
}

bool InputInit::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
   
    return true;
}

bool InputInit::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    
    return true;
}

//Adjust mouse clipping area
void InputInit::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void InputInit::windowClosed(Ogre::RenderWindow* rw)
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