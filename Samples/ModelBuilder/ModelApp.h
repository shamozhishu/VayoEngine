#pragma once

class ModelApp : public App, public Singleton<ModelApp>
{
public:
	ModelApp(const Root::Config& config);
	bool startup();
	void cleanup();
	bool keyClicked(const tagKeyInput& keyInput);

private:
	PROPERTY_R(OrbitCamera*, m_cam, OrbitCamera)
};
