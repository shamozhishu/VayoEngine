#pragma once

class TessgridApp : public App, public Singleton<TessgridApp>
{
public:
	TessgridApp(Root::Config* config);
	bool startup();
	void cleanup();
	bool keyClicked(const tagKeyInput& keyInput);

private:
	PROPERTY_R(OrbitCamera*, m_cam, OrbitCamera)
};
