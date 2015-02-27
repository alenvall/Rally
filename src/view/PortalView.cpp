#include "view/PortalView.h"

#include <OgreRoot.h>
#include <OgreEntity.h>
#include <OgreHardwarePixelBuffer.h>

#include <sstream>
#include <string>

namespace Rally { namespace View {

    PortalView::PortalView() :
            sceneManager(NULL),
            renderTarget(NULL),
            camera(NULL),
            // displaySurfaceMesh(NULL),
            displaySurfaceEntity(NULL),
            displaySurfaceNode(NULL) {
    }

    PortalView::~PortalView() {
    }

    void PortalView::detach() {
        if(sceneManager != NULL) {
            if(displaySurfaceNode) sceneManager->destroySceneNode(displaySurfaceNode);
            if(displaySurfaceEntity) sceneManager->destroyEntity(displaySurfaceEntity);
            // Todo: How to destroy mesh?
            // Todo: How to unlink camera so that it can be destroyed?
            if(!material.isNull()) Ogre::MaterialManager::getSingleton().remove(material->getHandle());
            if(!magicTexture.isNull()) Ogre::TextureManager::getSingleton().remove(magicTexture->getHandle());
        }

        displaySurfaceMesh.setNull();
        material.setNull();
        magicTexture.setNull();
    }

    void PortalView::attachTo(Ogre::SceneManager* sceneManager, const std::string& portalName) {
        this->sceneManager = sceneManager;

        // Create a RTT texture, and acquire buffers.
        magicTexture = Ogre::TextureManager::getSingleton().createManual(portalName + "_Texture",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_2D,
            1024, 1024, // width, height
            0, // # of mipmaps (0 = none), this and filter type needs to be set on the texture unit below too
            Ogre::PF_R8G8B8,
            Ogre::TU_RENDERTARGET, // Makes it a RTT texture with a render target
            NULL, // Manual resource loader (?)
            false, // No gamma correction
            0 // No FSAA for now
            /* fsaaHint optional parameter */);
        renderTarget = magicTexture->getBuffer()->getRenderTarget();

        // Don't capture snapshots every frame!
        renderTarget->setAutoUpdated(false);

        camera = sceneManager->createCamera(portalName + "_Camera");
        camera->setNearClipDistance(0.5f);
        camera->setFarClipDistance(1000.0f);
        camera->setAspectRatio(1.0f);

        Ogre::Viewport* viewport = renderTarget->addViewport(camera);
        viewport->setAutoUpdated(true); // Shouldn't affect us since the render target isn't auto-updated.
        viewport->setBackgroundColour(Ogre::ColourValue::Black); // Probably a good value for portals

        // Create material which uses the magic texture
        material = Ogre::MaterialManager::getSingleton().create(portalName + "_Material",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        Ogre::TextureUnitState* textureUnitState = material->getTechnique(0)->getPass(0)->createTextureUnitState();
        textureUnitState->setTexture(magicTexture);
        textureUnitState->setNumMipmaps(0);
        textureUnitState->setTextureFiltering(Ogre::TFO_BILINEAR);

        // Create a manual with a flat square surface, then conver it to a mesh.
        Ogre::ManualObject* displaySurfaceManual = sceneManager->createManualObject();
        displaySurfaceManual->setDynamic(false);

        displaySurfaceManual->begin("BaseWhiteNoLighting",  Ogre::RenderOperation::OT_TRIANGLE_LIST);
        displaySurfaceManual->position(Ogre::Vector3(-1.0f, 1.0f, 0));
        displaySurfaceManual->textureCoord(0.0f, 0.0f);

        displaySurfaceManual->position(Ogre::Vector3(1.0f, 1.0f, 0));
        displaySurfaceManual->textureCoord(1.0f, 0.0f);

        displaySurfaceManual->position(Ogre::Vector3(1.0f, -1.0f, 0));
        displaySurfaceManual->textureCoord(1.0f, 1.0f);

        displaySurfaceManual->position(Ogre::Vector3(-1.0f, -1.0f, 0));
        displaySurfaceManual->textureCoord(0.0f, 1.0f);

        displaySurfaceManual->triangle(2, 1, 0);
        displaySurfaceManual->triangle(0, 3, 2);
        displaySurfaceManual->end();

        displaySurfaceMesh = displaySurfaceManual->convertToMesh(portalName + "_Mesh");
        sceneManager->destroyManualObject(displaySurfaceManual);

        // Instantiate an entity with the surface mesh and place it in the world.
        displaySurfaceEntity = sceneManager->createEntity(portalName + "_Entity", displaySurfaceMesh);
        displaySurfaceEntity->setMaterial(material);
        displaySurfaceNode = sceneManager->getRootSceneNode()->createChildSceneNode();
        displaySurfaceNode->attachObject(displaySurfaceEntity);
    }

    void PortalView::moveCamera(const Rally::Vector3& position, const Rally::Vector3& lookAt) {
        camera->setPosition(position);
        camera->lookAt(lookAt);
    }

    void PortalView::setPosition(const Rally::Vector3& position) {
        displaySurfaceNode->setPosition(position);
    }

    void PortalView::setOrientation(const Rally::Quaternion& orientation) {
        displaySurfaceNode->setOrientation(orientation);
    }

    void PortalView::setScale(float x, float y, bool scaleViewport) {
        float aspectRatio = x / y;
        if(scaleViewport) {
            float maxCoord = (x > y) ? x : y;
            displaySurfaceNode->scale(maxCoord, maxCoord, 0);

            // Maximize the area in the square texture
            if(aspectRatio == 1.0f) {
                renderTarget->getViewport(0)->setDimensions(
                    1.0f, // left
                    aspectRatio / 2.0f, // top
                    1.0f, // width
                    1.0f - aspectRatio // height
                );
            } else if(aspectRatio > 1.0f) {
                float invertedAspectRatio = 1.0f / aspectRatio;
                renderTarget->getViewport(0)->setDimensions(
                    0.0f, // left
                    (1.0f - invertedAspectRatio) / 2.0f, // top
                    1.0f, // width
                    invertedAspectRatio // height
                );
            } else {
                renderTarget->getViewport(0)->setDimensions(
                    (1.0f - aspectRatio) / 2.0f, // left
                    0.0f, // top
                    aspectRatio, // width
                    1.0f // height
                );
            }
        } else {
            camera->setAspectRatio(aspectRatio);
            displaySurfaceNode->scale(x, y, 0);
        }
    }

    void PortalView::takeSnapshot() {
        renderTarget->update();
    }

} }
