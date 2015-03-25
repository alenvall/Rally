#ifndef RALLY_MODEL_CHECKPOINT_H_
#define RALLY_MODEL_CHECKPOINT_H_

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "Rally.h"
#include "model/PhysicsWorld.h"

namespace Rally { namespace Model {

	class Checkpoint : public PhysicsWorld_StepCallback {

	public:
		Checkpoint(Rally::Model::PhysicsWorld& physicsWorld);
		virtual ~Checkpoint();

		void attachToWorld(const btVector3& position, const btVector3& shape);

		void checkCollision();
		virtual void processCollision(btCollisionObject* colObj);

		virtual void stepped(float deltaTime);

        Rally::Vector3 getPosition() const;
        Rally::Quaternion getOrientation() const;

		void setEnabled(bool enabled){
			this->enabled = enabled;
		}

		bool isEnabled(){
			return enabled;
		}

	private:
		void init(const btVector3& position, const btVector3& shape);

		Rally::Model::PhysicsWorld& physicsWorld;
		btPairCachingGhostObject* ghostObject;
		btDefaultMotionState* bodyMotionState;

		bool enabled;
	};

} }

#endif
