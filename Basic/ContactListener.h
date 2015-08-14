#ifndef _ContactListener_
#define _ContactListener_

#include "cocos2d.h"
#include "Box2D\Box2D.h"

using namespace std;

struct contactData{
	b2Fixture* FixtureA;
	b2Fixture* FixtureB;

	contactData(b2Fixture* fixA, b2Fixture* fixB){
		FixtureA = fixA;
		FixtureB = fixB;
	}

	bool operator==(const contactData& other) const
	{
		return (FixtureA == other.FixtureA) && (FixtureB == other.FixtureB);
	}
};

class ContactListener : public b2ContactListener{
public:
	ContactListener();
	~ContactListener();

	vector<contactData> contactList;

	virtual void BeginContact(b2Contact *contact);
	void contactBallNPaddle(b2Contact *contact, b2Body *bodyA, b2Body *bodyB);
	//void contactBallNBrick(b2Contact *contact, b2Body *bodyA, b2Body *bodyB);
	virtual void EndContact(b2Contact *contact);
	//virtual void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);
	//virtual void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);

};



#endif //ContactListener