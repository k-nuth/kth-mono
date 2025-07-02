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
        "tests": True,
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
            check_min_cppstd(self, "23")

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
        # Disable ecmult static precomputation for Emscripten to avoid native build issues
        if self.settings.os == "Emscripten":
            self.output.info("Setting enable_ecmult_static_precomputation to False for Emscripten")
            self.options.enable_ecmult_static_precomputation = False

    def configure(self):
        KnuthConanFileV2.configure(self)

        self.options["fmt/*"].header_only = True
        # log os
        self.output.info("Compiling for OS: %s" % (self.settings.os,))
        if self.settings.os == "Emscripten":
            self.options["boost/*"].header_only = True

        if self.options.log == "spdlog":
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
        # Set the main CMake file name
        self.cpp_info.set_property("cmake_file_name", "kth")
        
        # Define individual components as separate targets
        # Each component will be available as kth::component_name
        
        # Secp256k1 cryptographic library
        self.cpp_info.components["secp256k1"].libs = ["secp256k1"]
        self.cpp_info.components["secp256k1"].names["cmake_find_package"] = "secp256k1"
        self.cpp_info.components["secp256k1"].names["cmake_find_package_multi"] = "secp256k1"
        # secp256k1 requires GMP for big number operations
        self.cpp_info.components["secp256k1"].requires = ["gmp::gmp"]

        # Core infrastructure component
        self.cpp_info.components["infrastructure"].libs = ["infrastructure"]
        self.cpp_info.components["infrastructure"].names["cmake_find_package"] = "infrastructure"
        self.cpp_info.components["infrastructure"].names["cmake_find_package_multi"] = "infrastructure"
        # Infrastructure core dependencies: secp256k1, boost, fmt, expected-lite, ctre, spdlog
        self.cpp_info.components["infrastructure"].requires = [
            "secp256k1", 
            "boost::boost", 
            "fmt::fmt", 
            "expected-lite::expected-lite", 
            "ctre::ctre", 
            "spdlog::spdlog"
        ]
        
        
        # Domain models and business logic
        self.cpp_info.components["domain"].libs = ["domain"]
        self.cpp_info.components["domain"].names["cmake_find_package"] = "domain"
        self.cpp_info.components["domain"].names["cmake_find_package_multi"] = "domain"
        # Domain depends on infrastructure and tiny-aes-c for wallet encryption
        self.cpp_info.components["domain"].requires = [
            "infrastructure", 
            "tiny-aes-c::tiny-aes-c"
        ]
        
        # Consensus rules and validation
        self.cpp_info.components["consensus"].libs = ["consensus"]
        self.cpp_info.components["consensus"].names["cmake_find_package"] = "consensus"
        self.cpp_info.components["consensus"].names["cmake_find_package_multi"] = "consensus"
        # Consensus has its own direct dependencies: boost, openssl, secp256k1 (internal component)
        self.cpp_info.components["consensus"].requires = [
            "secp256k1",
            "boost::boost", 
            "openssl::openssl"
        ]
        
        # Database layer
        self.cpp_info.components["database"].libs = ["database"]
        self.cpp_info.components["database"].names["cmake_find_package"] = "database"
        self.cpp_info.components["database"].names["cmake_find_package_multi"] = "database"
        # Database depends on domain and lmdb
        self.cpp_info.components["database"].requires = ["domain", "lmdb::lmdb"]
        
        # Blockchain management
        self.cpp_info.components["blockchain"].libs = ["blockchain"]
        self.cpp_info.components["blockchain"].names["cmake_find_package"] = "blockchain"
        self.cpp_info.components["blockchain"].names["cmake_find_package_multi"] = "blockchain"
        # Blockchain depends on database and optionally consensus
        self.cpp_info.components["blockchain"].requires = [
            "database", 
            "consensus"
        ]
        
        # Network layer (not available in Emscripten builds)
        if self.settings.os != "Emscripten":
            self.cpp_info.components["network"].libs = ["network"]
            self.cpp_info.components["network"].names["cmake_find_package"] = "network"
            self.cpp_info.components["network"].names["cmake_find_package_multi"] = "network"
            # Network depends on domain
            self.cpp_info.components["network"].requires = ["domain"]
        
        # Node implementation
        self.cpp_info.components["node"].libs = ["node"]
        self.cpp_info.components["node"].names["cmake_find_package"] = "node"
        self.cpp_info.components["node"].names["cmake_find_package_multi"] = "node"
        # Node depends on blockchain and optionally network (if not Emscripten)
        node_requires = ["blockchain"]
        if self.settings.os != "Emscripten":
            node_requires.append("network")
        self.cpp_info.components["node"].requires = node_requires
        
        # Optional components
        # Node executable (if built)
        try:
            node_exe_libs = [lib for lib in collect_libs(self) if "node-exe" in lib or "kth-node-exe" in lib]
            if node_exe_libs:
                self.cpp_info.components["node-exe"].libs = node_exe_libs
                self.cpp_info.components["node-exe"].names["cmake_find_package"] = "node-exe"
                self.cpp_info.components["node-exe"].names["cmake_find_package_multi"] = "node-exe"
                self.cpp_info.components["node-exe"].requires = ["node"]
        except:
            # If collect_libs fails or node-exe is not built, skip it
            pass
        
        # C API (if enabled)
        # Note: BUILD_C_API option might not be accessible here, so we'll try to detect if the lib exists
        try:
            c_api_libs = [lib for lib in collect_libs(self) if "c-api" in lib or "kth-c-api" in lib]
            if c_api_libs:
                self.cpp_info.components["c-api"].libs = c_api_libs
                self.cpp_info.components["c-api"].names["cmake_find_package"] = "c-api"
                self.cpp_info.components["c-api"].names["cmake_find_package_multi"] = "c-api"
                self.cpp_info.components["c-api"].requires = ["node"]
        except:
            # If collect_libs fails or c-api is not built, skip it
            pass
        
        # Main target that includes all core components (equivalent to the old behavior)
        # This provides a convenient way to link against all of kth at once
        main_requires = ["infrastructure", "domain", "consensus", "database", "blockchain", "node", "secp256k1"]
        if self.settings.os != "Emscripten":
            main_requires.append("network")
        
        self.cpp_info.components["kth"].requires = main_requires
        self.cpp_info.components["kth"].names["cmake_find_package"] = "kth"
        self.cpp_info.components["kth"].names["cmake_find_package_multi"] = "kth"
