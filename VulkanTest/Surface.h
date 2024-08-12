#pragma once
#include <vulkan/vulkan.h>
#include <memory>

class GraphicsInstance;
class Window;

class Surface {
public:
	Surface(const std::unique_ptr<GraphicsInstance>& instance, const std::unique_ptr<Window>& window);
	~Surface();

	const VkSurfaceKHR getSurface() const { return surface; }

private:
	/// <summary>
	/// abstraction of the window surface to be rendered to, required for on-screen rendering
	/// </summary>
	VkSurfaceKHR surface;

	const std::unique_ptr<GraphicsInstance>& instanceRef;
};

