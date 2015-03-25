#include "view/PlayerCarView.h"
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <sstream>
#include <string>

namespace Rally { namespace View {

    PlayerCarView::PlayerCarView() :
            reflectionsOn(false) {
        std::set<std::string> reflected;
        reflected.insert("carcolourred");
        reflected.insert("blackplastic");
        reflectionView.setReflectionReceivers(reflected);
    }

    PlayerCarView::~PlayerCarView() {
    }

    void PlayerCarView::attachTo(Ogre::SceneManager* sceneManager, Rally::Model::Car& car) {
        CarView::attachTo(sceneManager, "PlayerCar");
		skidmarkView.attachTo(sceneManager, car);
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

    void PlayerCarView::changeCar(char carType) {
        bool hadReflections = reflectionsOn;

        if(hadReflections) setReflectionsOn(false);

        CarView::changeCar(carType);

        std::set<std::string> reflected;
        reflected.insert(carEntity->getSubEntity(4)->getMaterialName());
        switch(carType) {
            case 'a':
            case 'd':
            case 'g':
            case 'j':
            case 'm':
            case 'p':
                reflected.insert("blackplastic");
            break;
        }
        reflectionView.setReflectionReceivers(reflected);

        if(hadReflections) setReflectionsOn(true);
    }

    void PlayerCarView::updateBody(const Rally::Vector3& position, const Rally::Quaternion& orientation) {
        if(reflectionsOn) {
            reflectionView.moveCamera(position);
        }
        CarView::updateBody(position, orientation);
		skidmarkView.update();
    }

    void PlayerCarView::setEffectFactor(float effectFactor) {
        if(reflectionsOn) {
            reflectionView.setEffectFactor(effectFactor);
        }
    }

} }
