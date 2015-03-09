#ifndef RALLY_VIEW_BLOOMVIEW_H_
#define RALLY_VIEW_BLOOMVIEW_H_

#include "model/Car.h"

#include "Rally.h"

#include <OgreCompositorInstance.h>
#include <OgreMaterial.h>
#include <OgreGpuProgramParams.h>

namespace Ogre {
    class Viewport;
}

namespace Rally { namespace View {

class BloomView : public Ogre::CompositorInstance::Listener {
    public:
        BloomView();
        virtual ~BloomView();

        virtual void notifyMaterialSetup(Ogre::uint32 compositorPassId, Ogre::MaterialPtr& clonedMaterial);
        virtual void notifyMaterialRender(Ogre::uint32 compositorPassId, Ogre::MaterialPtr& clonedMaterial);

        void attachTo(Ogre::Viewport* viewport, const Rally::Model::Car* car);
        void setEnabled(bool enabled);
        void detach();

    private:
        void recalculateBlur();

        Ogre::CompositorInstance* compositorInstance;
        Ogre::GpuProgramParametersSharedPtr blurHorizontalShaderParameters;
        Ogre::GpuProgramParametersSharedPtr blurVerticalShaderParameters;

        Ogre::Viewport* viewport;
        const Rally::Model::Car* car;

        float blurBaseWeight;
        Ogre::Vector4 blurWeights;
        Ogre::Vector4 blurHorizontalOffsets;
        Ogre::Vector4 blurVerticalOffsets;
};

} }

#endif // RALLY_VIEW_BLOOMVIEW_H_
