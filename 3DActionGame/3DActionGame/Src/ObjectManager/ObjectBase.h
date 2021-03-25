﻿#ifndef OBJECT_BASE_H_
#define OBJECT_BASE_H_

class ObjectBase
{
public:
	ObjectBase(){}

	~ObjectBase(){}

	
	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
	
	virtual void Init() = 0;
	virtual void Release() = 0;
	
};

#endif