//Created by 16007006
//Provides a GameObject class which can then be modified and controlled by different components.
//GO's have an initialise component to allow gameplay progammer to set up object outside of constructor.

#pragma once
#include "myinputs.h"
#include "components.h"
#include <list>
#include <string>

//Forward declare - only referenced, never used.
class ObjectManager; 

class GameObject
{
private:
	//Components
	// Render and Collission are core components which need specific access permissions
	// They are still added to the list & updated, but can be specifically called upon
	//RenderComponent*    pRenderComponent; 
	CollisionComponent* pCollisionComponent;
	RenderComponent*    pRenderComponent;
	ObjectManager*      pObjectManager;
protected:	
	std::list<Component*> pComponentList;
public:
	//Variables	
	bool     active;       //Is GO currently on screen
	Vector2D position;     //Position of GO
	float    angle;        //Angle of GO - Default is 0
	Vector2D velocity;     //Velocity of GO - could be moved to PhysicsComponent
	
	//Functions
	GameObject(ObjectManager* pObjectManager);// Constructor	
	~GameObject();// Destructor
	void Initialise(RenderComponent* pRenderComponent, CollisionComponent* pCollisionComponent, 
					Vector2D position, Vector2D velocity);
	void Update();
	bool isActive(); //Indicates if object is active. Keeps actual variable protected.
	void AddComponent(Component* newComponent); //Adds component pointer to pComponentList
	CollisionComponent* GetCollision(); //Returns a pointer to the GO's collision component
	RenderComponent*    GetRender();    //Returns a pointer to the GO's render component
	ObjectManager*      GetOM();        //Returns a pointer to the ObjectManager which created this GO
};