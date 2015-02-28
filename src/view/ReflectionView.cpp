#include "view/ReflectionView.h"

#include <OgreRoot.h>
#include <OgreSubEntity.h>
#include <OgreEntity.h>
#include <OgreHardwarePixelBuffer.h>

#include <sstream>
#include <set>

namespace Rally { namespace View {

    ReflectionView::ReflectionView(const std::set<std::string>& reflectionReceivers) :
            sceneManager(NULL),
            renderTargets(),
            camera(NULL),
            oldMaterialPtrs(NULL),
            reflectionReceivers(reflectionReceivers) {
        // Ogre uses the same order as the OpenGL specification for cube maps:
        // 0 GL_TEXTURE_CUBE_MAP_POSITIVE_X
        // 1 GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        // 2 GL_TEXTURE_CUBE_MAP_POSITIVE_Y
        // 3 GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        // 4 GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        // 5 GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
        cameraOrientations[0] = Ogre::Vector3::NEGATIVE_UNIT_Z.getRotationTo(Ogre::Vector3::UNIT_X);
        cameraOrientations[1] = Ogre::Vector3::NEGATIVE_UNIT_Z.getRotationTo(Ogre::Vector3::NEGATIVE_UNIT_X);
        cameraOrientations[2] = Ogre::Vector3::NEGATIVE_UNIT_Z.getRotationTo(Ogre::Vector3::UNIT_Y);
        cameraOrientations[3] = Ogre::Vector3::NEGATIVE_UNIT_Z.getRotationTo(Ogre::Vector3::NEGATIVE_UNIT_Y);
        cameraOrientations[4] = Ogre::Vector3::NEGATIVE_UNIT_Z.getRotationTo(Ogre::Vector3::UNIT_Z);
        cameraOrientations[5] = Ogre::Vector3::NEGATIVE_UNIT_Z.getRotationTo(Ogre::Vector3::NEGATIVE_UNIT_Z);
    }

    ReflectionView::~ReflectionView() {
    }

    void ReflectionView::detach() {
        if(!reflectionTexture.isNull()) {
            // Set back the old materials and clean up the reflected ones.
            unsigned int subEntityCount = entity->getNumSubEntities();
            for(unsigned int subEntityId = 0; subEntityId < subEntityCount; subEntityId++) {
                if(!oldMaterialPtrs[subEntityId].isNull()) {
                    Ogre::SubEntity* subEntity = entity->getSubEntity(subEntityId);
                    Ogre::MaterialPtr reflectiveMaterial = subEntity->getMaterial();

                    subEntity->setMaterial(oldMaterialPtrs[subEntityId]);
                    oldMaterialPtrs[subEntityId].setNull();

                    Ogre::MaterialManager::getSingleton().remove(reflectiveMaterial->getHandle());
                    reflectiveMaterial.setNull();
                }
            }

            delete[] oldMaterialPtrs;
            oldMaterialPtrs = NULL;

            for(int i = 0; i < 6; i++) {
                renderTargets[i]->removeListener(this);
                renderTargets[i]->removeAllViewports();
                renderTargets[i] = NULL;
            }

            sceneManager->destroyCamera(camera);
            camera = NULL;

            if(!reflectionTexture.isNull()) Ogre::TextureManager::getSingleton().remove(reflectionTexture->getHandle());
            reflectionTexture.setNull();
            sceneManager = NULL;
        }
    }

    void ReflectionView::attachTo(Ogre::SceneManager* sceneManager, Ogre::Entity* appliedTo, const std::string& reflectionName) {
        if(!reflectionTexture.isNull()) {
            return;
        }

        this->sceneManager = sceneManager;
        this->entity = appliedTo;

        // Create a RTT texture
        reflectionTexture = Ogre::TextureManager::getSingleton().createManual(reflectionName + "_Texture",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_CUBE_MAP,
            128, 128, // width, height
            0, // # of mipmaps (0 = none), this and filter type needs to be set on the texture unit below too
            Ogre::PF_R8G8B8,
            Ogre::TU_RENDERTARGET, // Makes it a RTT texture with a render target
            NULL, // Manual resource loader (?)
            false, // No gamma correction
            0 // No FSAA for now
            /* fsaaHint optional parameter */);

        camera = sceneManager->createCamera(reflectionName + "_Camera");
        camera->setNearClipDistance(0.5f);
        camera->setFarClipDistance(1000.0f);
        camera->setFixedYawAxis(false);
        camera->setFOVy(Ogre::Degree(90));
        camera->setAspectRatio(1.0f);

        // Setup render target for each face
        for(unsigned int i = 0; i < 6; i++) {
            renderTargets[i] = reflectionTexture->getBuffer(i)->getRenderTarget();
            Ogre::Viewport* viewport = renderTargets[i]->addViewport(camera);
            viewport->setOverlaysEnabled(false);
            renderTargets[i]->addListener(this);
        }

        // Replace materials with reflected ones
        unsigned int subEntityCount = entity->getNumSubEntities();
        oldMaterialPtrs = new Ogre::MaterialPtr[subEntityCount];
        for(unsigned int subEntityId = 0; subEntityId < subEntityCount; subEntityId++) {
            Ogre::SubEntity* subEntity = entity->getSubEntity(subEntityId);
            Ogre::MaterialPtr oldMaterial = subEntity->getMaterial();

            if(reflectionReceivers.count(oldMaterial->getName()) > 0) {
                Ogre::MaterialPtr newMaterial = oldMaterial->clone(oldMaterial->getName() + "__" + reflectionName + "_Reflection");

                Ogre::TextureUnitState* textureUnitState = newMaterial->getTechnique(0)->getPass(0)->createTextureUnitState();
                textureUnitState->setCubicTexture(&reflectionTexture, true);
                textureUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                textureUnitState->setEnvironmentMap(true, Ogre::TextureUnitState::ENV_REFLECTION);

                oldMaterialPtrs[subEntityId] = oldMaterial;
                subEntity->setMaterial(newMaterial);
            } else {
                oldMaterialPtrs[subEntityId].setNull();
            }
        }
    }

    void ReflectionView::preRenderTargetUpdate(const Ogre::RenderTargetEvent& event) {
        entity->setVisible(false); // Hide to not create garbage if not perfectly convex.

        // Orient the camera so that it looks at the correct way for this snapshot
        for(unsigned int i = 0; i < 6; i++) {
            if(event.source == renderTargets[i]) {
                camera->setOrientation(cameraOrientations[i]);
                break;
            }
        }
    }

    void ReflectionView::postRenderTargetUpdate(const Ogre::RenderTargetEvent& event) {
        entity->setVisible(true);
    }

    void ReflectionView::moveCamera(const Rally::Vector3& position) {
        camera->setPosition(position);
    }

} }
