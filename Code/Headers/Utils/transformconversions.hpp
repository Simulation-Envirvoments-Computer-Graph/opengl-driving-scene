#pragma once

#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>


namespace Utils
{
  class TransformConversions
  {
    public:
      static glm::mat4 btScalar2glmMat4(btScalar* matrix);
      static glm::vec3 btVector32glmVec3(btVector3 matrix);
      static btVector3 glmVec32btVector3(glm::vec3 vec);
  };
}