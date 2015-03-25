#ifndef RALLY_VIEW_GBUFFERVIEW_H_
#define RALLY_VIEW_GBUFFERVIEW_H_

#include "Rally.h"

//#include <OgreCompositorInstance.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
//#include <OgreGpuProgramParams.h>

namespace Ogre {
    class Viewport;
}

namespace Rally { namespace View {

class GBufferView : /*public Ogre::CompositorInstance::Listener*/ public Ogre::MaterialManager::Listener {
    public:
        GBufferView();
        virtual ~GBufferView();

        // virtual void notifyMaterialSetup(Ogre::uint32 compositorPassId, Ogre::MaterialPtr& clonedMaterial);
        // virtual void notifyMaterialRender(Ogre::uint32 compositorPassId, Ogre::MaterialPtr& clonedMaterial);

        virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex,
            const Ogre::String& schemeName, Ogre::Material* originalMaterial,
            unsigned short lodIndex, const Ogre::Renderable* renderable);

        void attachTo(Ogre::Viewport* viewport);
        void setEnabled(bool enabled);
        void detach();

    private:
        bool enabled;
        Ogre::MaterialPtr renderGBufferMaterialPtr;
        // Ogre::CompositorInstance* compositorInstance;
        Ogre::Viewport* viewport;
};

} }

#endif // RALLY_VIEW_GBUFFERVIEW_H_
