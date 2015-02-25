#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "Rally.h"

namespace Rally { namespace Model {

    
    class Checkpoint {
        public:
			Checkpoint(btDynamicsWorld& dynamicsWorld);
            virtual ~Checkpoint();

        private:
			btGhostObject* ghostObject;
            btDynamicsWorld& dynamicsWorld;
	};

} }