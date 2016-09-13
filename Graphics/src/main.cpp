#include <BaseApplication.h>
#include <RenderingGeometryApplication.h>

int main() 
{	
	BaseApplication * app = new RenderingGeometryApplication();
	if (app->startup())
		app->run();
	app->shutdown();
	return 0;
}