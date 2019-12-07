#include "player_node.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>

namespace game {
	PlayerNode::PlayerNode(const std::string name, const Resource *geometry, const Resource *material, BaseNode* camera) : SceneNode(name, geometry, material),
		forward_factor(40.0f),
		x_tilt_percentage(0.0f),
		y_tilt_percentage(0.0f),
		hull_strength(100),
		energy(100),
		tractor_beam_on(false),
		shielding_on(false)
	{
		// Set This as the parentNode of the camera while taking its own parent as his
		camera->addChildNode(this);
	}

	PlayerNode::~PlayerNode() {}

	glm::vec3 PlayerNode::GetPosition(void)
	{
		return mPosition + (dynamic_cast<Camera*>(getParentNode()))->GetPosition();
	}

	void PlayerNode::rotateLeft() {
		x_tilt_percentage += glm::pi<float>() / 20.0f;
		x_tilt_percentage = glm::min(glm::pi<float>() / 2.0f, x_tilt_percentage);
	}

	void PlayerNode::rotateRight() {
		x_tilt_percentage -= glm::pi<float>() / 20.0f;
		x_tilt_percentage = glm::max(-glm::pi<float>() / 2.0f, x_tilt_percentage);
	}

	void PlayerNode::rotateForward() {
		y_tilt_percentage += glm::pi<float>() / 20.0f;
		y_tilt_percentage = glm::min(glm::pi<float>() / 2.0f, y_tilt_percentage);
	}

	void PlayerNode::rotateBackward() {
		y_tilt_percentage -= glm::pi<float>() / 20.0f;
		y_tilt_percentage = glm::max(-glm::pi<float>() / 2.0f, y_tilt_percentage);
	}

	void PlayerNode::rotateByCamera() {
		float velocity_limit = glm::pi<float>() / 2.0f;
		
		glm::vec3 current_velocity = ((Camera*)this->getParentNode())->getCurrentVelocity();

		x_tilt_percentage = -current_velocity.x * velocity_limit;
		y_tilt_percentage = current_velocity.z * velocity_limit;
	
		x_tilt_percentage = glm::max(-1.0f, x_tilt_percentage);
		x_tilt_percentage = glm::min( 1.0f, x_tilt_percentage);
		
		y_tilt_percentage = glm::max(-1.0f, y_tilt_percentage);
		y_tilt_percentage = glm::min( 1.0f, y_tilt_percentage);
		
		// std::cout << "PERCENTAGES ::: " << x_tilt_percentage << " " << y_tilt_percentage << std::endl;
	}

	void PlayerNode::Draw(Camera* camera, glm::mat4 parentTransf) {
		// Select proper material (shader program)
		glUseProgram(mMaterial);

		// Set geometry to draw
		glBindBuffer(GL_ARRAY_BUFFER, mArrayBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementArrayBuffer);

		// Set globals for camera
		camera->SetupShader(mMaterial);

		// Set world matrix and other shader input variables
		SetupShader(mMaterial, parentTransf);

		// Draw geometry
		if (mMode == GL_POINTS) {
			glDrawArrays(mMode, 0, mSize);
		}
		else {
			glDrawElements(mMode, mSize, GL_UNSIGNED_INT, 0);
		}

		for (BaseNode* bn : getChildNodes())
		{
			bn->Draw(camera, parentTransf);
		}

		for (BaseNode *bn : weapons) {
			std::string node_name = bn->getName();

			if (tractor_beam_on && node_name.compare("TRACTORBEAM") == 0) {
				bn->Draw(camera, parentTransf);
			}
			if (shielding_on && node_name.compare("SHIELD") == 0) {
				bn->Draw(camera, parentTransf);
			}
		}
	}

	void PlayerNode::Update() {
		SceneNode::Update();
		rotateByCamera();
		setPlayerPosition();
		checkWeapons();

		for (BaseNode* bn : getChildNodes())
		{
			bn->Update();
		}

		for (BaseNode* bn : weapons)
		{
			bn->Update();
		}
	}

	void PlayerNode::checkWeapons() {
		if (tractor_beam_on)
			updateTractorBeam();
		if (shielding_on)
			updateShield();
	}

	void PlayerNode::updateTractorBeam() {
		BaseNode* rootNode = getRootNode();
		std::vector<BaseNode*>* to_remove = new std::vector<BaseNode*>;
		for (BaseNode* bn : rootNode->getChildNodes()) {
			EntityNode* en = dynamic_cast<EntityNode*>(bn);
			if (en != NULL) {
				suckEntity(en, to_remove);
			}
		}

		for (BaseNode* removee : *to_remove) {
			rootNode->removeChildNode(removee->getName());
			delete removee;
		}
		delete to_remove;
	}

	void PlayerNode::takeDamage(DamageType damage) {
		hull_strength -= damage;
	}

	void PlayerNode::updateShield() {
		BaseNode* rootNode = getRootNode();
		std::vector<BaseNode*>* to_remove = new std::vector<BaseNode*>;
		for (BaseNode* bn : rootNode->getChildNodes()) {
			ProjectileNode* pn = dynamic_cast<ProjectileNode*>(bn);
			if (pn != NULL) {
				shieldProjectile(pn, to_remove);
			}
		}
		for (BaseNode* removee : *to_remove) {
			rootNode->removeChildNode(removee->getName());
			delete removee;
		}
		delete to_remove;
	}

	void PlayerNode::suckEntity(EntityNode* en, std::vector<BaseNode*>* to_remove) {
		glm::vec3 curr_pos = GetPosition();
		glm::vec3 entity_pos = en->GetPosition();
				
		if (curr_pos.y < entity_pos.y)
			return;

		float dist = glm::distance(glm::vec2(curr_pos.x, curr_pos.z), glm::vec2(entity_pos.x, entity_pos.z));
		float height = curr_pos.y - entity_pos.y;

		if (dist < height / 4.0f) {
			en->rise();
			
			
			CowEntityNode* cn = dynamic_cast<CowEntityNode*>(en);
			BullEntityNode* bn = dynamic_cast<BullEntityNode*>(en);

			if ((bn != NULL || cn != NULL) && glm::distance(curr_pos, entity_pos) < 3.0f) {
				to_remove->push_back(en);
				if (bn != NULL) {
					takeDamage(BULL);
				}
				else {
					SceneNode* collected = new SceneNode(*((SceneNode*)cn));
					addChildNode(collected);
					collected->SetPosition(glm::vec3(0.0f));
					collected->Translate(glm::vec3(2.0f * cos(getChildNodes().size()), 1.0f,2.0f * sin(getChildNodes().size())));
					collected->Scale(glm::vec3(0.25f));
				}
			}
		}
	}

	void PlayerNode::shieldProjectile(ProjectileNode* pn, std::vector<BaseNode*>* to_remove) {
		glm::vec3 curr_pos = GetPosition();
		glm::vec3 entity_pos = pn->GetPosition();

		float dist = glm::distance(curr_pos, entity_pos);
		
		std::cout << dist << std::endl;

		if (dist < 7.5f) {
			to_remove->push_back(pn);
		}
	}

	BaseNode* PlayerNode::getRootNode() {
		BaseNode* rootNode = getParentNode();

		while (rootNode != NULL && rootNode->getName().compare("ROOT") != 0) {
			rootNode = rootNode->getParentNode();
		}

		return rootNode;
	}

	void PlayerNode::setPlayerPosition() {
		mPosition = -forward_factor * glm::vec3(0.0f, 0.0f, 1.0f);
	}

	float PlayerNode::getDistanceFromCamera() {
		return forward_factor;
	}

	void PlayerNode::SetupShader(GLuint program, glm::mat4& parentTransf /*= glm::mat4(1.0)*/) {

		// Set attributes for shaders
		GLint vertex_att = glGetAttribLocation(program, "vertex");
		glVertexAttribPointer(vertex_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(vertex_att);

		GLint normal_att = glGetAttribLocation(program, "normal");
		glVertexAttribPointer(normal_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(normal_att);

		GLint color_att = glGetAttribLocation(program, "color");
		glVertexAttribPointer(color_att, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(color_att);

		GLint tex_att = glGetAttribLocation(program, "uv");
		glVertexAttribPointer(tex_att, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void *)(9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(tex_att);

		// Adding the tilts when moving
		float angle_x = (glm::pi<float>() / 16) * glm::sin(x_tilt_percentage);
		float angle_y = (glm::pi<float>() / 16) * glm::sin(y_tilt_percentage);

		// std::cout << "PERCENTAGES ::: " << x_tilt_percentage << " " << y_tilt_percentage << std::endl;
		// std::cout << "ANGLES ::: " << angle_x << " " << angle_y << std::endl;
		glm::quat current_rotation;
		current_rotation = glm::quat_cast(glm::rotate(glm::mat4(), angle_x, glm::vec3(0.0, 0.0, 1.0)));
		current_rotation = glm::normalize(current_rotation);
		current_rotation *= glm::quat_cast(glm::rotate(glm::mat4(), angle_y, glm::vec3(1.0, 0.0, 0.0)));
		current_rotation = glm::normalize(current_rotation);
		mOrientation *= glm::angleAxis(glm::pi<float>() / 2, glm::vec3(0.0f, 1.0f, 0.0f));


		// Aply transformations *ISROT*
		glm::mat4 scaling = glm::scale(glm::mat4(1.0), mScale);
		glm::mat4 rotation = glm::mat4_cast(current_rotation);
		glm::mat4 translation = glm::translate(glm::mat4(1.0), mPosition);
		glm::mat4 temp_transf = parentTransf * translation * rotation;
		parentTransf *= translation * glm::mat4_cast(glm::normalize(mOrientation));

		// Scaling is done only on local object
		glm::mat4 transf = glm::scale(temp_transf, mScale);

		GLint world_mat = glGetUniformLocation(program, "world_mat");
		glUniformMatrix4fv(world_mat, 1, GL_FALSE, glm::value_ptr(transf));

		// Timer
		GLint timer_var = glGetUniformLocation(program, "timer");
		double current_time = glfwGetTime();
		glUniform1f(timer_var, (float)current_time);
	}

}