#pragma once
#include "Panels/Base/Panel.h"

class Object;
class r1Object;
class r1Tileset3d;
class Viewport;

class p1Objects :
	public Panel
{
public:
	p1Objects(bool start_active = true, bool appear_mainmenubar = true, bool can_close = true);
	~p1Objects();

	void Start() override;
	void Update() override;

	void ModalEditTileset();

	bool IsOnTileset(const uint64_t& uid) const;

	void Header();

	uint64_t GetObjectSelected() const;

	int selected = -1;
	r1Tileset3d* tileset = nullptr;

private:
	void TreeNode(Object* obj);

	std::vector<Object*> to_destroy;
	r1Object* objSelected = nullptr;

	Viewport* viewport = nullptr;

	char bufname[30];
};

