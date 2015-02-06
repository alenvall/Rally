#ifndef RALLY_MODEL_CAR_H_
#define RALLY_MODEL_CAR_H_

// Todo: Move this to shared file.
namespace Rally {
    typedef Vector3 Ogre::Vector3;
}

namespace Rally { namespace Model {

    class Car {
        public:
            Car();
            virtual ~Car();

            Rally::Vector3 getPosition() const;
            Rally::Vector3 getOrientation() const;
            Rally::Vector3 getVelocity() const;
    };

} }

#endif // RALLY_MODEL_CAR_H_
