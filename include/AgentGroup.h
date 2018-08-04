#pragma once

#include <agent.h>

#include <cstdint>

#define N_AGENTS 10

class World;

class AgentGroup {
public:
  AgentGroup() {};
  ~AgentGroup() {};

  void init(World* world, const Body::Color color, const Body::Type type);
  void update(const uint32_t dt);
  void render() const;
  void shutdown();
  void setSteering(Body::SteeringMode steering);

private:
  World * world_;
  Agent agents_[N_AGENTS];
};