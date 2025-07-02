# Copyright (c) 2016-2024 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import os
from conan import ConanFile
from conan.tools.build.cppstd import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy #, apply_conandata_patches, export_conandata_patches, get, rm, rmdir
from kthbuild import option_on_off, march_conan_manip, pass_march_to_compiler
from kthbuild import KnuthConanFileV2

required_conan_version = ">=2.0"

class Secp256k1Conan(KnuthConanFileV2):
    name = "secp256k1"
    license = "http://www.boost.org/users/license.html"
    url = "https://github.com/k-nuth/secp256k1"
    description = "Optimized C library for EC operations on curve secp256k1"
    settings = "os", "compiler", "build_type", "arch"
    package_type = "library"

    #TODO(fernando): See what to do with shared/static option... (not supported yet in Cmake)

    options = {"shared": [True, False],
               "fPIC": [True, False],
               "enable_experimental": [True, False],
               "enable_endomorphism": [True, False],

               "ecmult_window_size": ["ANY"],
               "ecmult_gen_precision": ["ANY"],

               "enable_ecmult_static_precomputation": [True, False],
               "enable_module_ecdh": [True, False],
               "enable_module_schnorr": [True, False],
               "enable_module_recovery": [True, False],
               "enable_module_multiset": [True, False],
               "benchmark": [True, False],
               "tests": [True, False],
               "openssl_tests": [True, False],
               "bignum_lib": [True, False],

               "march_id": ["ANY"],
               "march_strategy": ["download_if_possible", "optimized", "download_or_fail"],

               "verbose": [True, False],
               "cxxflags": ["ANY"],
               "cflags": ["ANY"],
               "cmake_export_compile_commands": [True, False],

            #  "with_bignum": ["conan", "auto", "system", "no"]
            #TODO(fernando): check what to do with with_asm, with_field and with_scalar
            # Check CMake files and Legacy and bitcoin core makefiles
            #  "with_asm": ['x86_64', 'arm', 'no', 'auto'],
            #  "with_field": ['64bit', '32bit', 'auto'],
            #  "with_scalar": ['64bit', '32bit', 'auto'],
            #  "with_bignum": ['gmp', 'no', 'auto'],
    }

    default_options = {
        "shared": False,
        "fPIC": True,
        "enable_experimental": False,
        "enable_endomorphism": False,

        "ecmult_window_size": 15,
        "ecmult_gen_precision": 4,

        "enable_ecmult_static_precomputation": False,
        "enable_module_ecdh": False,
        "enable_module_schnorr": True,
        "enable_module_recovery": True,
        "enable_module_multiset": True,
        "benchmark": False,
        "tests": True,
        "openssl_tests": True,
        "bignum_lib": True,

        "march_strategy": "download_if_possible",

        "verbose": False,
        "cmake_export_compile_commands": False

        # "with_bignum": conan"
        # "with_asm": 'auto',
        # "with_field": 'auto'"
        # "with_scalar": 'auto'"
        # "with_bignum": 'auto'"
    }

    # exports = "conan_*", "ci_utils/*"
    exports_sources = "src/*", "include/*", "CMakeLists.txt", "ci_utils/cmake/*", "cmake/*", "secp256k1Config.cmake.in", "contrib/*", "test/*"

    @property
    def bignum_lib_name(self):
        if self.options.bignum_lib:
            if self.settings.os == "Windows":
                return "mpir"
            else:
                return "gmp"
        else:
            return "no"

    def requirements(self):
        if self.options.bignum_lib:
            if self.settings.os == "Windows":
                self.requires("mpir/3.0.0", transitive_headers=True, transitive_libs=True)
            else:
                self.requires("gmp/6.3.0", transitive_headers=True, transitive_libs=True)

    def validate(self):
        KnuthConanFileV2.validate(self, pure_c=True)
        if self.info.settings.compiler.cppstd:
            check_min_cppstd(self, "20")

    def config_options(self):
        # ConanFile.config_options(self)
        KnuthConanFileV2.config_options(self)

    def configure(self):
        # del self.settings.compiler.libcxx       #Pure-C Library
        KnuthConanFileV2.configure(self, pure_c=True)

    def package_id(self):
        KnuthConanFileV2.package_id(self)
        # v = str(self.info.settings.compiler.version)
        # self.output.info(f'********* version: {v}')

        self.info.options.benchmark = ["ANY"]
        self.info.options.openssl_tests = ["ANY"]

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = self.cmake_toolchain_basis(pure_c=True)
        # tc.variables["CMAKE_VERBOSE_MAKEFILE"] = True

        tc.variables["ENABLE_BENCHMARK"] = option_on_off(self.options.benchmark)
        tc.variables["ENABLE_TESTS"] = option_on_off(self.options.tests)
        tc.variables["ENABLE_OPENSSL_TESTS"] = option_on_off(self.options.openssl_tests)
        # tc.variables["ENABLE_BENCHMARK"] = option_on_off(self.benchmark)
        # tc.variables["ENABLE_TESTS"] = option_on_off(self.tests)
        # tc.variables["ENABLE_OPENSSL_TESTS"] = option_on_off(self.openssl_tests)
        tc.variables["ENABLE_EXPERIMENTAL"] = option_on_off(self.options.enable_experimental)
        tc.variables["ENABLE_ENDOMORPHISM"] = option_on_off(self.options.enable_endomorphism)

        tc.variables["SECP256K1_ECMULT_WINDOW_SIZE"] = self.options.ecmult_window_size
        tc.variables["SECP256K1_ECMULT_GEN_PRECISION"] = self.options.ecmult_gen_precision

        tc.variables["ENABLE_ECMULT_STATIC_PRECOMPUTATION"] = option_on_off(self.options.enable_ecmult_static_precomputation)
        tc.variables["ENABLE_MODULE_ECDH"] = option_on_off(self.options.enable_module_ecdh)
        tc.variables["ENABLE_MODULE_SCHNORR"] = option_on_off(self.options.enable_module_schnorr)
        tc.variables["ENABLE_MODULE_RECOVERY"] = option_on_off(self.options.enable_module_recovery)
        tc.variables["ENABLE_MODULE_MULTISET"] = option_on_off(self.options.enable_module_multiset)
        tc.variables["CONAN_DISABLE_CHECK_COMPILER"] = option_on_off(True)

        self.output.info("Bignum lib selected: %s" % (self.bignum_lib_name,))
        tc.variables["WITH_BIGNUM"] = self.bignum_lib_name
        # tc.variables["WITH_ASM"] = option_on_off(self.options.with_asm)
        # tc.variables["WITH_FIELD"] = option_on_off(self.options.with_field)
        # tc.variables["WITH_SCALAR"] = option_on_off(self.options.with_scalar)
        # tc.variables["WITH_BIGNUM"] = option_on_off(self.options.with_bignum)

        if self.settings.os == "Windows":
            if self.settings.compiler == "Visual Studio" and (self.settings.compiler.version != 12):
                tc.variables["ENABLE_TESTS"] = option_on_off(False)   #Workaround. test broke MSVC

        tc.generate()
        tc = CMakeDeps(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

        #TODO(fernando): Cmake Tests and Visual Studio doesn't work
        #TODO(fernando): Secp256k1 segfaults al least on Windows
        # if self.options.tests:
        #     cmake.test()
        #     # cmake.test(target="tests")


    # def package(self):
    #     copy(self, "*.h", dst="include", src="include", keep_path=True)
    #     copy(self, "*.lib", dst="lib", keep_path=False)
    #     copy(self, "*.dll", dst="bin", keep_path=False)
    #     copy(self, "*.dylib*", dst="lib", keep_path=False)
    #     copy(self, "*.so", dst="lib", keep_path=False)
    #     copy(self, "*.a", dst="lib", keep_path=False)

    def package(self):
        cmake = CMake(self)
        cmake.install()
        # rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))
        # rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))
        # rmdir(self, os.path.join(self.package_folder, "res"))
        # rmdir(self, os.path.join(self.package_folder, "share"))

    def package_info(self):
        self.cpp_info.libs = ["secp256k1"]
        if self.package_folder:
            self.env_info.CPATH = os.path.join(self.package_folder, "include")
            self.env_info.C_INCLUDE_PATH = os.path.join(self.package_folder, "include")
            self.env_info.CPLUS_INCLUDE_PATH = os.path.join(self.package_folder, "include")
