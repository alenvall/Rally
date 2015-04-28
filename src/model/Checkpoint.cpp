#include "model/Checkpoint.h"

namespace Rally { namespace Model {

	Checkpoint::Checkpoint(Rally::Model::PhysicsWorld& physicsWorld) :
		physicsWorld(physicsWorld){
	}

	Checkpoint::~Checkpoint(){
	}

	void Checkpoint::init(const btVector3& position, const btVector3& shape){
		ghostObject = new btPairCachingGhostObject();
		btCollisionShape* tempShape = new btBoxShape(shape);
		ghostObject->setCollisionShape(tempShape);
		ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		ghostObject->setWorldTransform(btTransform(btQuaternion(0.f, 0.f, 0.f, 1.f), position));

		physicsWorld.getDynamicsWorld()->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

		enabled = true;
	}

	void Checkpoint::attachToWorld(const btVector3& position, const btVector3& shape){
		init(position, shape);

		physicsWorld.getDynamicsWorld()->addCollisionObject(ghostObject);

        physicsWorld.registerStepCallback(this);
	}

	Rally::Vector3 Checkpoint::getPosition() const{
		btVector3 vec = ghostObject->getWorldTransform().getOrigin();
		return Rally::Vector3(vec.getX(), vec.getY(), vec.getZ());
	}

	Rally::Quaternion Checkpoint::getOrientation() const{
        const btQuaternion orientation = ghostObject->getWorldTransform().getRotation();
        return Rally::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z());
    }

	void Checkpoint::processCollision(btCollisionObject* colObj){

	}

	void Checkpoint::checkCollision(){
		btBroadphasePairArray& collisionPairs = ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
		const int	numObjects = collisionPairs.size();
		static btManifoldArray	m_manifoldArray;

		btCollisionObject* colObj = NULL;

		for(int i=0;i<numObjects;i++)	{
			m_manifoldArray.resize(0);
			const btBroadphasePair& cPair = collisionPairs[i];
			const btBroadphasePair* collisionPair = physicsWorld.getDynamicsWorld()->getPairCache()->findPair(cPair.m_pProxy0,cPair.m_pProxy1);
			if (!collisionPair) continue;
			if (collisionPair->m_algorithm) collisionPair->m_algorithm->getAllContactManifolds(m_manifoldArray);

			for (int j=0;j<m_manifoldArray.size();j++)	{
				btPersistentManifold* manifold = m_manifoldArray[j];

				for (int p=0,numContacts=manifold->getNumContacts();p<numContacts;p++){
					const btManifoldPoint&pt = manifold->getContactPoint(p);
					if (pt.getDistance() < 0.0) {
						colObj = (btCollisionObject*) (manifold->getBody0() == ghostObject ? manifold->getBody1() : manifold->getBody0());
						break;
					}
				}
				break;
			}
		}

		if(colObj != NULL){
			processCollision(colObj);
		}
	}

	void Checkpoint::stepped(float deltaTime) {
		checkCollision();
	}
} }
