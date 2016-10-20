#ifndef __STEF_PWORLD_H__
#define __STEF_PWORLD_H__

#include "particle.h"
#include "pfgen.h"
#include "pcontacts.h"
#include "box.h"
#include <list>

namespace stef
{

class ParticleWorld
{

public:
    typedef std::list<Particle*> Particles;
    typedef std::list<ParticleContactGenerator*> ContactGenerators;

private:

    //True if the world should calculate the number of iterations
    //to give the contact resolver at each frame.
    bool calculateIterations;

    //hold the list of particles
    Particles particles;

    ParticleForceRegistry pfRegistry;

    ParticleContactResolver pcResolver;

    ContactGenerators contactGenerators;

    //hold the contacts list
    //a piece of memory ready to hold the contacts may appearing in the game
    ParticleContact* contacts;
    unsigned maxContacts;   //the size of the contacts array

public:
    ParticleWorld(unsigned maxContacts, unsigned iterations = 0);
    ~ParticleWorld();

    //must be called once before RunPhysics()
    //to remove forceAccum for every particle and to do anything necessary
    void StartFrame();

    //calls each of the registered contact generator to report their contacts
    //return the number of generated contacts
    unsigned GenerateContcats();

    //integrates all the particles
    void Integrate(real duration);

    //core update
    void RunPhysics(real duration);

    Particles& GetParticles();
    ContactGenerators& GetContactGenerators();
    ParticleForceRegistry& GetForceRegistry();

};

class GroundContacts: public ParticleContactGenerator
{

protected:
    ParticleWorld::Particles* particles;
    real horizon;
    real elasticity;

public:
    GroundContacts(real horizon = 0.0f);

    void Init(ParticleWorld::Particles* particles);
    void SetHorizon(real horizon);
    void SetElasticity(real elasticity);

    unsigned virtual AddContact(ParticleContact* contact, unsigned limit) const;

};

class GroundContactsWithBox: public GroundContacts
{

public:
    GroundContactsWithBox(real horizon = 0.0f);

    unsigned virtual AddContact(ParticleContact* contact, unsigned limit) const;

};

class SceneWallsContacts: public ParticleContactGenerator
{

protected:
    ParticleWorld::Particles* particles;
    real leftWall, rightWall, backWall, frontWall;
    real elasticity;

public:
    SceneWallsContacts(real leftWall = 0, real rightWall = 0, real backWall = 0, real frontWall = 0);

    void Init(ParticleWorld::Particles* particles);
    void SetWalls(real leftWall, real rightWall, real backWall, real frontWall);
    void SetElasticity(real elasticity);

    unsigned virtual AddContact(ParticleContact* contact, unsigned limit) const;

};

class SceneWallsContactsWithBox: public SceneWallsContacts
{

public:
    SceneWallsContactsWithBox(real leftWall = 0, real rightWall = 0, real backWall = 0, real frontWall = 0);

    unsigned virtual AddContact(ParticleContact* contact, unsigned limit) const;

};

}

#endif // __STEF_PWORLD_H__
