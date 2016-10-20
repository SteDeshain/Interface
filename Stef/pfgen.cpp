#include "pfgen.h"

namespace stef
{

void ParticleForceRegistry::UpdateForces(real duration)
{
    Registry::iterator i = registrations.begin();
    for (; i != registrations.end(); i++)
    {
        i->fg->UpdateForce(i->particle, duration);
    }
}

void ParticleForceRegistry::Add(Particle* particle, ParticleForceGenerator* fg)
{
	ParticleForceRegistration registration;
	registration.particle = particle;
	registration.fg = fg;
	registrations.push_back(registration);
}

ParticleSpring::ParticleSpring(Particle* other, real springConstant, real restLength)
    :other(other), springConstant(springConstant), restLength(restLength)
{
}

void ParticleSpring::UpdateForce(Particle* particle, real duration)
{
    // Calculate the vector of the spring
    Vector3 force;
    particle->GetPosition(&force);
    force -= other->GetPosition();

    // Calculate the magnitude of the force
    real magnitude = force.GetMagnitude();
    magnitude = real_abs(magnitude - restLength);
    magnitude *= springConstant;

    // Calculate the final force and apply it
    force.Normalize();
    force *= magnitude * -1;
    particle->AddForce(force);
}

ParticleFakeSpring::ParticleFakeSpring(Vector3 anchor, real springConstant, real damping)
    :anchor(anchor), springConstant(springConstant), damping(damping)
{
}

void ParticleFakeSpring::UpdateForce(Particle* particle, real duration)
{
    if(!particle->HasFiniteMass())
        return;

    Vector3 position;
    particle->GetPosition(&position);
    position -= anchor;

    real gamma = 0.5f * real_sqrt(4 * springConstant - damping * damping);
    if(gamma == 0.0f)
        return;
    Vector3 c = position * (damping / (2.0f * gamma)) + particle->GetVelocity() * (1.0f / gamma);

    // Calculate the target position
    Vector3 target = position * real_cos(gamma * duration) + c * real_sin(gamma * duration);
    target *= real_exp(-0.5f * duration * damping);

    // Calculate the resulting acceleration and therefore the force
    Vector3 accel = (target - position) * (1.0f / duration * duration) - particle->GetVelocity() * duration;
    particle->AddForce(accel * particle->GetMass());
}

ParticleAnchoredSpring::ParticleAnchoredSpring()
{
}

ParticleAnchoredSpring::ParticleAnchoredSpring(Vector3 anchor, real springConstant, real restLength)
    :anchor(anchor), springConstant(springConstant), restLength(restLength)
{
}

void ParticleAnchoredSpring::Init(Vector3 anchor, real springConstant, real restLength)
{
    this->anchor = anchor;
    this->springConstant = springConstant;
    this->restLength = restLength;
}

void ParticleAnchoredSpring::UpdateForce(Particle* particle, real duration)
{
    // Calculate the vector of the spring
    Vector3 force;
    particle->GetPosition(&force);
    force -= anchor;

    // Calculate the magnitude of the force
    real magnitude = force.GetMagnitude();
    magnitude = real_abs(magnitude - restLength);
    magnitude *= springConstant;

    // Calculate the final force and apply it
    force.Normalize();
    force *= magnitude * -1;
    particle->AddForce(force);
}

ParticleBungee::ParticleBungee(Particle* other, real springConstant, real restLength)
    :other(other), springConstant(springConstant), restLength(restLength)
{
}

void ParticleBungee::UpdateForce(Particle* particle, real duration)
{
    // Calculate the vector of the spring
    Vector3 force;
    particle->GetPosition(&force);
    force -= other->GetPosition();

    // Check if the bungee is compressed
    real magnitude = force.GetMagnitude();
    if(magnitude <= restLength)
        return;

    // Calculate the magnitude of the force
    magnitude = springConstant * (restLength - magnitude);

    // Calculate the final force and apply it
    force.Normalize();
    force *= magnitude * -1;
    particle->AddForce(force);
}

ParticleBuoyancy::ParticleBuoyancy(real maxDepth, real volume, real waterHeight, real liquidDensity)
    :maxDepth(maxDepth), volume(volume), waterHeight(waterHeight), liquidDensity(liquidDensity)
{
}

void ParticleBuoyancy::UpdateForce(Particle* particle, real duration)
{
    //calculate the submersion depth
    real depth = particle->GetPosition().z;

    //check if we are out of water
    if(depth >= waterHeight + maxDepth)
        return;
    Vector3 force(0, 0, 0);

    //check if we are at max depth
    if(depth <= waterHeight - maxDepth)
    {
        force.z = liquidDensity * volume;
        particle->AddForce(force);
        return;
    }

    //otherwise we are partly submerged
    force.z = liquidDensity * volume * (depth - maxDepth - waterHeight) / 2 * maxDepth;
    particle->AddForce(force);
}

}
