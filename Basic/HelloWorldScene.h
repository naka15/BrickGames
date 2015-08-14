#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Box2D\Box2D.h"
#include "GLES-Render.h"
#include "ContactListener.h"

USING_NS_CC;
class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    CREATE_FUNC(HelloWorld);

	//factor
	float factor = Director::getInstance()->getContentScaleFactor();
	//objects for sprites
	Texture2D* texture;
	Sprite* ball;
	Sprite* paddle;
	Vector<Sprite*> targets;

	//벽돌 숫자 조정
	int	BRICKS_HEIGHT;
	int BRICKS_WIDTH;
	stack<b2Body*> removeList;
	//게임이 진행중인지 체크
	bool isPlaying=true;
	//패들이 터치되었나
	bool isPaddleTouched;
	//공의 움직임을 저장하는 변수
	Point ballMovement;
	~HelloWorld();
	//debug
	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
	virtual void onEnter();
	virtual void onExit();
	virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
	virtual void onTouchEnded(Touch* touch, Event* event);
	//void contactBallNPaddle(b2Contact *contact, b2Body *bodyA, b2Body *bodyB);
	void contactBallNBrick(vector<contactData> destroy_list);

	b2Body* getBodyAtTab(Point p);
	b2Body* addNewSprite(Point point, Size size, b2BodyType bodytype, const char* spriteName, int type);

	bool makeWorld(bool debug);
	void initializeBricks();
	void initializeBall();
	void initializePaddle();
	void startGame(float dt);
	void gameLogic(float dt);
	void processCollision(Sprite* brick);
	
	Size winSize;
	b2World* _world;
	b2Body *paddleBody;
	b2Body *ballBody;

	ContactListener* myContactListener;
//debug
	GLESDebugDraw* m_debugDraw;

protected:
	void onDraw(const cocos2d::Mat4& transform, uint32_t flags);
	cocos2d::CustomCommand _customCmd;
};

#endif // __HELLOWORLD_SCENE_H__