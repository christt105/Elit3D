#pragma once
#include "Panel.h"
class p1Scene :
	public Panel
{
public:
	p1Scene(bool start_enabled = true);
	void InitFrameBuffer();
	~p1Scene();

	void Update() override;

	void SelectFrameBuffer();
	void DeselectFrameBuffer();

private:
	enum FBOIDS
	{
		FBO,
		FBO_MS,
		TEXTURE,
		TEXTURE_MS,
		RBO,
		RBO_MS
	};
	unsigned int ID[8];

	ImVec2 window_size;
};

