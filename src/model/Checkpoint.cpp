#include "Checkpoint.h"

namespace Rally { namespace Model {
	
	Checkpoint::Checkpoint(btDynamicsWorld& dynamicsWorld) : 
		dynamicsWorld(dynamicsWorld){

		ghostObject = new btGhostObject();
		btCollisionShape* shape = new btBoxShape(btVector3(btScalar(5.),btScalar(5.),btScalar(5.)));
		ghostObject->setCollisionShape(shape);
		ghostObject->setWorldTransform(btTransform(btQuaternion(0,0,0,1),btVector3(0,5,0)));
		ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

		// Add to world
		dynamicsWorld.addCollisionObject(ghostObject);
		//dynamicsWorld->addCollisionObject(ghostObject,btBroadphaseProxy::SensorTrigger,
			//btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger);
	}

	Checkpoint::~Checkpoint(){
		delete ghostObject->getCollisionShape();
		delete ghostObject;
	}

} }
/*
	if (m_dynamicsWorld)
	{
		btAlignedObjectArray < btCollisionObject* > objsInsidePairCachingGhostObject;	// We might want this to be a member variable...							
		btAlignedObjectArray < btCollisionObject* >* pObjsInsideGhostObject = NULL;		// We will store a reference of the current array in this pointer
		const btAlignedObjectArray < btCollisionObject* >& objs = m_dynamicsWorld->getCollisionObjectArray();
		for (int i=0,sz=objs.size();i<sz;i++)	{
			btCollisionObject* o = objs[i];
			btGhostObject* go = btGhostObject::upcast(o);
			if (go)	{
				objsInsidePairCachingGhostObject.resize(0);
				btPairCachingGhostObject* pgo = dynamic_cast < btPairCachingGhostObject* > (go);	// No upcast functionality...
				if (pgo)	{
					GetCollidingObjectsInsidePairCachingGhostObject(static_cast < btDiscreteDynamicsWorld* > (m_dynamicsWorld),pgo,objsInsidePairCachingGhostObject);
					pObjsInsideGhostObject = &objsInsidePairCachingGhostObject;
				}
				else {
					pObjsInsideGhostObject = &go->getOverlappingPairs();	// It's better not to try and copy the whole array, but to keep a reference to it!
				}	
				// Here pObjsInsideGhostObject should be valid.
				
				ProcessObectsInsideGhostObjects(*pObjsInsideGhostObject,pgo);
			}
		
		}	
	}
*/

/*
btBroadphasePairArray& collisionPairs = ghostObject->getOverlappingPairCache()->getOverlappingPairArray();   //New
     const int   numObjects=collisionPairs.size();   
   static btManifoldArray   m_manifoldArray;
   bool added;
   for(int i=0;i<numObjects;i++)   {
       const btBroadphasePair& collisionPair = collisionPairs[i];
      m_manifoldArray.resize(0);
      if (collisionPair.m_algorithm) collisionPair.m_algorithm->getAllContactManifolds(m_manifoldArray);
      else printf("No collisionPair.m_algorithm - probably m_dynamicsWorld->getDispatcher()->dispatchAllCollisionPairs(...) must be missing.\n");
      added = false;
      for (int j=0;j<m_manifoldArray.size();j++)   {
         btPersistentManifold* manifold = m_manifoldArray[j];
         // Here we are in the narrowphase, but can happen that manifold->getNumContacts()==0:
         if (m_processOnlyObjectsWithNegativePenetrationDistance)   {
            for (int p=0;p<manifold->getNumContacts();p++)         {
               const btManifoldPoint&pt = manifold->getContactPoint(p);
               if (pt.getDistance() < 0.0)    {
                  // How can I be sure that the colObjs are all distinct ? I use the "added" flag.
                  m_objectsInFrustum.push_back((btCollisionObject*) (manifold->getBody0() == m_ghostObject ? manifold->getBody1() : manifold->getBody0()));
                  added = true;
                  break;
               }   
            }
         if (added) break;
         }
         else if (manifold->getNumContacts()>0) {
            m_objectsInFrustum.push_back((btCollisionObject*) (manifold->getBody0() == m_ghostObject ? manifold->getBody1() : manifold->getBody0()));
            break;
         }
      }
   }   
}
*/