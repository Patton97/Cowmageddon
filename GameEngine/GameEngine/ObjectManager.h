//Created by 16007006
//Controls the creation, updating, and deletion of game objects
//Also provides functionality to check for collisions

#pragma once
#include "vector2d.h"
#include <list>

class GameObject;

class ObjectManager
{
private:
	std::list<GameObject*> pObjectList; //The list of all GameObjects
	void AddObject(GameObject* pObject);
public:	
	//Functions
	ObjectManager();  // Constructor
	~ObjectManager(); // Destructor
	GameObject* CreateUFO(Vector2D position);
	GameObject* CreateRock(Vector2D position, Vector2D velocity, float rotation);
	GameObject* CreateCow(Vector2D position, Vector2D velocity, float rotation);
	GameObject* CreateBullet(Vector2D position, Vector2D velocity);
	void UpdateAll();
	void CheckAllCollisions();
	void DeleteInactive();
	void DeleteAll();
};