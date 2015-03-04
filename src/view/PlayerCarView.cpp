#include "view/PlayerCarView.h"
#include <OgreEntity.h>
#include <sstream>
#include <string>

namespace Rally { namespace View {

    namespace {
        std::set<std::string> getReflectedMaterials() {
            std::set<std::string> reflected;
            reflected.insert("carcolourhotpink");
            reflected.insert("blackplastic");
            return reflected;
        }
    }

    PlayerCarView::PlayerCarView() :
            reflectionView(getReflectedMaterials()),
            reflectionsOn(false) {
    }

    PlayerCarView::~PlayerCarView() {
    }

    void PlayerCarView::attachTo(Ogre::SceneManager* sceneManager) {
        CarView::attachTo(sceneManager, "PlayerCar");
    }

    void PlayerCarView::detach() {
        if(reflectionsOn) reflectionView.detach();
        CarView::detach();
    }

    void PlayerCarView::setReflectionsOn(bool state) {
        if(reflectionsOn == false && state == true) {
            reflectionsOn = true;
            reflectionView.attachTo(sceneManager, carEntity, "PlayerCarReflection");
        } else if(reflectionsOn == true && state == false) {
            reflectionView.detach();
            reflectionsOn = false;
        }
    }

    void PlayerCarView::updateBody(const Rally::Vector3& position, const Rally::Quaternion& orientation) {
        if(reflectionsOn) {
            reflectionView.moveCamera(position);
        }
        CarView::updateBody(position, orientation);
    }

} }
