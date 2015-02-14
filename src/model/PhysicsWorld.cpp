#include "model/PhysicsWorld.h"

#include <btBulletDynamicsCommon.h>
#include <../Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>

#include <sstream>
#include <iostream>

namespace Rally { namespace Model {

    PhysicsWorld::PhysicsWorld() {
    }

    PhysicsWorld::~PhysicsWorld() {
        delete fileLoader;
        delete dynamicsWorld;
        delete solver;
        delete dispatcher,
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

        // Import world file
        fileLoader = new btBulletWorldImporter(dynamicsWorld);
        // Optionally enable the verbose mode to provide debugging information during file loading
        // (a lot of data is generated, so this option is very slow)
        //fileLoader->setVerboseMode(true);
        fileLoader->loadFile(bulletFile.c_str());

        // Debug: Output the amount of loaded rigid bodies
        int bodyCount = fileLoader->getNumRigidBodies();
        std::cout << "Number of rigid bodied: " << bodyCount << std::endl;
    }

    void PhysicsWorld::update(float deltaTime) {
        dynamicsWorld->stepSimulation(deltaTime, 12, 1.0/60.0);
    }
} }
