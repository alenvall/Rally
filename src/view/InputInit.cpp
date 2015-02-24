#include "InputInit.h"

#include "OISKeyboard.h"

#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreLogManager.h>

InputInit::InputInit() :
        renderWindow(0),
        shutDown(false),
        mInputManager(0),
        mMouse(0),
        mKeyboard(0) {
    root = Ogre::Root::getSingletonPtr();
}

InputInit::~InputInit() {
    //Remove ourself as a Window listener
    //  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(renderWindow);
}

void InputInit::setup() {
    renderWindow = (Ogre::RenderWindow*)(Ogre::Root::getSingletonPtr()->getRenderTarget("Rally Sport Racing Game"));
    keyInit();
    createFrameListener();
}

void InputInit::createFrameListener()
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");

    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    renderWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem(pl);

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(renderWindow);

    //Register as a Window listener
    //Ogre::WindowEventUtilities::addWindowEventListener(rw, this);

    root = Ogre::Root::getSingletonPtr();
    root->addFrameListener(this);
}

void InputInit::keyInit() {
    keyMap["up"] = false;
    keyMap["down"] = false;
    keyMap["left"] = false;
    keyMap["right"] = false;
    keyMap["escape"] = false;
}

bool InputInit::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(renderWindow->isClosed())
        return false;

    if(shutDown)
        return false;

    //Fix for 1.9
    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    return true;
}

bool InputInit::isKeyPressed(const std::string& s) {
    return keyMap.at(s);
}

bool InputInit::keyPressed(const OIS::KeyEvent& arg) {
    if (arg.key == OIS::KC_UP)
    {
        keyMap.at("up") = true;
    }
    if (arg.key == OIS::KC_DOWN)
    {
        keyMap.at("down") = true;
    }
    if (arg.key == OIS::KC_LEFT)
    {
        keyMap.at("left") = true;
    }
    if (arg.key == OIS::KC_RIGHT)
    {
        keyMap.at("right") = true;
    }
    if (arg.key == OIS::KC_ESCAPE)
    {
        keyMap.at("escape") = true;
        shutDown = true;
    }
    return true;
}

bool InputInit::keyReleased(const OIS::KeyEvent& arg) {
    if (arg.key == OIS::KC_UP)
    {
        keyMap.at("up") = false;
    }
    if (arg.key == OIS::KC_DOWN)
    {
        keyMap.at("down") = false;
    }
    if (arg.key == OIS::KC_LEFT)
    {
        keyMap.at("left") = false;
    }
    if (arg.key == OIS::KC_RIGHT)
    {
        keyMap.at("right") = false;
    }
    if (arg.key == OIS::KC_ESCAPE)
    {
        keyMap.at("escape") = false;

    }
    return true;
}

bool InputInit::mouseMoved(const OIS::MouseEvent& arg) {
    return true;
}

bool InputInit::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) {
    return true;
}

bool InputInit::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id) {
    return true;
}

//Adjust mouse clipping area
void InputInit::windowResized(Ogre::RenderWindow* rw) {
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void InputInit::windowClosed(Ogre::RenderWindow* rw) {
    //Only close for window that created OIS (the main window in these demos)
    if( rw == renderWindow )
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
