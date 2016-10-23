#include "pcontacts.h"

namespace stef
{

//real ParticleContact::velocityLimit = 0.2f;   //200fps
real ParticleContact::velocityLimit = 0.4f;    //100fps

void ParticleContact::Resolve(real duration)
{
    ResolveVelocity(duration);
    ResolveInterpenetration(duration);
}

real ParticleContact::CalculateSeparatingVelocity() const
{
    Vector3 relativeVelocity = particle[0]->GetVelocity();
    if(particle[1])
    {
        relativeVelocity -= particle[1]->GetVelocity();
    }

    return relativeVelocity * contactNormal;
}

void ParticleContact::ResolveVelocity(real duration)
{
    real separatingVelocity = CalculateSeparatingVelocity();

    real appliedRestitution = restitution;
    if(real_abs(separatingVelocity) < ParticleContact::velocityLimit)
    {
        appliedRestitution = 0;
    }

    //check whether it needs to be resolved
    if(separatingVelocity > 0)
    {
        //the contact is either separating or stationary
        //there is no impulse required
        return;
    }

    //separating velocity after contact/collision
    real newSepVelocity = separatingVelocity * appliedRestitution * -1;

    //check the velocity built-up due to acceleration(gravity) only
    Vector3 accCausedVelocity = particle[0]->GetAcceleration();
    if(particle[1])
        accCausedVelocity -= particle[1]->GetAcceleration();
    real accCausedSepVelocity = accCausedVelocity * contactNormal * duration;

    //if we've got a closing velocity due to acceleration built-up
    //remove it from the new separating velocity
    if(accCausedSepVelocity < 0)
    {
        newSepVelocity += appliedRestitution * accCausedSepVelocity;
        if(newSepVelocity < 0)
            newSepVelocity = 0;
    }

    //the first component of the equition
    real deltaVelocity = newSepVelocity - separatingVelocity;

    real totalInverseMass = particle[0]->GetInverseMass();
    if(particle[1])
        totalInverseMass += particle[1]->GetInverseMass();

    //if both particles have infinite mass, then impulses have no effect
    if(totalInverseMass <= 0)
        return;

    //calculate the impulse to apply
    real impulse = deltaVelocity / totalInverseMass;

    //find the amount of impulse per unit of inverse mass
    Vector3 impulsePerInvMass = contactNormal * impulse;

    //apply impulses
    particle[0]->SetVelocity(particle[0]->GetVelocity() +
                             impulsePerInvMass * particle[0]->GetInverseMass());
    if(particle[1])
    {
        particle[1]->SetVelocity(particle[1]->GetVelocity() +
                                 impulsePerInvMass * particle[1]->GetInverseMass() * -1);
    }

    microCollision = false;
    //check whether there is a friction
    //it's a micro collision, no need to awake either particle
    if(newSepVelocity == 0)
    {
        //in this physics engine, only platform can product friction
        if(contactNormal == Vector3::TOP)
        {
            microCollision = true;

            Vector3 planarVelocity(particle[0]->GetVelocity().x, particle[0]->GetVelocity().y, 0);
            if(particle[1])
            {
                planarVelocity -= Vector3(particle[1]->GetVelocity().x, particle[1]->GetVelocity().y, 0);
            }
            if(planarVelocity == Vector3::ZERO)
                return;

            Vector3 frictionImpulse = planarVelocity * -1;
            frictionImpulse.Normalize();
            frictionImpulse *= impulse * friction;

            Vector3 originVelocityDirection = particle[0]->GetVelocity();

            particle[0]->SetVelocity(particle[0]->GetVelocity() +
                                     frictionImpulse * particle[0]->GetInverseMass());

            Vector3 afterVelocityDirection = particle[0]->GetVelocity();
            if(!(afterVelocityDirection.AtOneWay(originVelocityDirection)))
                particle[0]->SetVelocity(Vector3::ZERO);

            if(particle[1])
            {
                originVelocityDirection = particle[1]->GetVelocity();

                particle[1]->SetVelocity(particle[1]->GetVelocity() +
                                         frictionImpulse * particle[1]->GetInverseMass());

                afterVelocityDirection = particle[1]->GetVelocity();
                if(!(afterVelocityDirection.AtOneWay(originVelocityDirection)))
                    particle[1]->SetVelocity(Vector3::ZERO);
            }
        }
    }
    else
    {
        MatchAwakeState();
    }
}

void ParticleContact::ResolveInterpenetration(real duration)
{
    if(penetration <= 0)
        return;

    real totalInverseMass = particle[0]->GetInverseMass();
    if(particle[1])
        totalInverseMass += particle[1]->GetInverseMass();

    if(totalInverseMass <= 0)
        return;

    if(microCollision)
    {
#if 1
        Vector3 totalMove = contactNormal * penetration;
        if(particle[1])
        {
            if(particle[0]->GetPosition().z < particle[1]->GetPosition().z)
            {
                particleMovement[0].Clear();
                particleMovement[1] = totalMove * -1;
            }
            else
            {
                particleMovement[1].Clear();
                particleMovement[0] = totalMove;
            }
        }
        else
        {
            particleMovement[1].Clear();
            particleMovement[0] = totalMove;
        }
#else
        Vector3 movePerInvMass = contactNormal * (penetration / totalInverseMass);

        particleMovement[0] = movePerInvMass * particle[0]->GetInverseMass();
        if(particle[1])
        {
            particleMovement[1] = movePerInvMass * particle[1]->GetInverseMass() * -1;
        }
        else
        {
            particleMovement[1].Clear();
        }
#endif // 0
    }
    else
    {
        Vector3 movePerInvMass = contactNormal * (penetration / totalInverseMass);

        particleMovement[0] = movePerInvMass * particle[0]->GetInverseMass();
        if(particle[1])
        {
            particleMovement[1] = movePerInvMass * particle[1]->GetInverseMass() * -1;
        }
        else
        {
            particleMovement[1].Clear();
        }
    }

    particle[0]->SetPosition(particle[0]->GetPosition() + particleMovement[0]);
    if(particle[1])
    {
        particle[1]->SetPosition(particle[1]->GetPosition() + particleMovement[1]);
    }
}

void ParticleContact::MatchAwakeState()
{
    //collision with the world never cause a body to wake up
    if(!particle[1])
        return;

    bool particle0Awake = particle[0]->GetAwake();
    bool particle1Awake = particle[1]->GetAwake();

    //wake up only the sleeping one
    if(particle0Awake ^ particle1Awake)
    {
        if(particle0Awake)
            particle[1]->SetAwake(true);
        else
            particle[0]->SetAwake(true);
    }
}

ParticleContactResolver::ParticleContactResolver(unsigned iterations)
    :iterations(iterations)
{
}

void ParticleContactResolver::SetIterations(unsigned iterations)
{
    this->iterations = iterations;
}

void ParticleContactResolver::ResolveContacts(ParticleContact* contactArray, unsigned numContacts, real duration)
{
    iterationsUsed = 0;
    while(iterationsUsed < iterations)
    {
        //find the contact with the largest closing velocity
        real maxClsVel = 0;
        unsigned maxIndex = numContacts - 1;
        for(unsigned i = 0; i < numContacts; i++)
        {
            real sepVel = contactArray[i].CalculateSeparatingVelocity();
            if(sepVel < maxClsVel)
            {
                maxClsVel = sepVel;
                maxIndex = i;
            }
        }
        //resolve this contact
        contactArray[maxIndex].Resolve(duration);

        //update the inter-penetrations for all particles
        Vector3* mov = contactArray[maxIndex].particleMovement;
		for (int i = 0; i < numContacts; i++)
		{
			if (contactArray[i].particle[0] == contactArray[maxIndex].particle[0])
			{
				contactArray[i].penetration -= mov[0] * contactArray[i].contactNormal;
			}
			else if (contactArray[i].particle[0] == contactArray[maxIndex].particle[1])
			{
				contactArray[i].penetration -= mov[1] * contactArray[i].contactNormal;
			}
			if (contactArray[i].particle[1])
			{
				if (contactArray[i].particle[1] == contactArray[maxIndex].particle[0])
				{
					contactArray[i].penetration += mov[0] * contactArray[i].contactNormal;
				}
				else if (contactArray[i].particle[1] == contactArray[maxIndex].particle[1])
				{
					contactArray[i].penetration += mov[1] * contactArray[i].contactNormal;
				}
			}
		}

        iterationsUsed++;
    }
}

}
