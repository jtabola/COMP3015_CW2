#include "helper/scene.h"
#include "helper/scenerunner.h"
#include "scenebasic_uniform.h"
#include "helper/globals.h"

bool stopRotation = true;

void toggleRotation() {
	stopRotation = !stopRotation;
}

int main(int argc, char* argv[])
{
	SceneRunner runner("Skull on fire");

	std::unique_ptr<Scene> scene;

	scene = std::unique_ptr<Scene>(new SceneBasic_Uniform());

	return runner.run(*scene);
}