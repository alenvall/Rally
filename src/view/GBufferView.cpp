#include "view/GBufferView.h"

#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgreCompositorManager.h>

#include <sstream>
#include <cmath>

namespace Rally { namespace View {

    GBufferView::GBufferView() :
            enabled(false),
            viewport(NULL) {
    }

    GBufferView::~GBufferView() {
    }

    void GBufferView::attachTo(Ogre::Viewport* viewport) {
        this->viewport = viewport;

        renderGBufferMaterialPtr = Ogre::MaterialManager::getSingleton().getByName("compositors/RenderGBuffer");
        if(renderGBufferMaterialPtr.isNull()) {
            throw std::runtime_error("Error finding material for rendering to the GBuffer.");
        }

        Ogre::CompositorInstance* compositorInstance = Ogre::CompositorManager::getSingleton().addCompositor(viewport, "compositors/GBufferCompositor");
        if(compositorInstance == NULL) {
            throw std::runtime_error("The GBuffer compositor could not be instantiated.");
        }

        // compositorInstance->addListener(this);
        setEnabled(true);
    }

    void GBufferView::setEnabled(bool enabled) {
        if(this->enabled == enabled) {
            // Already in that state
            return;
        }

        if(!enabled) {
            Ogre::MaterialManager::getSingleton().removeListener(this, "GBufferScheme");
        }

        Ogre::CompositorManager::getSingleton().setCompositorEnabled(viewport, "compositors/GBufferCompositor", enabled);

        if(enabled) {
            Ogre::MaterialManager::getSingleton().addListener(this, "GBufferScheme");
            // The compositor script will render the scene with scheme GBufferScheme
        }
    }

    void GBufferView::detach() {
        setEnabled(false);
        Ogre::CompositorManager::getSingleton().removeCompositor(viewport, "compositors/GBufferCompositor");
        renderGBufferMaterialPtr.setNull();
    }

    Ogre::Technique* GBufferView::handleSchemeNotFound(unsigned short schemeIndex,
            const Ogre::String& schemeName, Ogre::Material* originalMaterial,
            unsigned short lodIndex, const Ogre::Renderable* renderable) {
        // This listener should be registered for the GBufferScheme only, i.e.
        // we don't need to check the SchemeName explicitly here.

        // Only render the object with one pass of our own GBuffer material
        // Credits of copying the pass like this goes to the Ogre SSAO demo source.
        Ogre::Technique* technique = originalMaterial->createTechnique();
        technique->setSchemeName(schemeName);
        Ogre::Pass* pass = technique->createPass();
        Ogre::Pass* templatePass = renderGBufferMaterialPtr->getTechnique(0)->getPass(0);
        *pass = *templatePass;

        pass->setEmissive(originalMaterial->getTechnique(0)->getPass(0)->getEmissive());

        return technique;
    }

} }
