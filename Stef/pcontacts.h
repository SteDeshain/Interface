#ifndef __STEF_PCONTECTS_H__
#define __STEF_PCONTECTS_H__

#include "core.h"
#include "particle.h"

namespace stef
{

class ParticleContact
{

friend class ParticleContactResolver;

public:
    //the second particle can be NULL for contacts with the scenery
    Particle* particle[2];

    //restitution coefficient
    real restitution;

    //the direction of the contact
    Vector3 contactNormal;

    //the penetration
    real penetration;   //相交深度(渗透穿透)

    //the friction
//    real friction = 0.05f;  //default friction
    real friction = 0.5f;  //default friction

    bool microCollision = false;

    Vector3 particleMovement[2];

    static real velocityLimit;

protected:
    void Resolve(real duration);
    real CalculateSeparatingVelocity() const;

    //sleep
    void MatchAwakeState();

private:
    void ResolveVelocity(real duration);
    void ResolveInterpenetration(real duration);    //处理相交

};

class ParticleContactResolver
{

protected:
    //the number of iterations allowed
    unsigned iterations;

    //a performance tracking value
    //we keep a record of the actual number of iterations used
    unsigned iterationsUsed;

public:
    ParticleContactResolver(unsigned iterations);

    void SetIterations(unsigned iterations);

    void ResolveContacts(ParticleContact* contactArray, unsigned numContacts, real duration);

};

class ParticleContactGenerator
{

public:
    /**
    * Fills the given contact structure with the generated
    * contact. The contact pointer should point to the first
    * available contact in a contact array, where limit is the
    * maximum number of contacts in the array that can be written
    * to. The method returns the number of contacts that have
    * been written.
    */
    virtual unsigned AddContact(ParticleContact* contact, unsigned limit) const = 0;

};

}

#endif // __STEF_PCONTECTS_H__
