#include "model/Finish.h"

namespace Rally { namespace Model {

	void Finish::processCollision(btCollisionObject* colObj){
		finished = true;
	}

} }