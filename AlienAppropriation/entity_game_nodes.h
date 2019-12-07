#ifndef ENTITY__GAME_NODES_H_
#define ENTITY__GAME_NODES_H_


#include "entity_node.h"

// Implemented game entities
// Cow
// Bull
// Farmer with shotgun
// Stationary cannon (homing missile)

// Potential game entities
// Military person with rifle
// Military person with missile launcher
// Jeep with mounted gun
// Tank with mounted cannon
// Stationary cannon (laser)
// Stationary cannon (missile)

namespace game {

	// Cow :)
	// Cow walks around grazing, oblivious
	// Player tries to take them 
	class CowEntityNode : public EntityNode {

	public:
		// Create scene node from given resources
		CowEntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

		// Destructor
		~CowEntityNode();

		virtual void Update();

	private:

		void doStand();
		void doWalk();
		void doRun();

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

		void doStand();
		void doWalk();
		void doRun();

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

	}; // class BullEntityNode


	// Farmer
	// Farmer man, has a shotgun and will shoot you if you get too close
	// Probably only has one behavior and thats walk/hop towards player and shoot if within range
	class FarmerEntityNode : public EntityNode {

	public:
		// Create scene node from given resources
		FarmerEntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

		// Destructor
		~FarmerEntityNode();

		void Update();
		

	private:

		void doFire();

		// Timers for behaviors purposes
		float mNextTimer;

	}; // class FarmerEntityNode


	// Cannon
	// Doesnt move, rotates towards player and fires missiles
	class CannonMissileEntityNode : public EntityNode {

	public:
		// Create scene node from given resources
		CannonMissileEntityNode(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture = NULL);

		// Destructor
		~CannonMissileEntityNode();

		void Update();

	private:

		void fireHeatMissile();

		// Timers for behaviors purposes
		float mLastTimer;
		float mNextTimer;

		// total number of missiles fired by this cannon
		int mProjectiles;

	}; // class CannonMissileEntityNode



} // namespace game

#endif // ENTITY__GAME_NODES_H_
