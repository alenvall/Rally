#ifndef RALLY_VIEW_PORTALVIEW_H_
#define RALLY_VIEW_PORTALVIEW_H_

#include "Rally.h"

#include "OgreManualObject.h"

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

class PortalView {
    public:
        PortalView();
        virtual ~PortalView();
        void detach();
        void attachTo(Ogre::SceneManager* sceneManager, const std::string& portalName);
        void moveCamera(const Rally::Vector3& position, const Rally::Vector3& lookAt);
        void setPosition(const Rally::Vector3& position);
        void setOrientation(const Rally::Quaternion& orientation);
        void setScale(float x, float y);
        void takeSnapshot();

    private:
        Ogre::SceneManager* sceneManager;

        Ogre::TexturePtr magicTexture;
        Ogre::RenderTarget* renderTarget;
        Ogre::Camera* camera;
        Ogre::MaterialPtr material;

        Ogre::MeshPtr displaySurfaceMesh;
        Ogre::Entity* displaySurfaceEntity;
        Ogre::SceneNode* displaySurfaceNode;
};

} }

#endif // RALLY_VIEW_PORTALVIEW_H_
