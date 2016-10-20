#include "plinks.h"

namespace stef
{

real ParticleLink::GetCurrentLength() const
{
    Vector3 relativePos = particle[0]->GetPosition() - particle[1]->GetPosition();
    return relativePos.GetMagnitude();
}

unsigned ParticleCable::FillContact(ParticleContact* contact, unsigned limit) const
{
    real length = GetCurrentLength();
    if(length < maxLength)
        return 0;   //no contact

    contact->particle[0] = particle[0];
    contact->particle[1] = particle[1];

    Vector3 normal = particle[1]->GetPosition() - particle[0]->GetPosition();
    normal.Normalize();
    contact->contactNormal = normal;

    contact->penetration = length - maxLength;
    contact->restitution = restitution;

    return 1;
}

unsigned ParticleRod::FillContact(ParticleContact* contact, unsigned limit) const
{
    real currentLength = GetCurrentLength();
    if(currentLength == length)
        return 0;

    contact->particle[0] = particle[0];
    contact->particle[1] = particle[1];

    Vector3 normal = particle[1]->GetPosition() - particle[0]->GetPosition();
    normal.Normalize();
    if(currentLength > length)
    {
        contact->contactNormal = normal;
        contact->penetration = currentLength - length;
    }
    else
    {
        contact->contactNormal = normal * -1;
        contact->penetration = length - currentLength;
    }

    contact->restitution = 0;   //rob has no bounciness

    return 1;
}

}
