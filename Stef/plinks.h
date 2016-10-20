#ifndef __STEF_PLINKS_H__
#define __STEF_PLINKS_H__

#include "core.h"
#include "particle.h"
#include "pcontacts.h"

namespace stef
{

class ParticleLink
{

public:
    //hold the pair of particles that are connected by this link
    Particle* particle[2];

protected:
    real GetCurrentLength() const;

public:
    //Fills the given contact structure with the contact needed to keep
    //the link from violating its constraint. The contact pointer should
    //point to the first available contact in a contact array, where limit
    //is the maximum number of contacts in the array that can be written to.
    //The method returns the number of contacts that have been written.
    //This format is common to contact-generating functions, but this class
    //can only generate one single contact, so the pointer can be a pointer to
    //a single element. The limit parameter is assumed to be at least one (zero
    //isn't valid), and the return is either 0 or 1
    virtual unsigned FillContact(ParticleContact* contact, unsigned limit) const = 0;

};

class ParticleCable: public ParticleLink    //绳索
{

public:
    real maxLength;
    real restitution;   //bounciness of the cable

public:
    virtual unsigned FillContact(ParticleContact* contact, unsigned limit) const;

};

class ParticleRod: public ParticleLink      //连杆
{

public:
    real length;    //the length of the rod

public:
//    real GetCurrentLength() const;

    virtual unsigned FillContact(ParticleContact* contact, unsigned limit) const;

};

}

#endif // __STEF_PLINKS_H__
