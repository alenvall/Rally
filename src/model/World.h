#ifndef RALLY_MODEL_WORLD_H_
#define RALLY_MODEL_WORLD_H_

#include "model/PhysicsWorld.h"
#include "model/Car.h"
#include "model/RemoteCar.h"
#include "model/Goal.h"
#include <map>

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

			Rally::Model::Checkpoint& getGoal() {
                return goal;
            }

			/*const std::map<int, Rally::Model::Checkpoint>& getCheckPoints() const {
                return checkPoints;
            }*/

        private:
            Rally::Model::PhysicsWorld physicsWorld;
            Rally::Model::Car playerCar;
			Rally::Model::Goal goal;
            //std::map<int, Rally::Model::Checkpoint> checkPoints;
            std::map<int, Rally::Model::RemoteCar> remoteCars;
    };

} }

#endif // RALLY_MODEL_WORLD_H_
