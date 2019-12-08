#pragma once

#include "scene_node.h"

namespace game {
	class UINode : public SceneNode {
	public:
		UINode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture);
		~UINode();
		
		void addStat(float*, float*);

		virtual void update(double deltaTime);

	private:
		float* max_stat;
		float* tracker;
	};
}