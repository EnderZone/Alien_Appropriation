#pragma once


#include "scene_node.h"

#include <string.h>


namespace game
{

	class ProjectileNode : public SceneNode
	{
	public:
		ProjectileNode(std::string name, const Resource *geometry, const Resource *material, float lifespan);
		~ProjectileNode();

		void Update();
	
	private:
		float mRemainingLife;
		float mLastTime;

	};



}