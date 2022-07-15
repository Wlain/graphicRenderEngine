# simpleRenderEngine
This is a simple render engine based on OpenGL in MAC-OS, which use C++17

# dependencies
* glfw
* glew
* glm
* spdlog
* imgui
* stb
* json11
* lua
* sol2
* box2d
* bullet3

example:

small tips:
* One-click replacement gl function
sublime: `gl[A-Z](.*)\); -> CHECK_GL(gl$1\)); `;