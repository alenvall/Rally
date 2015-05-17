#ifndef RALLY_MODEL_WORLD_H_
#define RALLY_MODEL_WORLD_H_

#include "model/PhysicsWorld.h"
#include "model/Car.h"
#include "model/RemoteCar.h"
#include "model/Finish.h"
#include <map>
#include "util/Timer.h"

namespace Rally { namespace Model {

    class World {
        public:
            World();
            virtual ~World();
            void initialize(const std::string & bulletFile);
            void update(float deltaTime);

            Rally::Model::PhysicsWorld& getPhysicsWorld() {
                return physicsWorld;
            }

            Rally::Model::Car& getPlayerCar() {
                return playerCar;
            }

            Rally::Model::RemoteCar& getRemoteCar(int carId) {
                std::map<int, Rally::Model::RemoteCar>::iterator found = remoteCars.find(carId);

                // Lazily construct if not found
                if(found == remoteCars.end()) {
                    found = remoteCars.insert(std::map<int, Rally::Model::RemoteCar>::value_type(carId,
                        Rally::Model::RemoteCar(physicsWorld))).first;
                    found->second.attachToWorld();
                }

                return found->second;
            }

            bool removeRemoteCar(int carId) {
                return remoteCars.erase(carId) > 0;
            }
			
            const std::map<int, Rally::Model::RemoteCar>& getRemoteCars() const {
                return remoteCars;
            }

			Rally::Model::Checkpoint& getFinish() {
                return finish;
            }

			Rally::Model::Checkpoint& getStart() {
                return start;
            }

			void printFinishedTime();

			/*const std::map<int, Rally::Model::Checkpoint>& getCheckPoints() const {
                return checkPoints;
            }*/
			
			std::list<Rally::Vector3> positions;
		
            void gravityGlitch() {
                physicsWorld.gravityGlitch();
            }

        private:
            Rally::Model::PhysicsWorld physicsWorld;
            Rally::Model::Car playerCar;
			Rally::Model::Finish finish;
			Rally::Model::Finish start;
            Rally::Model::Finish tunnelTeleport;
            //std::map<int, Rally::Model::Checkpoint> checkPoints;
            std::map<int, Rally::Model::RemoteCar> remoteCars;
			Rally::Util::Timer finishTimer;

    };

} }

#endif // RALLY_MODEL_WORLD_H_
