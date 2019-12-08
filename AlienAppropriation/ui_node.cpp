#include "ui_node.h"

namespace game {
	UINode::UINode(const std::string name, const Resource *geometry, const Resource *material, const Resource* texture) : SceneNode(name, geometry, material, texture) {}
	UINode::~UINode() {}

	void UINode::addStat(float* stat_pointer, float* max_pointer) {
		tracker = stat_pointer;
		max_stat = max_pointer;
	}

	void UINode::Update() {
		float degrees = 5.0f * (*tracker / *max_stat);
		SetScale(glm::vec3(degrees, 0.5f, 1.0f));
	}
}