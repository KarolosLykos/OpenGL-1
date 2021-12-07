#include "GLCore.h"
#include "BatchRenderingLayer.h"

using namespace GLCore;

class BatchRendering : public Application
{
public:
	BatchRendering()
		:Application("OpenGL Batch Rendering")
	{
		PushLayer(new BatchRenderingLayer());
	}
};

int main()
{
	std::unique_ptr<BatchRendering> app = std::make_unique<BatchRendering>();
	app->Run();
}