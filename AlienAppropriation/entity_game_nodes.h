#ifndef ENTITY__GAME_NODES_H_
#define ENTITY__GAME_NODES_H_


#include "entity_node.h"

namespace game {

	// Cow :)
	// Cow talks around grazing, oblivious
	// Player tries to take them 
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


	// Bull
	// Looks like cow, but isnt
	// Has a different behaviour and will cause damage to player if caught
	class BullEntityNode : public EntityNode {

	public:
		// Create scene node from given resources
		BullEntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

		// Destructor
		~BullEntityNode();

		void Update();

	private:

		enum Behaviour
		{
			walk,
			stand,
			run,
			thrash
		};

		Behaviour mBehaviour;

		// Timers for behaviors purposes
		float mLastTimer;
		float mNextTimer;

	}; // class CowEntityNode

} // namespace game

#endif // ENTITY__GAME_NODES_H_
