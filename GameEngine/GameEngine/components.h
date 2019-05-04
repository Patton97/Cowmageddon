//Created by 16007006
//Required for advanced element: flexible/generic component system
//Provides the behavioral functionality of individual components, as well as 
//the main abstract superclass for all components

#pragma once
#include "mydrawengine.h"
#include "mysoundengine.h"
#include "gametimer.h"
#include "gamecode.h"

//Foward-declarations - only referenceed, never used
class GameObject;
class ObjectManager;

/**************************************************
 * COMPONENTS *************************************
 **************************************************/

class Component
{
protected:
	GameTimer timer; //Timer tracking frametime
public:
	Component(GameObject* pOwner);
	~Component();
	GameObject* pOwner;
	virtual void Update() = 0;
};

/********************
 * INPUT COMPONENTS *
 ********************/

class InputComponent : public Component
{
protected:
	SoundIndex moveSound;
	SoundIndex shootSound;
	float speed = 0.0f;    //The speed at which the GameObject will move
	float bulletDelay = 5.0f;//Prevents bullet spam every frame
public:
	// Constructor
	InputComponent(GameObject* pOwner, float speed);
	// Destructor
	virtual ~InputComponent();
	
	void Update() override;
};

/*********************
 * PHYSICS COMPONENT *
 *********************/

class PhysicsComponent : public Component
{
private:
	float rotation;
public:
	// Constructor
	PhysicsComponent(GameObject* pOwner, float rotation = 0.0f);
	// Destructor
	~PhysicsComponent();
	//Functions
	void Update() override;
};

/********************
 * RENDER COMPONENT *
 ********************/

class RenderComponent : public Component
{
private:
	PictureIndex img;
	float scale;
	float transparency;
public:
	// Constructor
	RenderComponent(GameObject* pOwner, wchar_t* filename, float scale = 1.0f, float transparency = 0.0f);
	// Destructor
	~RenderComponent();
	//Functions
	void Update() override;
	void LoadImg(wchar_t* filename);
	float GetScale();	
};

/**************************************************
 * COLLISION COMPONENTS ***************************
 **************************************************/

//CollisionComponents are either BoxCollisionComponent, CircleCollisionComponent, or a subclass of one of these types.
//  This decides the collision shape delivered to the ObjectManager when checking intersections
//  Super Collision Components such as BoxCollisionComponent and CircleCollisionComponent will destroy their
//  owner GameObject regardless of what they have collided with.
//  Subclasses of these root components may overwrite this with their own directed behaviour

/****************************
 * Root Collision Component *
 ****************************/

class CollisionComponent : public Component
{
public:
	CollisionComponent(GameObject* pOwner); //Constructor
	virtual ~CollisionComponent();          //Destructor
	//Functions
	virtual void ProcessCollision(GameObject* otherObject);
	virtual IShape2D* GetShape() = 0; //Every CollisionComponent will return a shape, 
	                                  //but the abstract root cannot assume
};

/*******************************
 * Shaped Collision Components *
 *******************************/

//Box-collision uses a rectangle
class BoxCollisionComponent : public CollisionComponent
{
private:
	Rectangle2D shape;
	float width, height;
public:
	BoxCollisionComponent(GameObject* pOwner, float width, float height); //Constructor
	~BoxCollisionComponent();                  //Destructor
	IShape2D* GetShape() override; //Overrides the abstract superclass to return a rectangle
	void Update() override; //DEBUG ONLY - Visualises collision shape
};
//Circle-collision uses a circle
class CircleCollisionComponent : public CollisionComponent
{
private:
	Circle2D shape;
	float radius;
public:
	CircleCollisionComponent(GameObject* pOwner, float radius); //Constructor
	~CircleCollisionComponent();                  //Destructor
	IShape2D* GetShape() override; //Overrides the abstract superclass to return a circle
	void Update() override; //DEBUG ONLY - Visualises collision shape
};

/****************************************
 * Object-Specific Collision Components *
 ****************************************/

class RockCollisionComponent : public CircleCollisionComponent
{
public:
	RockCollisionComponent(GameObject* pOwner, float radius); //Constructor
	virtual ~RockCollisionComponent();          //Destructor
	//Functions
	void ProcessCollision(GameObject* otherObject) override;
}; 

class UFOCollisionComponent : public CircleCollisionComponent
{
public:
	// Constructor
	UFOCollisionComponent(GameObject* pOwner, float radius);
	// Destructor
	virtual ~UFOCollisionComponent();
	void ProcessCollision(GameObject* otherObject) override;
};

class BulletCollisionComponent : public BoxCollisionComponent
{
public:
	// Constructor
	BulletCollisionComponent(GameObject* pOwner, float width, float height);
	// Destructor
	virtual ~BulletCollisionComponent();
	void ProcessCollision(GameObject* otherObject) override;
};

class CowCollisionComponent : public BoxCollisionComponent
{
public:
	CowCollisionComponent(GameObject* pOwner, float width, float height); //Constructor
	virtual ~CowCollisionComponent();          //Destructor
	void ProcessCollision(GameObject* otherObject) override;
};

/**************************************************
 * EXPIRATION COMPONENT ***************************
 **************************************************/

class ExpirationComponent : public Component
{
private:
	bool recyclable;
public:
	//Constructor
	ExpirationComponent(GameObject* pOwner, bool recyclable = false);
	//Destructor
	~ExpirationComponent();
	void Update() override;
	void Recycle();
};