#include "ContactListener.h"

#define PTM_RATIO 32
#define Paddle_halfLength 40
using namespace std;
USING_NS_CC;
ContactListener::ContactListener(){

}
ContactListener::~ContactListener(){

}

void ContactListener::BeginContact(b2Contact *contact){
	log("Contact:Begin");

	
	b2Fixture *fixA = contact->GetFixtureA();
	b2Fixture *fixB = contact->GetFixtureB();
	b2Body *bodyA = fixA->GetBody();
	b2Body *bodyB = fixB->GetBody();
	contactData data = {fixA,fixB};
	contactList.push_back(data);
	if ((Sprite*)bodyA->GetUserData() != NULL && (Sprite*)bodyB->GetUserData() != NULL){
	this->contactBallNPaddle(contact, bodyA, bodyB);
	//this->contactBallNBrick(contact, bodyA, bodyB);
	}

}

void ContactListener::EndContact(b2Contact *contact){
	log("Contact:End");
	contactData data = {contact->GetFixtureA(),contact->GetFixtureB()};
	vector<contactData>::iterator pos;
	pos = find(contactList.begin(), contactList.end(), data);
	if (pos != contactList.end()){
		contactList.erase(pos);
	}
}
//
//void ContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold){
//	log("Contact:PreSolve");
//}
//void ContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse){
//	log("Contact:PostSolve");
//	
//}

void ContactListener::contactBallNPaddle(b2Contact *contact, b2Body *bodyA, b2Body *bodyB){

	Sprite* SpriteA = (Sprite*)bodyA->GetUserData();
	Sprite* SpriteB = (Sprite*)bodyB->GetUserData();
	if ((SpriteA->getTag() == 0 && SpriteB->getTag() == 1) || (SpriteA->getTag() == 1 && SpriteB->getTag() == 0)){
	
		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);
		b2Vec2 contact_Point = worldManifold.points[0];
		b2Vec2 real_Point;
		real_Point.x = contact_Point.x * 32.0f;
		real_Point.y = contact_Point.y * 32.0f;
		log("x : %f", (float)real_Point.x);
		log("y : %f", (float)real_Point.y);
		if (bodyA->GetType() == b2_dynamicBody){
			Point paddlePoint = SpriteB->getPosition();
			float x = (float)real_Point.x - paddlePoint.x;
			if (x>-40 && x<40 && real_Point.y>50){
				float y = (float)sqrt(1600.0f - x*x);
				b2Vec2 velocity = bodyA->GetLinearVelocity();
				float speed = velocity.x*velocity.x + velocity.y*velocity.y;
				log("velocity x : %f, velocity y : %f", velocity.x, velocity.y);
				x = x*sqrt(speed * 1024 / 1600);
				y = y*sqrt(speed * 1024 / 1600);
				bodyA->SetLinearVelocity(b2Vec2((float)x / PTM_RATIO, (float)y / PTM_RATIO));
				log("x : %f, y: %f", x, y);
			}
		}
		else{
			
			Point paddlePoint = SpriteA->getPosition();
			float x = (float)real_Point.x - paddlePoint.x;
			if (x>-40 && x<40 && real_Point.y>50){
				float y = (float)sqrt(1600.0f - x*x);
				b2Vec2 velocity = bodyB->GetLinearVelocity();
				log("velocity x : %, velocity y : %f", velocity.x, velocity.y);
				float speed = velocity.x*velocity.x + velocity.y*velocity.y;
				x = x*sqrt(1024 * speed / 1600);
				y = y*sqrt(1024 * speed / 1600);
				bodyB->SetLinearVelocity(b2Vec2((float)x / PTM_RATIO, (float)y / PTM_RATIO));
				log("x : %f, y: %f", x, y);
			}
		}
	}
}



//void ContactListener::contactBallNBrick(b2Contact *contact, b2Body *bodyA, b2Body *bodyB){
//	Sprite* SpriteA = (Sprite*)bodyA->GetUserData();
//	Sprite* SpriteB = (Sprite*)bodyB->GetUserData();
//	if ((SpriteA->getTag() == 0 && SpriteB->getTag() == 2)){
//		
//	}
//	else if ((SpriteA->getTag() == 2 && SpriteB->getTag() == 0)){
//
//	}
//}