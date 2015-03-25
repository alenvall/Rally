#include "model/PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>
#include <../Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>

#include <stdexcept>
#include <sstream>
#include <iostream>

namespace Rally { namespace Model {

    namespace {
        void preStepCallbackProxy(btDynamicsWorld* dynamicsWorld, btScalar deltaTime) {
            PhysicsWorld* physicsWorld = static_cast<PhysicsWorld*>(dynamicsWorld->getWorldUserInfo());
            physicsWorld->invokeStepCallbacks(deltaTime, true);
        }

        void postStepCallbackProxy(btDynamicsWorld* dynamicsWorld, btScalar deltaTime) {
            PhysicsWorld* physicsWorld = static_cast<PhysicsWorld*>(dynamicsWorld->getWorldUserInfo());
            physicsWorld->invokeStepCallbacks(deltaTime, false);
        }
    }

    PhysicsWorld::PhysicsWorld() :
			fileLoader(NULL),
			dynamicsWorld(NULL),
			solver(NULL),
			dispatcher(NULL),
			collisionConfiguration(NULL),
			broadphase(NULL),
			gravityGlitchActive(false) {
        gravityGlitchTimer.reset();
    }

    PhysicsWorld::~PhysicsWorld() {
        delete fileLoader;
        delete dynamicsWorld;
        delete solver;
        delete dispatcher;
        delete collisionConfiguration;
        delete broadphase;
    }

    void PhysicsWorld::initialize(const std::string & bulletFile) {
        // Create physics world
        broadphase = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        solver = new btSequentialImpulseConstraintSolver;
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

        /*dynamicsWorld->addRigidBody(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(
            0.f, // mass
            new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1.f),
                btVector3(0,-3.0f,0))), // position
            new btBoxShape(btVector3(5000.f, 3.f, 5000.f)),
            btVector3(0,0,0)))); // inertia (0 for 0-mass objects)
/**/
        // Import world file
        fileLoader = new btBulletWorldImporter(dynamicsWorld);
        // Optionally enable the verbose mode to provide debugging information during file loading
        // (a lot of data is generated, so this option is very slow)
        //fileLoader->setVerboseMode(true);
        fileLoader->loadFile(bulletFile.c_str());

        int bodyCount = fileLoader->getNumRigidBodies();
        if(bodyCount <= 0) {
            throw std::runtime_error("Failed to load bullet world definition file.");
        }
        // std::cout << "Number of rigid bodied: " << bodyCount << std::endl;*/

        dynamicsWorld->setInternalTickCallback(preStepCallbackProxy, static_cast<void*>(this), true);
        dynamicsWorld->setInternalTickCallback(postStepCallbackProxy, static_cast<void*>(this), false);

        dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));
        gravityGlitchTimer.reset();
    }

    void PhysicsWorld::update(float deltaTime) {
        dynamicsWorld->stepSimulation(deltaTime, 12, 1.0f/60.0f);
    }

    void PhysicsWorld::registerStepCallback(PhysicsWorld_StepCallback* stepCallback) {
        stepCallbacks.push_back(stepCallback);
    }

    void PhysicsWorld::unregisterStepCallback(PhysicsWorld_StepCallback* stepCallback) {
        for(std::vector<PhysicsWorld_StepCallback*>::iterator callbackIterator = stepCallbacks.begin();
                callbackIterator != stepCallbacks.end();
                ++callbackIterator) {
            if(*callbackIterator == stepCallback) {
                stepCallbacks.erase(callbackIterator);
                return;
            }
        }
    }

    void PhysicsWorld::invokeStepCallbacks(float deltaTime, bool isPre) {
        if(isPre) {
            gravityGlitchDriver();
        }

        for(std::vector<PhysicsWorld_StepCallback*>::iterator callbackIterator = stepCallbacks.begin();
                callbackIterator != stepCallbacks.end();
                ++callbackIterator) {
            if(isPre) {
                (*callbackIterator)->willStep(deltaTime);
            } else {
                (*callbackIterator)->stepped(deltaTime);
            }
        }
    }

    void PhysicsWorld::gravityGlitch() {
        if(!gravityGlitchActive) {
            gravityGlitchTimer.reset();
            gravityGlitchActive = true;
        }
    }

    void PhysicsWorld::gravityGlitchDriver() {
        if(gravityGlitchActive) {
            float step = gravityGlitchTimer.getElapsedSeconds();
            if(step < 0.6f) {
                dynamicsWorld->setGravity(btVector3(3.0f * btSin(step), 4.0f * btCos(step), 5.0f * btCos(step)));
            } else {
                dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));
                gravityGlitchActive = false;
            }
        }
    }

} }
