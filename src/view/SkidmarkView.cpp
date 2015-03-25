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
		skidmarkBillboards->setDefaultDimensions(Ogre::Real(0.2), Ogre::Real(0.5));
		skidmarkBillboards->setAutoextend(true);

		skidmarkNode->attachObject(skidmarkBillboards);
	}

	void SkidmarkView::update(){
		std::list<Rally::Vector3> positions;
		std::list<Rally::Vector3> normals;
		std::list<Rally::Vector3> directions;
		std::list<Rally::Vector3>::iterator iterator;
		std::list<Rally::Vector3>::iterator iteratorNormals;
		std::list<Rally::Vector3>::iterator iteratorDirections;

		for(int i = 0; i < 4; i++){
			positions = car->getSkidmarkPositions(i);
			normals = car->getSkidmarkNormals(i);
			directions = car->getSkidmarkDirections(i);

			if(positions.size() > 0){
				iterator = positions.begin();
				iteratorNormals = normals.begin();
				iteratorDirections = directions.begin();

				if(positions.size() >= 2 && normals.size() >= 2 && directions.size() >= 2){
					createSkidmark(*iterator, *iteratorNormals,
						*iteratorDirections);
					for(int n = 0, size = positions.size(); n < size; n++){
						Rally::Vector3 temp = *iterator;
						Rally::Vector3 tempDir = *iterator;

						int m = positions.size()-n-1;
						if(m > 3)
							m = 3;
						while(m > 0){
							for(int a = 0; a < m; a++){
								++iterator;
							}

							createSkidmark((*iterator).midPoint(temp), *iteratorNormals,
								(*iteratorDirections));

							for(int b = 0; b < m; b++){
								--iterator;
							}
							m--;
						}
						if(iterator != positions.end()){
							++iterator;
							++iteratorNormals;
							++iteratorDirections;
						}
					}

					car->clearSkidmarkPositions(i);
					car->clearSkidmarkNormals(i);
					car->clearSkidmarkDirections(i);
				}
			}
		}
	}

	void SkidmarkView::createSkidmark(Rally::Vector3 position, Rally::Vector3 normal, Rally::Vector3 direction){
		Ogre::Billboard* b = skidmarkBillboards->createBillboard(Rally::Vector3(position.x, position.y+0.05, position.z),
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

		if(skidmarkBillboards->getNumBillboards() > 1000)
			skidmarkBillboards->removeBillboard(skidmarkBillboards->getBillboard(skidmarkBillboards->getNumBillboards()-1000));
	}

} }
