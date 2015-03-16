#ifndef RALLY_VIEW_PLAYERCARVIEW_H_
#define RALLY_VIEW_PLAYERCARVIEW_H_

#include "view/CarView.h"
#include "view/ReflectionView.h"

#include "Rally.h"

namespace Rally { namespace Model {
    class RemoteCar;
} }

namespace Rally { namespace View {

class PlayerCarView : public CarView {
    public:
        PlayerCarView();
        virtual ~PlayerCarView();
        virtual void attachTo(Ogre::SceneManager* sceneManager);
        virtual void detach();
        virtual void updateBody(const Rally::Vector3& position, const Rally::Quaternion& orientation);
        virtual void changeCar(char carType);

        void setReflectionsOn(bool state);

        bool isReflectionsOn() {
            return reflectionsOn;
        }

    private:
        bool reflectionsOn;
        Rally::View::ReflectionView reflectionView;
};

} }

#endif // RALLY_VIEW_PLAYERCARVIEW_H_
