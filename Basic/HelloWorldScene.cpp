#include "HelloWorldScene.h"
#define PTM_RATIO 32
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	winSize = Director::getInstance()->getWinSize();

	srand((int)time(NULL));
	//터치 활성화 : onEnter,onExit에서 처리

	//벽돌 개수 지정
	BRICKS_HEIGHT = 4;
	BRICKS_WIDTH = 5;
   
	//벽돌 텍스쳐 로딩
	texture = Director::getInstance()->getTextureCache()->addImage("Images/white-512x512.png");
	if (this->makeWorld(true)){
		//start game after 2 second
		this->scheduleOnce(schedule_selector(HelloWorld::startGame), 2.0f);
	}
	
	
	

    return true;
}
//월드 생성
bool HelloWorld:: makeWorld(bool debug){
	b2Vec2 gravity = b2Vec2(0, 0.f);
	_world = new b2World(gravity);
	_world->SetAllowSleeping(true);
	_world->SetContinuousPhysics(true);
	if (debug){
		m_debugDraw = new GLESDebugDraw(PTM_RATIO);
		_world->SetDebugDraw(m_debugDraw);
		uint32 flags = 0;

		flags += b2Draw::e_shapeBit;
		flags += b2Draw::e_jointBit;
		m_debugDraw->SetFlags(flags);
	}
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);

	b2Body *groundBody = _world->CreateBody(&groundBodyDef);
	
	b2EdgeShape groundEdge;
	b2FixtureDef boxShapeDef;

	boxShapeDef.shape = &groundEdge;

	//아래
	groundEdge.Set(b2Vec2(0, 0), b2Vec2(winSize.width / PTM_RATIO, 0));
	groundBody->CreateFixture(&boxShapeDef);

	//왼쪽
	groundEdge.Set(b2Vec2(0, 0), b2Vec2(0, winSize.height / PTM_RATIO));
	groundBody->CreateFixture(&boxShapeDef);

	//위
	groundEdge.Set(b2Vec2(0, winSize.height / PTM_RATIO), b2Vec2(winSize.width / PTM_RATIO, winSize.height / PTM_RATIO));
	groundBody->CreateFixture(&boxShapeDef);
	//오른쪽
	groundEdge.Set(b2Vec2(winSize.width / PTM_RATIO, 0), b2Vec2(winSize.width / PTM_RATIO, winSize.height / PTM_RATIO));
	groundBody->CreateFixture(&boxShapeDef);
	//월드 생성 끝

	
	this->initializeBall();
	this->initializeBricks();
	this->initializePaddle();

	myContactListener = new ContactListener();
	_world->SetContactListener((b2ContactListener*)myContactListener);
	
	return true;

	
}

void HelloWorld::startGame(float dt){
	ball->setPosition(Point(160, 240));
	
	isPlaying = true;

	this->schedule(schedule_selector(HelloWorld::gameLogic));
}

void HelloWorld::gameLogic(float dt){
	b2Vec2 vel = ballBody->GetLinearVelocity();
	/*if (vel.x < 0.1&&vel.x>=0){
		vel.x = 0.1;
	}
	if (vel.x<0 && vel.x>-0.1){
		vel.x = -0.1;
	}*/
	if (vel.y < 0.1 && vel.y >= 0){
		vel.y = 0.1;
	}
	if (vel.y<0 && vel.y>-0.1){
		vel.y = -0.1;
	}
	ballBody->SetLinearVelocity(vel);
	_world->Step(dt, 8, 3);
	for (b2Body *b = _world->GetBodyList(); b; b = b->GetNext()){
		if (b->GetUserData() != NULL){
			
				Sprite *spriteData = (Sprite *)b->GetUserData();
				spriteData->setPosition(Point(b->GetPosition().x*PTM_RATIO, b->GetPosition().y*PTM_RATIO));
				spriteData->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
			
		}
	}

	vector<contactData>::iterator pos;
	vector<contactData> destroy_list;
	for (pos = myContactListener->contactList.begin(); pos != myContactListener->contactList.end(); ++pos){
		contactData data = *pos;
		b2Body *bodyA = data.FixtureA->GetBody();
		b2Body *bodyB = data.FixtureB->GetBody();
		if (bodyA->GetUserData() != NULL && bodyB->GetUserData() != NULL){
				Sprite* SpriteA = (Sprite*)bodyA->GetUserData();
				log("SpriteA tag : %d", SpriteA->getTag());
				Sprite* SpriteB = (Sprite*)bodyB->GetUserData();
				log("SpriteB tag : %d", SpriteB->getTag());
				if ((SpriteA->getTag() == 0 && SpriteB->getTag() == 2) || (SpriteA->getTag() == 2 && SpriteB->getTag() == 0)){
					log("brick collision");
					destroy_list.push_back({ data.FixtureA, data.FixtureB });
				}
			}
		}
	contactBallNBrick(destroy_list);
}

void HelloWorld::contactBallNBrick(vector<contactData> destroy_list){
	vector<contactData>::iterator pos;
	for (pos = destroy_list.begin(); pos != destroy_list.end(); ++pos){
		b2Body *bodyA = pos->FixtureA->GetBody();
		b2Body *bodyB = pos->FixtureB->GetBody();
		Sprite* SpriteA = (Sprite*)bodyA->GetUserData();
		Sprite* SpriteB = (Sprite*)bodyB->GetUserData();
		if (SpriteA->getTag() == 2){
			this->removeChild(SpriteA, true);
			_world->DestroyBody(bodyA);
		}
		else{
			this->removeChild(SpriteB, true);
			_world->DestroyBody(bodyB);
		}
	}
}

b2Body* HelloWorld::addNewSprite(Point point, Size size, b2BodyType bodytype, const char* spriteName, int type){
	b2BodyDef bodyDef;
	bodyDef.type = bodytype;
	bodyDef.position.Set((float)point.x / PTM_RATIO, (float)point.y / PTM_RATIO);

	if (spriteName){
		if (strcmp(spriteName, "test") == 0){

		}
		else{
			Sprite* sprite = Sprite::create(spriteName);
			sprite->setPosition(point);
			this->addChild(sprite);
			bodyDef.userData = sprite;
		}
	}
	b2Body *body = _world->CreateBody(&bodyDef);
	
	b2FixtureDef fixtureDef;
	b2PolygonShape dynamicBox;
	b2CircleShape circle;
	
	if (type == 0){
		dynamicBox.SetAsBox((float)size.width / 2 / PTM_RATIO, (float)size.height / 2 / PTM_RATIO);
		fixtureDef.shape = &dynamicBox;
	}
	else{

	}

	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 0.0f;

	body->CreateFixture(&fixtureDef);

	return body;
}
void HelloWorld::initializeBricks(){
	int count = 0;

	for (int y = 0; y < BRICKS_HEIGHT; y++){
		for (int x = 0; x < BRICKS_WIDTH; x++){
			Sprite* bricks = Sprite::createWithTexture(texture, Rect(0, 0, 64, 40));
		//color
			switch (count++ % 4){
			case 0:
				bricks->setColor(Color3B(255, 255, 255));
				break;
			case 1:
				bricks->setColor(Color3B(255, 0, 0));
				break;
			case 2:
				bricks->setColor(Color3B(0, 255, 0));
				break;
			case 3:
				bricks->setColor(Color3B(0, 0, 255));
				break;
			default:
				break;
			}

			bricks->setPosition(Point(x * 64 + 32, (y * 40) + 280));
			bricks->setTag(2);
			this->addChild(bricks);

			b2BodyDef bodyDef;
			bodyDef.type = b2_staticBody;
			bodyDef.position.Set(((float)(x * 64) + 32)/PTM_RATIO, ((float)(y * 40) + 280)/PTM_RATIO);
			bodyDef.userData = bricks;
			b2Body* body1 = _world->CreateBody(&bodyDef);
			b2PolygonShape staticBox;
			//staticBox.SetAsBox((float)(32) / PTM_RATIO, (float)(20) / PTM_RATIO);
			staticBox.SetAsBox(((float)bricks->getContentSize().width / 2) / PTM_RATIO, (float)(bricks->getContentSize().height / 2) / PTM_RATIO);
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &staticBox;
			fixtureDef.density = 1.0f;

			body1->CreateFixture(&fixtureDef);

			//targets.pushBack(bricks);
		}
	}
}

void HelloWorld::initializeBall(){
	/*auto tex = Director::getInstance()->getTextureCache()->addImage("Images/ballImage.png");
	ball = Sprite::createWithTexture(tex,Rect(0,0,256,256));*/
	ball = Sprite::create("Images/ballImage.png");
	Size s = ball->getContentSize();
	log("size width : %f", s.width);
	ball->setScale((float)28/160);
	ball->setPosition(Point(160,240));
	ball->setTag(0);
	this->addChild(ball);
	
	b2BodyDef bodyDefBall;
	bodyDefBall.type = b2_dynamicBody;
	bodyDefBall.position.Set((float)160 / PTM_RATIO, (float)240 / PTM_RATIO);
	bodyDefBall.linearVelocity = (b2Vec2(0, -10.0f));
	bodyDefBall.userData = ball;
	ballBody = _world->CreateBody(&bodyDefBall);

	b2CircleShape circleBall;
	circleBall.m_radius =(float) 12 / PTM_RATIO;
	
	b2FixtureDef fixtureDefBall;
	fixtureDefBall.shape = &circleBall;
	fixtureDefBall.density = 1.0f;
	fixtureDefBall.friction = 0.0f;
	fixtureDefBall.restitution = 1.0f;
	
	ballBody->CreateFixture(&fixtureDefBall);
}

void HelloWorld::initializePaddle(){
	paddle = Sprite::createWithTexture(texture, Rect(0, 0, 80, 10));
	paddle->setColor(Color3B(255, 255, 0));
	paddle->setPosition(Point(160, 50));
	paddle->setTag(1);
	this->addChild(paddle);
	
	b2BodyDef bodyDefPaddle;
	bodyDefPaddle.type = b2_kinematicBody;
	bodyDefPaddle.position.Set((float)160/PTM_RATIO, (float)50/PTM_RATIO);
	bodyDefPaddle.userData = paddle;
	bodyDefPaddle.fixedRotation = true;

	paddleBody = _world->CreateBody(&bodyDefPaddle);
	b2PolygonShape paddleBox;
	paddleBox.SetAsBox(((float)paddle->getContentSize().width / 2) / PTM_RATIO,
		(float)(paddle->getContentSize().height / 2) / PTM_RATIO);

	b2FixtureDef fixturePaddleDef;
	fixturePaddleDef.shape = &paddleBox;
	fixturePaddleDef.density = 10000000.0f;
	

	paddleBody->CreateFixture(&fixturePaddleDef);

}

bool HelloWorld::onTouchBegan(Touch* touch, Event* event){
	if (!isPlaying){
		return true;
	}
	auto touchPoint = touch->getLocation();
	
	/*bDrag = true;
	b2MouseJointDef md;
	md.bodyA = gbody;
	md.bodyB = paddleBody;
	md.target.Set(paddleBody->GetPosition().x, paddleBody->GetPosition().y);
	md.maxForce = 300.0*paddleBody->GetMass();

	mouseJoint = (b2MouseJoint *)_world->CreateJoint(&md);*/
	isPaddleTouched = true;
	return true;
}

void HelloWorld::onTouchMoved(Touch* touch, Event* event){
	
	if (isPaddleTouched){
		auto touchPoint = touch->getLocation();
		if (touchPoint.x < 40){
			touchPoint.x = 40;
		}
		if (touchPoint.x > 280){
			touchPoint.x = 280;
		}
		auto tempPoint = paddleBody->GetPosition();
	//	paddleBody->SetLinearVelocity(b2Vec2((float)(touchPoint.x - tempPoint.x)/PTM_RATIO,0));
		paddleBody->SetTransform(b2Vec2((float)touchPoint.x / PTM_RATIO,(float) 50 / PTM_RATIO),paddleBody->GetAngle());
	}
	/*if (isPaddleTouched){
		auto touchPoint = touch->getLocation();
		

		Point newLocation = Point(touchPoint.x, paddle->getPosition().y);
		paddle->setPosition(newLocation);
	}*/

}

void HelloWorld::onTouchEnded(Touch *touch, Event * event){
	if (isPaddleTouched){
		//_world->DestroyJoint(mouseJoint);
	//	mouseJoint = NULL;

	//	paddleBody->SetAwake(true);
	}
	isPaddleTouched = false;
}

void HelloWorld::onEnter(){
	Layer::onEnter();

	auto listener = EventListenerTouchOneByOne::create();

	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void HelloWorld::onExit(){
	//_eventDispatcher->removeEventListenersForType(EventListener::Type::TOUCH_ONE_BY_ONE);
	Layer::onExit();
}

HelloWorld::~HelloWorld(){
	delete myContactListener;
	delete _world;
	_world = NULL;
}
//바디를 터치 햇는지
b2Body* HelloWorld::getBodyAtTab(Point p){
	return NULL;
}
//box2d debug
void HelloWorld::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags){
	Layer::draw(renderer, transform, flags);

	_customCmd.init(_globalZOrder, transform, flags);
	_customCmd.func = CC_CALLBACK_0(HelloWorld::onDraw, this, transform, flags);
	renderer->addCommand(&_customCmd);
}

void HelloWorld::onDraw(const Mat4 &transform, uint32_t flags){
	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when setting matrix stack");
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	GL::enableVertexAttribs(cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION);
	_world->DrawDebugData();
	CHECK_GL_ERROR_DEBUG();

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}