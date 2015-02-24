#ifndef RALLY_VIEW_INPUTINIT_H_
#define RALLY_VIEW_INPUTINIT_H_

//#include <OISEvents.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <OgreFrameListener.h>

#include <string>
#include <map>

class InputInit : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
    public:
        InputInit();
        virtual ~InputInit();
        void setup();
        bool isKeyPressed(const std::string &s);

    private:
        typedef std::map<std::string, bool> Map;

        void keyInit();
        void createFrameListener();

        // Ogre::FrameListener
        virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);


        // OIS::KeyListener
        virtual bool keyPressed(const OIS::KeyEvent& arg);
        virtual bool keyReleased(const OIS::KeyEvent& arg);

        // OIS::MouseListener
        virtual bool mouseMoved(const OIS::MouseEvent& arg);
        virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
        virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

        // Ogre::WindowEventListener
        virtual void windowResized(Ogre::RenderWindow* rw);
        virtual void windowClosed(Ogre::RenderWindow* rw);

        Ogre::Root* root;
        Ogre::RenderWindow* renderWindow;
        InputInit::Map keyMap;

        bool shutDown;

        // OIS Input devices
        OIS::InputManager* mInputManager;
        OIS::Mouse*    mMouse;
        OIS::Keyboard* mKeyboard;

};

#endif // RALLY_VIEW_INPUTINIT_H_
