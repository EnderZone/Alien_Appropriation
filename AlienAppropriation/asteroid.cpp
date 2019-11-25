#include "asteroid.h"

namespace game {

Asteroid::Asteroid(const std::string name, const Resource *geometry, const Resource *material) : SceneNode(name, geometry, material) {
}


Asteroid::~Asteroid(){
}


glm::quat Asteroid::GetAngM(void) const {

    return mAngularMomentum;
}


void Asteroid::SetAngM(glm::quat angm){

    mAngularMomentum = angm;
}


void Asteroid::Update(void){

    Rotate(mAngularMomentum);
}
            
} // namespace game
