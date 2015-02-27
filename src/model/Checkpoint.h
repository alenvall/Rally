#ifndef RALLY_MODEL_CHECKPOINT_H_
#define RALLY_MODEL_CHECKPOINT_H_

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "Rally.h"
#include "model/PhysicsWorld.h"

namespace Rally { namespace Model {


	class Checkpoint {

	public:
		Checkpoint(Rally::Model::PhysicsWorld& physicsWorld);
		virtual ~Checkpoint();
		void Checkpoint::attachToWorld();

		void checkCollision();
		virtual void processCollision(btCollisionObject* colObj);

	private:
		void Checkpoint::init();

		Rally::Model::PhysicsWorld& physicsWorld;
		btPairCachingGhostObject* ghostObject;
		btDefaultMotionState* bodyMotionState;
	};

} }

#endif