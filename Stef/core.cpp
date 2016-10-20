#include "core.h"

namespace stef
{

const Vector3 Vector3::ZERO = Vector3(0, 0, 0);
const Vector3 Vector3::GRAVITY = Vector3(0, 0, -9.81);
const Vector3 Vector3::HIGH_GRAVITY = Vector3(0, 0, -19.62);
const Vector3 Vector3::RIGHT = Vector3(1, 0, 0);
const Vector3 Vector3::LEFT = Vector3(-1, 0, 0);
const Vector3 Vector3::FRONT = Vector3(0, 1, 0);
const Vector3 Vector3::BACK = Vector3(0, -1, 0);
const Vector3 Vector3::TOP = Vector3(0, 0, 1);
const Vector3 Vector3::BOTTOM = Vector3(0, 0, -1);

}
