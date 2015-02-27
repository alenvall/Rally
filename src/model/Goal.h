#include "model/Checkpoint.h"

#include "Rally.h"
#include "model\PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>

namespace Rally { namespace Model {

	class Goal : public Checkpoint {

	public:
		Goal(Rally::Model::PhysicsWorld& physicsWorld) : Checkpoint(physicsWorld){};

		virtual void processCollision(btCollisionObject* colObj);

		//private:
	};

} }