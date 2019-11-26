#ifndef ENTITY__GAME_NODES_H_
#define ENTITY__GAME_NODES_H_


#include "entity_node.h"

namespace game {

	// Class that manages one object in a scene 
	class CowEntityNode : public EntityNode {

	public:
		// Create scene node from given resources
		CowEntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

		// Destructor
		~CowEntityNode();

		void Update();

	private:

		enum Behaviour
		{
			walk,
			stand,
			run	
		};

		Behaviour mBehaviour;

		// Timers for behaviors purposes
		float mLastTimer;
		float mNextTimer;

	}; // class CowEntityNode

} // namespace game

#endif // ENTITY__GAME_NODES_H_
