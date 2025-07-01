from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout

class TestKthTargetsConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        # When using the actual package, you would do:
        # self.requires("kth/0.68.2", transitive_headers=True, transitive_libs=True)
        
        # For local testing with the editable package:
        self.requires("kth/0.68.2", transitive_headers=True, transitive_libs=True)

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.20]")
