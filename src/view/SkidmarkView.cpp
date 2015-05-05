#include "view/SkidmarkView.h"

#include <OgreRoot.h>
#include <OgreBillboardSet.h>
#include <OgreBillboard.h>

#include <sstream>
#include <string>

namespace Rally { namespace View {


	SkidmarkView::SkidmarkView() {}

	SkidmarkView::~SkidmarkView() {}

	void SkidmarkView::attachTo(Ogre::SceneManager* sceneManager, Rally::Model::Car& car){
		this->car = &car;
		skidmarkNode = sceneManager->getRootSceneNode()->createChildSceneNode();

		Ogre::BillboardType type = Ogre::BBT_PERPENDICULAR_SELF;
		Ogre::BillboardRotationType rotationType = Ogre::BBR_VERTEX;
		Rally::Vector3 up(0, 0, 1);
		Rally::Vector3 common(0, 1, 0);

		skidmarkBillboards = sceneManager->createBillboardSet();
		skidmarkBillboards->setMaterialName("skidmark");
		skidmarkBillboards->setVisible(true);
		skidmarkBillboards->setBillboardType(type);
		skidmarkBillboards->setBillboardRotationType(rotationType);
		skidmarkBillboards->setCommonUpVector(up);
		skidmarkBillboards->setCommonDirection(common);
		skidmarkBillboards->setBillboardsInWorldSpace(true);
		skidmarkBillboards->setDefaultDimensions(Ogre::Real(0.14), Ogre::Real(0.35));
		skidmarkBillboards->setAutoextend(true);

		skidmarkNode->attachObject(skidmarkBillboards);
	}

	void SkidmarkView::update(){
		std::list<Rally::Vector3> positions;
		std::list<Rally::Vector3> normals;
		std::list<Rally::Vector3> directions;
        std::list<float> tractions;
		std::list<Rally::Vector3>::iterator iterator;
		std::list<Rally::Vector3>::iterator iteratorNormals;
		std::list<Rally::Vector3>::iterator iteratorDirections;
        std::list<float>::iterator iteratorTractions;

        float traction = 1.0f;

		for(int i = 0; i < 4; i++){
			positions = car->getSkidmarkPositions(i);
			normals = car->getSkidmarkNormals(i);
			directions = car->getSkidmarkDirections(i);
			tractions = car->getSkidmarkTractions(i);

			if(positions.size() > 0){
				iterator = positions.begin();
				iteratorNormals = normals.begin();
				iteratorDirections = directions.begin();
                iteratorTractions = tractions.begin();

                if(positions.size() >= 4 && normals.size() >= 4 && directions.size() >= 4){
					createSkidmark(*iterator, *iteratorNormals, 
						*iteratorDirections, *iteratorTractions);

					for(int n = 0, size = positions.size(); n < size; n++){
						Rally::Vector3 temp = *iterator;
						Rally::Vector3 tempDir = *iteratorDirections;
						float tempTraction = *iteratorTractions;

						int m = positions.size()-n-1;
						if(m > 5)
							m = 5;
						while(m > 0){
							for(int a = 0; a < m; a++){
								++iterator;
								++iteratorTractions;
							}

							createSkidmark((*iterator).midPoint(temp), *iteratorNormals, 
								(*iteratorDirections), ((*iteratorTractions) + tempTraction)/2.0f);	

							for(int b = 0; b < m; b++){
								--iteratorTractions;
								--iterator;
							}
							m--;
						}
						if(iterator != positions.end()){
							++iterator;
							++iteratorNormals;
							++iteratorDirections;
							++iteratorTractions;
						}
					}

					car->clearSkidmarkPositions(i);
					car->clearSkidmarkNormals(i);
					car->clearSkidmarkDirections(i);
					car->clearSkidmarkTractions(i);
				}
			}
		}
	}

	void SkidmarkView::createSkidmark(Rally::Vector3 position, Rally::Vector3 normal, Rally::Vector3 direction, float traction){
		Ogre::Billboard* b = skidmarkBillboards->createBillboard(Rally::Vector3(position.x, position.y+0.05f, position.z), 
			Ogre::ColourValue::Black);

		b->mDirection = normal;

		direction.y = 0;

		Ogre::Radian r = direction.angleBetween(Rally::Vector3(0, 0, 1));

		if(direction.z > 0 && direction.x > 0){
			r =  Ogre::Radian(Ogre::Math::PI*2) - r;
		}

		if(direction.z < 0 && direction.x > 0){
			r *= -1;
		}

		b->setRotation(r);
        
        double lengthAdjust = 0.07;
        float alphaAdjust = 28.0f;

        float trac = 1 - traction + 0.005;

        b->setDimensions(Ogre::Real(0.14), Ogre::Real(Ogre::Math::Clamp(lengthAdjust*car->getPhysicsCar().getVelocity().length(), 0.3, 0.7)));
        b->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f, Ogre::Math::Pow(trac, alphaAdjust)));

		if(skidmarkBillboards->getNumBillboards() > 1000)
			skidmarkBillboards->removeBillboard(skidmarkBillboards->getBillboard(skidmarkBillboards->getNumBillboards()-1000));
	}

} }
