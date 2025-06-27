# Copyright (c) 2016-2025 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

import os
from conan import ConanFile
from conan.tools.build.cppstd import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy
from kthbuild import KnuthConanFileV2, option_on_off
from conan.tools.files import collect_libs

required_conan_version = ">=2.0"

class KthRecipe(KnuthConanFileV2):
    # version is set dynamically via --version parameter
    # version="0.68.0"
    name = "kth"
    license = "https://opensource.org/licenses/MIT"
    url = "https://github.com/k-nuth/kth-mono"
    description = "Bitcoin Cross-Platform C++ Development Toolkit"
    settings = "os", "compiler", "build_type", "arch"

    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "tests": [True, False],
        "console": [True, False],
        "march_id": ["ANY"],
        "march_strategy": ["download_if_possible", "optimized", "download_or_fail"],
        "currency": ['BCH', 'BTC', 'LTC'],
        "verbose": [True, False],
        "mempool": [True, False],
        "db": ['dynamic'],
        "db_readonly": [True, False],
        "cxxflags": ["ANY"],
        "cflags": ["ANY"],
        "cmake_export_compile_commands": [True, False],
        "log": ["boost", "spdlog", "binlog"],

        "with_examples": [True, False],
        "with_icu": [True, False],
        "with_png": [True, False],
        "with_qrencode": [True, False],

        "enable_benchmark": [True, False],
        "enable_openssl_tests": [True, False],
        "enable_experimental": [True, False],
        "enable_endomorphism": [True, False],
        "enable_ecmult_static_precomputation": [True, False],
        "enable_module_ecdh": [True, False],
        "enable_module_schnorr": [True, False],
        "enable_module_recovery": [True, False],
    }
            #    "with_asm": ['x86_64', 'arm', 'no', 'auto'],
            #    "with_field": ['64bit', '32bit', 'auto'],
            #    "with_scalar": ['64bit', '32bit', 'auto'],
            #    "with_bignum": ['gmp', 'no', 'auto'],


    default_options = {
        "shared": False,
        "fPIC": True,
        "tests": False,
        "console": False,
        "march_strategy": "download_if_possible",
        "currency": "BCH",
        "verbose": False,
        "mempool": False,
        "db": "dynamic",
        "db_readonly": False,
        "cmake_export_compile_commands": False,
        "log": "spdlog",

        "with_examples": False,
        "with_icu": False,
        "with_png": False,
        "with_qrencode": False,
        "enable_benchmark": False,
        "enable_openssl_tests": False,
        "enable_experimental": False,
        "enable_endomorphism": False,
        "enable_ecmult_static_precomputation": True,
        "enable_module_ecdh": False,
        "enable_module_schnorr": True,
        "enable_module_recovery": True
    }

        # "with_asm='auto'",
        # "with_field='auto'",
        # "with_scalar='auto'"
        # "with_bignum='auto'"

    # exports_sources = "src/*", "CMakeLists.txt", "ci_utils/cmake/*", "cmake/*", "knuthbuildinfo.cmake","include/*", "test/*", "console/*"
    exports_sources = "src/*", "include/*", "CMakeLists.txt", "cmake/*", "ci_utils/cmake/*"


    @property
    def is_shared(self):
        # if self.settings.compiler == "Visual Studio" and self.msvc_mt_build:
        #     return False
        # else:
        #     return self.options.shared
        return self.options.shared

    def validate(self):
        KnuthConanFileV2.validate(self)
        if self.info.settings.compiler.cppstd:
            check_min_cppstd(self, "20")

    def requirements(self):
        self.requires("boost/1.86.0", transitive_headers=True, transitive_libs=True)
        self.requires("fmt/11.2.0", transitive_headers=True, transitive_libs=True)
        self.requires("spdlog/1.15.3", transitive_headers=True, transitive_libs=True)
        self.requires("lmdb/0.9.32", transitive_headers=True, transitive_libs=True)
        self.requires("gmp/6.3.0", transitive_headers=True, transitive_libs=True)
        self.requires("expected-lite/0.8.0", transitive_headers=True, transitive_libs=True)
        self.requires("ctre/3.8.1", transitive_headers=True, transitive_libs=True)
        self.requires("tiny-aes-c/1.0.0", transitive_headers=True, transitive_libs=True)
        self.requires("openssl/3.4.1", transitive_headers=True, transitive_libs=True)

        # if self.options.with_png:
        #     self.requires("libpng/1.6.34@kth/stable", transitive_headers=True, transitive_libs=True)

        # if self.options.with_qrencode:
        #     self.requires("libqrencode/4.0.0@kth/stable", transitive_headers=True, transitive_libs=True)

        # if self.options.asio_standalone:
        #     self.requires("asio/1.24.0", transitive_headers=True, transitive_libs=True)



    def build_requirements(self):
        if self.options.tests:
            self.test_requires("catch2/3.6.0")

    def config_options(self):
        KnuthConanFileV2.config_options(self)

    def configure(self):
        KnuthConanFileV2.configure(self)

        self.options["fmt/*"].header_only = True
        self.options["spdlog/*"].header_only = True

        self.options["*"].db_readonly = self.options.db_readonly
        self.output.info("Compiling with read-only DB: %s" % (self.options.db_readonly,))

        self.options["*"].mempool = self.options.mempool
        self.output.info("Compiling with mempool: %s" % (self.options.mempool,))

        self.options["*"].log = self.options.log
        self.output.info("Compiling with log: %s" % (self.options.log,))


    def package_id(self):
        KnuthConanFileV2.package_id(self)

        self.info.options.console = "ANY"
        # self.info.options.no_compilation = "ANY"

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = self.cmake_toolchain_basis()
        tc.variables["CMAKE_VERBOSE_MAKEFILE"] = "ON"

        tc.variables["GLOBAL_BUILD"] = "ON"
        tc.variables["ENABLE_SHARED"] = option_on_off(self.is_shared)
        tc.variables["ENABLE_SHARED_CAPI"] = option_on_off(self.is_shared)
        tc.variables["ENABLE_POSITION_INDEPENDENT_CODE"] = option_on_off(self.options.fPIC)

        tc.variables["WITH_CONSOLE"] = option_on_off(self.options.console)
        tc.variables["WITH_CONSOLE_CAPI"] = option_on_off(self.options.console)

        tc.variables["WITH_MEMPOOL"] = option_on_off(self.options.mempool)
        tc.variables["DB_READONLY_MODE"] = option_on_off(self.options.db_readonly)
        tc.variables["LOG_LIBRARY"] = self.options.log
        tc.variables["CONAN_DISABLE_CHECK_COMPILER"] = option_on_off(True)
        
        tc.variables["WITH_TESTS"] = option_on_off(self.options.tests)
        tc.variables["WITH_EXAMPLES"] = option_on_off(self.options.with_examples)
        tc.variables["WITH_ICU"] = option_on_off(self.options.with_icu)
        tc.variables["WITH_PNG"] = option_on_off(self.options.with_png)
        tc.variables["WITH_QRENCODE"] = option_on_off(self.options.with_qrencode)

        # Secp256k1 --------------------------------------------
        tc.variables["ENABLE_BENCHMARK"] = option_on_off(self.options.enable_benchmark)
        tc.variables["ENABLE_TESTS"] = option_on_off(self.options.tests)
        tc.variables["ENABLE_OPENSSL_TESTS"] = option_on_off(self.options.enable_openssl_tests)
        tc.variables["ENABLE_EXPERIMENTAL"] = option_on_off(self.options.enable_experimental)
        tc.variables["ENABLE_ENDOMORPHISM"] = option_on_off(self.options.enable_endomorphism)
        tc.variables["ENABLE_ECMULT_STATIC_PRECOMPUTATION"] = option_on_off(self.options.enable_ecmult_static_precomputation)
        tc.variables["ENABLE_MODULE_ECDH"] = option_on_off(self.options.enable_module_ecdh)
        tc.variables["ENABLE_MODULE_SCHNORR"] = option_on_off(self.options.enable_module_schnorr)
        tc.variables["ENABLE_MODULE_RECOVERY"] = option_on_off(self.options.enable_module_recovery)
        tc.variables["WITH_BIGNUM"] = "gmp"

        # tc.variables["WITH_ASM"] = option_on_off(self.options.with_asm)
        # tc.variables["WITH_FIELD"] = option_on_off(self.options.with_field)
        # tc.variables["WITH_SCALAR"] = option_on_off(self.options.with_scalar)
        # tc.variables["WITH_BIGNUM"] = option_on_off(self.options.with_bignum)
        # Secp256k1 -------------------------------------------- (END)

        tc.variables["CURRENCY"] = self.options.currency

        tc.generate()
        tc = CMakeDeps(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        if not self.options.cmake_export_compile_commands:
            cmake.build()
            if self.options.tests:
                cmake.test()


    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
