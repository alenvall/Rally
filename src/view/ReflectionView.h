#ifndef RALLY_VIEW_REFLECTIONVIEW_H_
#define RALLY_VIEW_REFLECTIONVIEW_H_

#include "Rally.h"

#include <OgreFrameListener.h>
#include <OgreManualObject.h>
#include <OgreRenderTargetListener.h>

#include <string>
#include <set>

namespace Ogre {
    class SceneManager;
    // class TexturePtr;
    class RenderTarget;
    class Camera;
    // class MeshPtr;
    class Entity;
    class SceneNode;
}

namespace Rally { namespace View {

class ReflectionView : public Ogre::FrameListener, public Ogre::RenderTargetListener {
    public:
        ReflectionView(const std::set<std::string>& reflectionReceivers);
        virtual ~ReflectionView();

        virtual bool frameStarted(const Ogre::FrameEvent& event);

        virtual void preRenderTargetUpdate(const Ogre::RenderTargetEvent& event);
        virtual void postRenderTargetUpdate(const Ogre::RenderTargetEvent& event);

        void detach();
        void attachTo(Ogre::SceneManager* sceneManager, Ogre::Entity* entity, const std::string& reflectionName);
        void moveCamera(const Rally::Vector3& position);

    private:
        Ogre::SceneManager* sceneManager;
        Ogre::Entity* entity;

        std::string reflectionName;

        Ogre::TexturePtr reflectionTexture;
        Ogre::RenderTarget* renderTargets[6];
        int renderRoundRobin;
        Ogre::Quaternion cameraOrientations[6];
        Ogre::Camera* camera;

        Ogre::MaterialPtr* oldMaterialPtrs;

        std::set<std::string> reflectionReceivers;
};

} }

#endif // RALLY_VIEW_REFLECTIONVIEW_H_
