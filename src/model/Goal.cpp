#include "model/Goal.h"

namespace Rally { namespace Model {

	void Goal::processCollision(btCollisionObject* colObj){
		Checkpoint::processCollision(colObj);

		std::cout << "GOAL COLLISION" << std::endl;
	}

} }