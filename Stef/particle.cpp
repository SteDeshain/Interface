#include <cassert>
#include "particle.h"

namespace stef
{

real Particle::sleepEpsilon = 0.01;
//real Particle::sleepEpsilon = 0.007;

void Particle::Integrate(real duration)
{
    if(!isAwake)
        return;

	//we don't integrate things with 0 mass.
	if(inverseMass == REAL_MAX || duration <= 0.0f)
        return;

    position.AddScaledVector(velocity, duration);

    Vector3 resultingAcc = acceleration;
    resultingAcc.AddScaledVector(forceAccum, inverseMass);

    velocity.AddScaledVector(resultingAcc, duration);

    //impose drag
    velocity *= real_pow(damping, duration);

    //clear the forces
    ClearAccumulator();

    if(canSleep)
    {
        real currentMotion = velocity * velocity;
        if(motion == 0)
            motion = 10 * Particle::sleepEpsilon;

        real bias = real_pow(0.5f, duration);
        motion = bias * motion + (1 - bias) * currentMotion;

        if(motion < Particle::sleepEpsilon)
            SetAwake(false);
        else if(motion > 10 * Particle::sleepEpsilon)
            motion = 10 * Particle::sleepEpsilon;
    }
}

void Particle::SetMass(const real mass)
{
    if(mass < 0 || mass == REAL_MAX)    //infinite mass
        inverseMass = 0;
    else if(mass == 0)  //zero mass
        this->inverseMass = REAL_MAX;
    else    //mass > 0
        this->inverseMass = ((real)1.0f)/mass;
}

real Particle::GetMass() const
{
    if (inverseMass == 0)   //infinite mass
    {
        return REAL_MAX;
    }
    else if(inverseMass == REAL_MAX)    //mass == 0
    {
        return 0;
    }
    else
    {
        return ((real)1.0)/inverseMass;
    }
}

void Particle::SetInverseMass(const real inverseMass)
{
    this->inverseMass = real_abs(inverseMass);
}

real Particle::GetInverseMass() const
{
    return inverseMass;
}

bool Particle::HasFiniteMass() const
{
    return inverseMass > 0.0f;
}

void Particle::SetDamping(const real damping)
{
    this->damping = damping;
}

real Particle::GetDamping() const
{
    return damping;
}

void Particle::SetElasticity(real elasticity)
{
    if(elasticity < 0)
        elasticity = 0;
    else if(elasticity > 1)
        elasticity = 1;

    this->elasticity = elasticity;
}

real Particle::GetElasticity()
{
    return elasticity;
}

void Particle::SetPosition(const Vector3& position)
{
    this->position = position;
}

void Particle::SetPosition(const real x, const real y, const real z)
{
    position.x = x;
    position.y = y;
    position.z = z;
}

void Particle::GetPosition(Vector3* position) const
{
    *position = this->position;
}

Vector3 Particle::GetPosition() const
{
    return position;
}

void Particle::SetLastPosition(const Vector3& lastPosition)
{
    this->lastPosition = lastPosition;
}

Vector3 Particle::GetLastPosition() const
{
    return lastPosition;
}

void Particle::SetVelocity(const Vector3& velocity)
{
    this->velocity = velocity;
}

void Particle::SetVelocity(const real x, const real y, const real z)
{
    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
}

void Particle::GetVelocity(Vector3* velocity) const
{
    *velocity = this->velocity;
}

Vector3 Particle::GetVelocity() const
{
    return velocity;
}

void Particle::SetAcceleration(const Vector3& acceleration)
{
    this->acceleration = acceleration;
}

void Particle::SetAcceleration(const real x, const real y, const real z)
{
    acceleration.x = x;
    acceleration.y = y;
    acceleration.z = z;
}

void Particle::GetAcceleration(Vector3* acceleration) const
{
    *acceleration = this->acceleration;
}

Vector3 Particle::GetAcceleration() const
{
    return acceleration;
}

void Particle::ClearAccumulator()
{
    forceAccum.Clear();
}

void Particle::AddForce(const Vector3& force)
{
    forceAccum += force;
    if(force != Vector3::ZERO)
        SetAwake(true);
}

void Particle::RestoreLastFrameValue()
{
    lastPosition = position;
}

void Particle::SetAwake(const bool awake)
{
    if(!canSleep && !awake)
        return;

    if(awake)
    {
        isAwake = true;
        //add a bit of motion to avoid it falling asleep immediately
        motion = Particle::sleepEpsilon * 2.0f;
    }
    else
    {
        isAwake = false;
        velocity.Clear();
    }
}

bool Particle::GetAwake()
{
    return isAwake;
}

void Particle::SetCanSleep(const bool canSleep)
{
    this->canSleep = canSleep;

    if(!canSleep && !isAwake)
        SetAwake(true);
}

}
