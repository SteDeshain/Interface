#ifndef __STEF_PFGEN_H__
#define __STEF_PFGEN_H__

#include <vector>
#include "core.h"
#include "particle.h"

namespace stef
{

class ParticleForceGenerator
{

public:
    virtual void UpdateForce(Particle* particle, real duration) = 0;

};

class ParticleForceRegistry
{

protected:
    struct ParticleForceRegistration
    {
        Particle* particle;
        ParticleForceGenerator* fg;
    };

    typedef std::vector<ParticleForceRegistration> Registry;
    Registry registrations;

public:
    void Add(Particle* particle, ParticleForceGenerator* fg);
    void Remove(Particle* particle, ParticleForceGenerator* fg);
    void Clear();

    void UpdateForces(real duration);

};

class ParticleSpring: public ParticleForceGenerator
{
    Particle* other;
    real springConstant;
    real restLength;

public:
    ParticleSpring(Particle* other, real springConstant, real restLength);

    virtual void UpdateForce(Particle* particle, real duration);
};

class ParticleFakeSpring: public ParticleForceGenerator
{
    Vector3 anchor;
    real springConstant;
    real damping;

public:
    ParticleFakeSpring(Vector3 anchor, real springConstant, real damping);

    virtual void UpdateForce(Particle* particle, real duration);
};

class ParticleAnchoredSpring: public ParticleForceGenerator
{
protected:
    Vector3 anchor;
    real springConstant;
    real restLength;

public:
    ParticleAnchoredSpring();
    ParticleAnchoredSpring(Vector3 anchor, real springConstant, real restLength);

    Vector3 GetAnchor() const
    {
        return anchor;
    }
    void Init(Vector3 anchor, real springConstant, real restLength);

    virtual void UpdateForce(Particle* particle, real duration);
};

class ParticleBungee: public ParticleForceGenerator
{
    Particle* other;
    real springConstant;
    real restLength;

public:
    ParticleBungee(Particle* other, real springConstant, real restLength);

    virtual void UpdateForce(Particle* particle, real duration);
};

class ParticleBuoyancy: public ParticleForceGenerator
{
    real maxDepth;
    real volume;
    real waterHeight;
    real liquidDensity;

public:
    ParticleBuoyancy(real maxDepth, real volume, real waterHeight,
                     real liquidDensity = 1000.0f);

    virtual void UpdateForce(Particle* particle, real duration);
};

}

#endif // __STEF_PFGEN_H__
