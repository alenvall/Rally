#include "model/Checkpoint.h"

namespace Rally { namespace Model {

	Checkpoint::Checkpoint(Rally::Model::PhysicsWorld& physicsWorld) :
		physicsWorld(physicsWorld){
	}

	Checkpoint::~Checkpoint(){
	}

	void Checkpoint::init(){
		ghostObject = new btPairCachingGhostObject();
		btCollisionShape* shape = new btBoxShape(btVector3(btScalar(2.),btScalar(2.),btScalar(2.)));
		ghostObject->setCollisionShape(shape);
		ghostObject->setWorldTransform(btTransform(btQuaternion(0,0,0,1),btVector3(0,5,0)));
		ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

		ghostObject->setWorldTransform(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(-50.0f, 2.f, 60.0f)));

		// Todo: move this
		physicsWorld.getDynamicsWorld()->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	}

	void Checkpoint::attachToWorld(){
		init();

		physicsWorld.getDynamicsWorld()->addCollisionObject(ghostObject);
	}

	void Checkpoint::processCollision(btCollisionObject* colObj){
		std::cout << "COLLISION" << std::endl;

		btRigidBody* b = btRigidBody::upcast(colObj);
		if(b)
			b->applyCentralImpulse(btVector3(0, 50, 0));
	}

	void Checkpoint::checkCollision(){
		btBroadphasePairArray& collisionPairs = ghostObject->getOverlappingPairCache()->getOverlappingPairArray();
		const int	numObjects=collisionPairs.size();	
		static btManifoldArray	m_manifoldArray;

		btCollisionObject* colObj = NULL;

		// TODO: restructure to only check for one object.
		// TODO(2): check so that the collisionObject is the car.
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

} }
