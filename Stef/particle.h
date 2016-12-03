#ifndef __STEF_PARTICLE_H__
#define __STEF_PARTICLE_H__

#include "core.h"

namespace stef
{

class Particle
{

public:
    enum ParticleMode
    {
        Dynamic,
        KeyFramed,
    };

protected:
    Vector3 position;
    Vector3 velocity;
    Vector3 forceAccum;
    Vector3 acceleration;

    Vector3 lastPosition = Vector3::ZERO;

    real damping;
    real inverseMass;

    real elasticity;    //bounciness: 0 for absolute soft, 1 for no bounciness

    //sleep
    bool isAwake = true;
    bool canSleep = true;
    real motion = 0;

    static real sleepEpsilon;

public:
    void Integrate(real duration);

    void SetMass(const real mass);
    real GetMass() const;

    void SetInverseMass(const real inverseMass);
    real GetInverseMass() const;

    bool HasFiniteMass() const;

    void SetDamping(const real damping);
    real GetDamping() const;

    void SetElasticity(real elasticity);
    real GetElasticity();

    void SetPosition(const Vector3& position);
    void SetPosition(const real x, const real y, const real z);
    void GetPosition(Vector3* position) const;
    Vector3 GetPosition() const;

    void SetLastPosition(const Vector3& lastPosition);
    Vector3 GetLastPosition() const;

    void SetVelocity(const Vector3& velocity);
    void SetVelocity(const real x, const real y, const real z);
    void GetVelocity(Vector3* velocity) const;
    Vector3 GetVelocity() const;

    void SetAcceleration(const Vector3& acceleration);
    void SetAcceleration(const real x, const real y, const real z);
    void GetAcceleration(Vector3* acceleration) const;
    Vector3 GetAcceleration() const;

    void ClearAccumulator();

    void AddForce(const Vector3& force);

    virtual void RestoreLastFrameValue();

    //sleep
    void SetAwake(const bool awake);
    bool GetAwake();
    void SetCanSleep(const bool canSleep);
    bool GetCanSleep();
    //temp
    real GetMotion()
    {
        return motion;
    }

};

}

#endif // __STEF_PARTICLE_H__
