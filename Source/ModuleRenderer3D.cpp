#include "Headers.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "SDL_opengl.h"
#include "XMLNode.h"
#include "ModuleXML.h"
#include "ModuleLayers.h"

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	cameras = app->camera;
	Console::S_Log("Initializing OpenGL 3.3");
	
	//Create context
	context = SDL_GL_CreateContext(app->window->window);

	Console::S_Log("Initializing Glew.");

	GLenum er = glewInit();

	SDL_GL_MakeCurrent(app->window->window, context);

	if(ret == true)
	{
		//Use Vsync	
		XMLNode renderNode = app->xml->GetConfigXML().FindChildBreadth("renderer");
		isVSync = renderNode.node.child("vsync").attribute("value").as_bool();
		ToggleVSync(isVSync);
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		//lights[0].ref = GL_LIGHT0;
		//lights[0].ambient.Set(0.5f, 0.5f, 0.5f, 0.5f);
		//lights[0].diffuse.Set(1.0f, 1.0f, 1.0f, 0.5f);
		//lights[0].SetPos(0.0f, 0.0f, 2.5f);
		//lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);

		// Enable opacity
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	modelRender.Init();

	// Projection matrix for
	OnResize(app->window->width, app->window->height);

	return ret;
}

// PreUpdate: clear buffer
UpdateStatus ModuleRenderer3D::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UpdateStatus::UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
UpdateStatus ModuleRenderer3D::PostUpdate()
{
	if (cameras->sceneCamera.active)
	{
		cameras->sceneCamera.frameBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		cameras->currentDrawingCamera = &cameras->sceneCamera;

		Application::Instance()->layers->DrawLayers();
		modelRender.Draw();
	}

	if (cameras->activeGameCamera != nullptr && cameras->activeGameCamera->active)
	{
		cameras->activeGameCamera->frameBuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		cameras->currentDrawingCamera = cameras->activeGameCamera;

		Application::Instance()->layers->DrawLayers();
		modelRender.Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Application::Instance()->layers->DrawEditor();

	SDL_GL_SwapWindow(app->window->window);

	return UpdateStatus::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	XMLNode configNode = app->xml->GetConfigXML();

	configNode.node.child("renderer").child("vsync").attribute("value").set_value(isVSync);

	configNode.Save();

	LOG("Deleting context");

	SDL_GL_DeleteContext(context);

	LOG("Context deleted");

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	cameras->RequestFrameBufferRegen(&cameras->sceneCamera, width, height);

	app->window->width = width;
	app->window->height = height;
}

void ModuleRenderer3D::ToggleVSync(bool isOn)
{
	isVSync = isOn;
	SDL_GL_SetSwapInterval(isVSync);
}

void ModuleRenderer3D::ToggleOpenGLSystem(bool enable, int openGLMacro)
{
	if (enable) glEnable(openGLMacro);
	else glDisable(openGLMacro);
}

void ModuleRenderer3D::ToggleOpenGLWireframe(bool enable)
{
	if (enable)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}