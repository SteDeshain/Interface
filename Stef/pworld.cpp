#include "pworld.h"

namespace stef
{

ParticleWorld::ParticleWorld(unsigned maxContacts, unsigned iterations)
    :pcResolver(iterations), maxContacts(maxContacts)
{
    contacts = new ParticleContact[maxContacts];
    calculateIterations = (iterations == 0);
}

ParticleWorld::~ParticleWorld()
{
    delete[] contacts;
}

void ParticleWorld::StartFrame()
{
    for(Particles::iterator p = particles.begin();
        p != particles.end();
        p++)
    {
        (*p)->ClearAccumulator();
    }
}

unsigned ParticleWorld::GenerateContcats()
{
    unsigned limit = maxContacts;
    ParticleContact* nextContact = contacts;

    for(ContactGenerators::iterator g = contactGenerators.begin();
        g != contactGenerators.end();
        g++)
    {
        unsigned used = (*g)->AddContact(nextContact, limit);   //limit cannot be zero

        limit -= used;
        nextContact += used;

        if(limit <= 0)
            break;
    }

    //return the number of the contacts used
    return maxContacts - limit;
}

void ParticleWorld::Integrate(real duration)
{
    for(Particles::iterator p = particles.begin();
        p != particles.end();
        p++)
    {
        (*p)->Integrate(duration);
    }
}

void ParticleWorld::RunPhysics(real duration)
{
    if(!duration)
        return;

    //first apply the force generations
    pfRegistry.UpdateForces(duration);

    //then integrate everything
    Integrate(duration);

    //generate contacts
    unsigned usedContcats = GenerateContcats();
    //and process them
    if(usedContcats)
    {
        if(calculateIterations)
            pcResolver.SetIterations(usedContcats * 2); //why double?
        pcResolver.ResolveContacts(contacts, usedContcats, duration);
    }

    //update lastPosition for every particle
    for(Particles::iterator p = particles.begin(); p != particles.end(); p++)
    {
        (*p)->RestoreLastFrameValue();
    }
}

ParticleWorld::Particles& ParticleWorld::GetParticles()
{
    return particles;
}

ParticleWorld::ContactGenerators& ParticleWorld::GetContactGenerators()
{
    return contactGenerators;
}

ParticleForceRegistry& ParticleWorld::GetForceRegistry()
{
    return pfRegistry;
}

GroundContacts::GroundContacts(real horizon)
    :horizon(horizon), elasticity(0.2f)
{
}

void GroundContacts::Init(ParticleWorld::Particles* particles)
{
    this->particles = particles;
}

void GroundContacts::SetHorizon(real horizon)
{
    this->horizon = horizon;
}

void GroundContacts::SetElasticity(real elasticity)
{
    this->elasticity = elasticity;
}

unsigned GroundContacts::AddContact(ParticleContact* contact, unsigned limit) const
{
	unsigned ct = 0;
	for(ParticleWorld::Particles::iterator p = particles->begin();
		p != particles->end();
		p++)
	{
		real z = (*p)->GetPosition().z;
		if(z < horizon)
		{
			contact->contactNormal = Vector3(0, 0, 1);
			contact->particle[0] = *p;
			contact->particle[1] = NULL;
			contact->penetration = horizon - z;
			contact->restitution = elasticity;
			contact++;
			ct++;
		}

		if(ct >= limit)
            return ct;
	}
	return ct;
}

GroundContactsWithBox::GroundContactsWithBox(real horizon)
    :GroundContacts(horizon)
{
}

unsigned GroundContactsWithBox::AddContact(ParticleContact* contact, unsigned limit) const
{
	unsigned ct = 0;
	for(ParticleWorld::Particles::iterator p = particles->begin();
		p != particles->end();
		p++)
	{
		real z = (*p)->GetPosition().z;
        bool underGround = false;
		Box* b = dynamic_cast<Box*>(*p);
		if(b)
		{
            underGround = (z - b->GetHalfSize().z < horizon);
		}
		else
		{
            underGround = (z < horizon);
		}

		if(underGround)
		{
			contact->contactNormal = Vector3(0, 0, 1);
			contact->particle[0] = *p;
			contact->particle[1] = NULL;
			if(b)
            {
                contact->penetration = horizon - z + b->GetHalfSize().z;
                contact->restitution = elasticity * b->GetElasticity();
            }
            else
            {
                contact->penetration = horizon - z;
                contact->restitution = elasticity;
            }
			contact++;
			ct++;
		}

		if(ct >= limit)
            return ct;
	}
	return ct;
}

SceneWallsContacts::SceneWallsContacts(real leftWall, real rightWall, real backWall, real frontWall)
    :leftWall(leftWall), rightWall(rightWall), backWall(backWall), frontWall(frontWall), elasticity(0.9f)
{
}

void SceneWallsContacts::Init(ParticleWorld::Particles* particles)
{
    this->particles = particles;
}

void SceneWallsContacts::SetWalls(real leftWall, real rightWall, real backWall, real frontWall)
{
    this->leftWall = leftWall;
    this->rightWall = rightWall;
    this->backWall = backWall;
    this->frontWall = frontWall;
}

void SceneWallsContacts::SetElasticity(real elasticity)
{
    this->elasticity = elasticity;
}

unsigned SceneWallsContacts::AddContact(ParticleContact* contact, unsigned limit) const
{
	unsigned ct = 0;
	for(ParticleWorld::Particles::iterator p = particles->begin();
		p != particles->end();
		p++)
	{
		real x = (*p)->GetPosition().x;
		real y = (*p)->GetPosition().y;
		bool beyondWalls = false;
		Vector3 contactNormal = Vector3::ZERO;
		real penetration = 0;

		if(x < leftWall)
		{
		    beyondWalls = true;
		    contactNormal = Vector3::RIGHT;
		    penetration = leftWall - x;
		}
		else if(x > rightWall)
        {
		    beyondWalls = true;
		    contactNormal = Vector3::LEFT;
		    penetration = x - rightWall;
        }

        if(y < backWall)
        {
            contactNormal += Vector3::FRONT;
		    if(beyondWalls)
            {
                contactNormal.Normalize();
                penetration = real_sqrt(penetration * penetration + (backWall - y) * (backWall - y));
            }
            else
            {
                beyondWalls = true;
                penetration = backWall - y;
            }
        }
        else if(y > frontWall)
        {
            contactNormal += Vector3::BACK;
		    if(beyondWalls)
            {
                contactNormal.Normalize();
                penetration = real_sqrt(penetration * penetration + (y - frontWall) * (y - frontWall));
            }
            else
            {
                beyondWalls = true;
                penetration = y - frontWall;
            }
        }

        if(beyondWalls)
        {
            contact->contactNormal = contactNormal;
            contact->particle[0] = *p;
            contact->particle[1] = NULL;
            contact->penetration = penetration;
            contact->restitution = elasticity;
            contact++;
            ct++;
        }

		if(ct >= limit)
            return ct;
	}
	return ct;
}

SceneWallsContactsWithBox::SceneWallsContactsWithBox(real leftWall, real rightWall, real backWall, real frontWall)
    :SceneWallsContacts(leftWall, rightWall, backWall, frontWall)
{
}

unsigned SceneWallsContactsWithBox::AddContact(ParticleContact* contact, unsigned limit) const
{
	unsigned ct = 0;
	for(ParticleWorld::Particles::iterator p = particles->begin();
		p != particles->end();
		p++)
	{
		bool beyondWalls = false;
		Vector3 contactNormal = Vector3::ZERO;
		real penetration = 0;
		real restitution = elasticity;

	    Box* b = dynamic_cast<Box*>(*p);
	    if(b)
        {
            real bLeft = b->GetLeft();
            real bRight = b->GetRight();
            real bBack = b->GetBack();
            real bFront = b->GetFront();
            if(bLeft < leftWall)
            {
                beyondWalls = true;
                contactNormal = Vector3::RIGHT;
                penetration = leftWall - bLeft;
                restitution *= b->GetElasticity();
            }
            else if(bRight > rightWall)
            {
                beyondWalls = true;
                contactNormal = Vector3::LEFT;
                penetration = bRight - rightWall;
                restitution *= b->GetElasticity();
            }

            if(bBack < backWall)
            {
                contactNormal += Vector3::FRONT;
                if(beyondWalls)
                {
                    contactNormal.Normalize();
                    penetration = real_sqrt(penetration * penetration + (backWall - bBack) * (backWall - bBack));
                }
                else
                {
                    beyondWalls = true;
                    penetration = backWall - bBack;
                    restitution *= b->GetElasticity();
                }
            }
            else if(bFront > frontWall)
            {
                contactNormal += Vector3::BACK;
                if(beyondWalls)
                {
                    contactNormal.Normalize();
                    penetration = real_sqrt(penetration * penetration + (bFront - frontWall) * (bFront - frontWall));
                }
                else
                {
                    beyondWalls = true;
                    penetration = bFront - frontWall;
                    restitution *= b->GetElasticity();
                }
            }
        }
        else
        {
            real x = (*p)->GetPosition().x;
            real y = (*p)->GetPosition().y;
            if(x < leftWall)
            {
                beyondWalls = true;
                contactNormal = Vector3::RIGHT;
                penetration = leftWall - x;
            }
            else if(x > rightWall)
            {
                beyondWalls = true;
                contactNormal = Vector3::LEFT;
                penetration = x - rightWall;
            }

            if(y < backWall)
            {
                contactNormal += Vector3::FRONT;
                if(beyondWalls)
                {
                    contactNormal.Normalize();
                    penetration = real_sqrt(penetration * penetration + (backWall - y) * (backWall - y));
                }
                else
                {
                    beyondWalls = true;
                    penetration = backWall - y;
                }
            }
            else if(y > frontWall)
            {
                contactNormal += Vector3::BACK;
                if(beyondWalls)
                {
                    contactNormal.Normalize();
                    penetration = real_sqrt(penetration * penetration + (y - frontWall) * (y - frontWall));
                }
                else
                {
                    beyondWalls = true;
                    penetration = y - frontWall;
                }
            }
        }

        if(beyondWalls)
        {
            contact->contactNormal = contactNormal;
            contact->particle[0] = *p;
            contact->particle[1] = NULL;
            contact->penetration = penetration;
            contact->restitution = restitution;
            contact++;
            ct++;
        }

		if(ct >= limit)
            return ct;
	}
	return ct;
}

}
