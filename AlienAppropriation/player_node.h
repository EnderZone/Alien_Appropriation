#pragma once

#include "scene_node.h"
#include "base_node.h"
#include "camera.h"
#include "entity_node.h"
#include "projectile_node.h"
#include "entity_game_nodes.h"


#include <string.h>

namespace game
{	
	enum DamageType { BULL = 10, MISSILE = 15, GUN = 3};

	class PlayerNode : public SceneNode
	{
	public:
		PlayerNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture);
		~PlayerNode();

		glm::vec3 getPosition(void);

		void rotateLeft();
		void rotateRight();
		void rotateForward();
		void rotateBackward();

		void rotateByCamera();

		virtual void draw(SceneNode* camera, glm::mat4 parentTransf = glm::mat4(1.0));
		virtual void update(double deltaTime);

		void setPlayerPosition();
		float getDistanceFromCamera();
		inline float* getHullStrength() { return hull_strength; }
		inline void addWeapon(SceneNode* w) { weapons.push_back(w); };
		inline void toggleTractorBeam(bool active) { tractor_beam_on = active; }
		inline void toggleShields(bool active) { shielding_on = active; }
		inline void addHealthTracker(float* t) { hull_strength = t; }
		inline void addEnergyTracker(float* t) { energy = t; }
		void PlayerNode::checkWeapons();

		inline bool isTractorBeamActive() { return tractor_beam_on; }
		inline bool isShieldActive() { return shielding_on;  }
		void takeDamage(DamageType);
		void dropBomb();
		void addCollected(std::string type);

		inline void addEnergy(float f) { *energy += f; }
		inline void addHealth(float f) { *hull_strength += f; }

	private:
		float x_tilt_percentage;
		float y_tilt_percentage;
		float forward_factor;

		float* energy;
		float* hull_strength;

		int cowsCollected;
		int hayCollected;

		bool tractor_beam_on = false;
		bool shielding_on = false;

		int bombCounter = 0;

		void SetupShader(GLuint program, glm::mat4& parentTransf = glm::mat4(1.0));

		
		std::vector<SceneNode*> weapons;		
	};
}
