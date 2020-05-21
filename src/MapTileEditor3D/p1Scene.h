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
	unsigned int tex_buf = 0u;
	unsigned int fbo = 0u;
	unsigned int rbo = 0u;

	ImVec2 window_size;
};

