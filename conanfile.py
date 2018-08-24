from conans import ConanFile, CMake


class IRMConan(ConanFile):
    name = "irm"
    version = "0.1"
    url = "https://github.com/elshize/irm"
    license = "MIT"
    description = "Information Retrieval evaluation metrics."
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = ("LICENSE", "README.md", "include/*", "tools/*",
                       "cmake/*", "CMakeLists.txt", "tests/*")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def requirements(self):
        self.requires("gtest/1.8.0@conan/stable")
        self.requires("CLI11/1.6.0@cliutils/stable")

    def package(self):
        cmake = CMake(self)
        cmake.install()
