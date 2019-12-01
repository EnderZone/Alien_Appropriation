#pragma once


#include "entity_node.h"

#include <string.h>


namespace game
{

	class ProjectileNode : public EntityNode
	{
	public:
		ProjectileNode(std::string name, const Resource *geometry, const Resource *material, float lifespan, glm::vec3 initialPos, glm::vec3 initialVelocityVec, const Resource *texture = nullptr);
		~ProjectileNode();

		void Update();
	
	protected:
		float mRemainingLife;
		float mLastTime;

	};


	class HeatMissileNode : public ProjectileNode
	{
	public:
		HeatMissileNode(std::string name, const Resource *geometry, const Resource *material, float lifespan, glm::vec3 initialPos, glm::vec3 initialVelocityVec, const Resource *texture = nullptr);
		~HeatMissileNode();

		void Update();
	private:


		float mMaxVelocity;
	};



}