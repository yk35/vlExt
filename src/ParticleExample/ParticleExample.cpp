#include <vlCore/VisualizationLibrary.hpp>
#include <vlGraphics/Applet.hpp>
#include <vlExtParticle/Particle.hpp>
#include <vlGLUT/GLUTWindow.hpp>
#include <vlCore/Colors.hpp>
#include <vlCore/Time.hpp>


class ParticleApplet : public vl::Applet
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
		mParticle = new vlExt::ParticleSystem(1000000);
		auto emitter = new vlExt::ParticleEmitter();
		emitter->setEmitRate(100000);
		{
			auto boxgen = new vlExt::BoxPosGen();
			boxgen->setPos(vl::vec3(0, 0, 0));
			boxgen->setMaxStartPosOffset(vl::vec3(0, 0, 0));
			emitter->addGenerator(boxgen);
			auto colgen = new vlExt::BasicColorGen();
			colgen->setStartCol(vl::vec4(1, 1, 1, 1), vl::vec4(1, 1, 1, 1));
			colgen->setEndCol(vl::vec4(1, 1, 1, 0.0), vl::vec4(1, 1, 1, 0.0));
			emitter->addGenerator(colgen);
			auto timegen = new vlExt::BasicTimeGen();
			timegen->setLifeTime(25, 35);
			emitter->addGenerator(timegen);
			auto velgen = new vlExt::ConeVelGen();
			velgen->setVelocity(1, 10);
			velgen->setDirectionBaseAngle(0, 0);
			velgen->setDirectionRange(vl::radians(10.f), vl::radians(10.f));
			emitter->addGenerator(velgen);

		}
		mParticle->emitters().push_back(emitter);
		auto updater = new vlExt::EulerUpdater();
		updater->setGlobalAcceleration(vl::vec4(0, 0, 0, 0));
		mParticle->updaters().push_back(updater);
		auto colUpdater = new vlExt::BasicColorUpdater();
		mParticle->updaters().push_back(colUpdater);
		mParticle->updaters().push_back(new vlExt::BasicTimeUpdater());
		auto tex = new vl::Texture();
		tex->prepareTexture2D(
			vl::loadImage("../data/particle.png").get(),
			vl::ETextureFormat::TF_RGBA,
			false
			);
		tex->getTexParameter()->setMinFilter(vl::ETexParamFilter::TPF_LINEAR);
		tex->getTexParameter()->setMagFilter(vl::ETexParamFilter::TPF_LINEAR);
		auto trans = new vl::Transform();
		rendering()->as<vl::Rendering>()->transform()->addChild(trans);
		mParticle->prepare(tex, trans);
		// install our scene manager, we use the SceneManagerActorTree which is the most generic
		vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
		scene_manager->setCullingEnabled(false);
		rendering()->as<vl::Rendering>()->sceneManagers()->push_back(scene_manager.get());
		// add the cube to the scene using the previously defined effect and transform 
		scene_manager->tree()->addActor(mParticle->actor());
	}
private:
	vl::ref<vlExt::ParticleSystem> mParticle;
};


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	vl::VisualizationLibrary::init();
	vl::OpenGLContextFormat format;
	format.setDoubleBuffer(true);
	format.setDepthBufferBits(24);
	format.setRGBABits(8, 8, 8, 8);
	format.setFullscreen(false);
	vl::ref<ParticleApplet> applet = new ParticleApplet();
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
	w->initGLUTWindow("particle", format, x, y, width, height);
	glutMainLoop();


	vl::VisualizationLibrary::shutdown();
	return 0;
}