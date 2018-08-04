//----------------------------------------------------------------------------
//                                                        _   ________  __
//  Copyright VIU 2018                                   | | / /  _/ / / /
//  Author: Ivan Fuertes <ivan.fuertes@campusviu.es>     | |/ // // /_/ /
//                                                       |___/___/\____/
//----------------------------------------------------------------------------

#include <body.h>
#include <agent.h>
#include <defines.h>
#include <debug_draw.h>

void Body::init(const Color color, const Type type) {
  type_ = type;
  color_ = color;

  switch(color) {
    case Color::Green: sprite_.loadFromFile(AGENT_GREEN_PATH); break;
    case Color::Blue: sprite_.loadFromFile(AGENT_BLUE_PATH); break;
    case Color::Purple: sprite_.loadFromFile(AGENT_PURPLE_PATH); break;
    case Color::Red: sprite_.loadFromFile(AGENT_RED_PATH); break;
    default: sprite_.loadFromFile(AGENT_GREEN_PATH);
  }

  steering_mode_ = SteeringMode::Kinematic_Seek;
}

void Body::update(const uint32_t dt) {
  KinematicSteering steering;
  if (type_ == Type::Autonomous) {
    switch (this->steering_mode_) {
    case Body::SteeringMode::Kinematic_Seek: 
      this->kinematicSeek(state_, target_->getKinematic(), &steering);
      break;
    case Body::SteeringMode::Kinematic_Flee: 
      this->kinematicFlee(state_, target_->getKinematic(), &steering);
      break;
    }
    this->applySteering(steering, dt);
  } else {
    updateManual(dt);
  }



  sprite_.setPosition(state_.position.x(), state_.position.y());
  sprite_.setRotation(state_.orientation);

}

void Body::applySteering(const KinematicSteering& steering, const uint32_t ms) {
  const float dt = ms * 0.001;
  state_.velocity = steering.velocity;
  state_.speed = state_.velocity.length();
  state_.position += state_.velocity * dt;
  state_.orientation = state_.orientation + steering.rotation * dt;
}

void Body::render() const {
  sprite_.render();

  DebugDraw::drawVector(dd.red.pos, dd.red.v, 0xFF, 0x00, 0x00, 0xFF);
  DebugDraw::drawVector(dd.green.pos, dd.green.v, 0x00, 0x50, 0x00, 0xFF);
  DebugDraw::drawVector(dd.blue.pos, dd.blue.v, 0x00, 0x00, 0xFF, 0xFF);
  DebugDraw::drawPositionHist(state_.position);
}

void Body::setTarget(Agent* target) {
  target_ = target;
}


void Body::updateManual(const uint32_t dt) {
  float time = dt * 0.001f;             //dt comes in miliseconds

  MathLib::Vec2 orientation;
  orientation.fromPolar(1.0f, state_.orientation);
  state_.velocity = orientation.normalized() * state_.speed;
  state_.position += state_.velocity * time;

  keepInSpeed();
  keepInBounds();

  dd.green.pos = state_.position;
  dd.green.v = state_.velocity;
}

void Body::setOrientation(const Vec2& velocity) {
  if (velocity.length2() > 0) {
    state_.orientation = atan2(velocity.y(), velocity.x());
  }
}

void Body::keepInBounds() {
  if (state_.position.x() > WINDOW_WIDTH) state_.position.x() = 0.0f;
  if (state_.position.x() < 0.0f) state_.position.x() = WINDOW_WIDTH;
  if (state_.position.y() > WINDOW_HEIGHT) state_.position.y() = 0.0f;
  if (state_.position.y() < 0.0f) state_.position.y() = WINDOW_HEIGHT;
}

void Body::keepInSpeed() {
  if (state_.velocity.length() > max_speed_) {
    state_.velocity = state_.velocity.normalized() * max_speed_;
  }
}

void Body::kinematicSeek(const KinematicStatus& character, const KinematicStatus* target, KinematicSteering* steering) const{
  steering->velocity = (target->position - character.position).normalized() * this->max_speed_;
  steering->rotation = 0.0f;
}

void Body::kinematicFlee(const KinematicStatus& character, const KinematicStatus* target, KinematicSteering* steering) const{
  steering->velocity = (character.position - target->position).normalized() * this->max_speed_;
  steering->rotation = 0.0f;
}
