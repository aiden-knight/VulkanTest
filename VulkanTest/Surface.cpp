#include "Surface.h"
#include "GraphicsInstance.h"
#include "Window.h"
#include "Debug.h"

Surface::Surface(const std::unique_ptr<GraphicsInstance>& instance, const std::unique_ptr<Window>& window) : instanceRef(instance) {
	if (window->createWindowSurface(instance, &surface) != VK_SUCCESS) {
		Debug::exception("failed to create window surface");
	}
}

Surface::~Surface() {
	vkDestroySurfaceKHR(instanceRef->getInstance(), surface, nullptr);
}
