#include <vlCore/VisualizationLibrary.hpp>
#include <vlGraphics/Applet.hpp>
#include <vlGLUT/GLUTWindow.hpp>
#include <vlCore/Colors.hpp>
#include <vlCore/Time.hpp>
#include <vlExtLoadWriter/ioAssimp.hpp>
#include <vlExtUtility/CallbackUtility.hpp>

#include <iostream>

class LoadWriterApplet : public vl::Applet
{

public:
	vl::Time mFPSTimer;
	virtual void updateScene()
	{
		if (mFPSTimer.elapsed() > 2)
		{
			mFPSTimer.start();
			openglContext()->setWindowTitle(vl::Say("[%.1n] %s") << fps() << appletName() + " - " + vl::String("VL ") + vl::VisualizationLibrary::versionString());
			vl::Log::print(vl::Say("FPS=%.1n\n") << fps());
		}
	}
	virtual void initEvent()
	{
		mFPSTimer.start();
		vl::ref<vl::ResourceDatabase> db;
		db = vlExt::loadAssimp(modelFile);
		if (!db.get())
		{
			db = vl::loadResource(modelFile);
		}
		std::vector<vl::ref<vl::Actor>> actors;
		db->extract(actors);
		vl::ref<vl::Light> light = new vl::Light();
		light->setPosition(vl::fvec4(1, 1, 1, 0));
		light->setAmbient(vl::fvec4(0, 0, 0, 1));
		light->setDiffuse(vl::fvec4(1, 1, 1, 1));
		light->setSpecular(vl::fvec4(0, 0, 0, 1));
		for (auto actor : actors)
		{
			actor->effect()->shader()->setRenderState(light.get(), 0);
			sceneManager()->tree()->addActor(actor.get());
		}
		sceneManager()->tree()->computeAABB();
		rendering()->as<vl::Rendering>()->setNearFarClippingPlanesOptimized(true);
	}

	vl::String modelFile;
};


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "usage: " << argv[0] << " modelfile" << std::endl;
		exit(EXIT_FAILURE);
	}
	glutInit(&argc, argv);
	vl::VisualizationLibrary::init();
	vl::OpenGLContextFormat format;
	format.setDoubleBuffer(true);
	format.setDepthBufferBits(24);
	format.setRGBABits(8, 8, 8, 8);
	format.setFullscreen(false);
	vl::ref<LoadWriterApplet> applet = new LoadWriterApplet();
	applet->modelFile = argv[1];
	applet->initialize();
	auto w = new vlGLUT::GLUTWindow();
	w->addEventListener(applet.get());
	/* target the window so we can render on it */
	applet->rendering()->as<vl::Rendering>()->renderer()->setFramebuffer(w->framebuffer());
	/* black background */
	applet->rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor(vl::black);
	/* define the camera position and orientation */
	vl::vec3 eye = vl::vec3(0, 0, 35); // camera position
	vl::vec3 center = vl::vec3(0, 0, 0);   // point the camera is looking at
	vl::vec3 up = vl::vec3(0, 1, 0);   // up direction
	vl::mat4 view_mat = vl::mat4::getLookAt(eye, center, up);
	applet->rendering()->as<vl::Rendering>()->camera()->setViewMatrix(view_mat);
	applet->rendering()->as<vl::Rendering>()->camera()->setNearPlane(0.001);
	applet->rendering()->as<vl::Rendering>()->camera()->setFarPlane(1000);
	/* Initialize the OpenGL context and window properties */
	int x = 0;
	int y = 0;
	int width = 512;
	int height = 512;
	w->initGLUTWindow("loadwriter", format, x, y, width, height);
	glutMainLoop();


	vl::VisualizationLibrary::shutdown();
	return 0;
}