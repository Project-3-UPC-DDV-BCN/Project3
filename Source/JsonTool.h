#ifndef __MODULEJSON__H__
#define __MODULEJSON__H__

#include "Module.h"
#include "Parson\parson.h"

#include <list>

class JSON_File
{
public:
	JSON_File(JSON_Value* value, JSON_Object* object, const char* path);
	JSON_File(JSON_File& file);
	~JSON_File();

	void SetString(const char* name, const char* str);
	const char* GetString(const char* name);

	const char* GetString(const char* name, const char* fallback);

	void SetBool(const char* name, bool b);
	bool GetBool(const char* name);

	bool GetBool(const char* name, bool fallback);

	void SetNumber(const char* name, double num);
	double GetNumber(const char* name);

	double GetNumber(const char* name, double fallback);

	void AddObject(const char* name);
	void RemoveObject(const char* name);

	bool ChangeObject(const char* name);

	bool ChangeObjectFromArray(const char* array, unsigned int index);

	void RootObject();

	void AddArray(const char* name);
	void AddArraySection(const char* name);
	bool FindInsideArray(const char* val, int id, json_value_type type);
	bool MoveInsideArray(const char* val, int id);
	bool MoveTo(const char* move);
	bool MoveToInsideArray(const char* move, int i);
	void ClearArray(const char* name);

	int ArraySize(const char* array);
	void ArrayAddString(const char* array, const char* str);
	void ArrayAddBool(const char* array, bool b);
	void ArrayAddNumber(const char* array, double num);
	void ArrayAddObject(const char* array);

	const char* ArrayGetString(const char* array, unsigned int index, const char* fallback = "Invalid");
	bool ArrayGetBool(const char* array, unsigned int index, bool fallback = false);
	double ArrayGetNumber(const char* array, unsigned int index, double fallback = 0);


	const char* GetPath();
	void Save();
	void CleanUp();


private:
	JSON_Value * val = nullptr;
	JSON_Object* obj = nullptr;
	JSON_Object* root = nullptr;
	std::string  path;
};

class JSONTool
{
public:
	JSONTool();
	~JSONTool();

	bool CleanUp();

	JSON_File* LoadJSON(const char* path);
	JSON_File* CreateJSON(const char* path);
	void UnloadJSON(JSON_File * son);

private:
	std::list<JSON_File*> jsons;
};


#endif