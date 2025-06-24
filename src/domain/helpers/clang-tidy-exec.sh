cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DUSE_CONAN=ON  -DENABLE_TESTS=OFF -DWITH_TESTS=OFF -DWITH_TOOLS=OFF -DCMAKE_BUILD_TYPE=Release -DJUST_KTH_SOURCES=ON 
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DUSE_CONAN=ON  -DENABLE_TESTS=OFF -DWITH_TESTS=OFF -DWITH_TOOLS=OFF -DCMAKE_BUILD_TYPE=Release

export PATH=$PATH:/usr/local/Cellar/llvm/6.0.0/bin/
clang-tidy --version
# python ../ci_utils/run-clang-tidy.py -checks='*,-android-*,-modernize-use-using,-hicpp-deprecated-headers,-modernize-deprecated-headers,-cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-owning-memory,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-hicpp-vararg,-hicpp-no-malloc,-fuchsia-default-arguments,-google-runtime-references,-cppcoreguidelines-no-malloc,-hicpp-no-malloc,-llvm-header-guard,-android-cloexec-fopen,-cppcoreguidelines-pro-type-union-access,-cppcoreguidelines-pro-type-const-cast,-hicpp-braces-around-statements,-hicpp-special-member-functions,-readability-braces-around-statements,-coreguidelines-special-member-function,-cppcoreguidelines-special-member-functions,-clang-diagnostic-static-inline-explicit-instantiation,-fuchsia-overloaded-operator,-cppcoreguidelines-pro-type-member-init,-hicpp-member-init' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='*,-android-*,-hicpp-deprecated-headers,-modernize-deprecated-headers,-cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-owning-memory,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-hicpp-vararg,-hicpp-no-malloc,-fuchsia-default-arguments,-google-runtime-references,-cppcoreguidelines-no-malloc,-hicpp-no-malloc,-llvm-header-guard,-android-cloexec-fopen,-cppcoreguidelines-pro-type-union-access,-cppcoreguidelines-pro-type-const-cast,-hicpp-braces-around-statements,-hicpp-special-member-functions,-readability-braces-around-statements,-coreguidelines-special-member-function,-cppcoreguidelines-special-member-functions,-clang-diagnostic-static-inline-explicit-instantiation,-fuchsia-overloaded-operator,-cppcoreguidelines-pro-type-member-init,-hicpp-member-init' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include

python ../ci_utils/run-clang-tidy.py -checks='llvm-include-order' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='google-readability-braces-around-statements' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='google-explicit-constructor' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='readability-implicit-bool-conversion' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include

python ../ci_utils/run-clang-tidy.py -checks='readability-inconsistent-declaration-parameter-name' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='readability-avoid-const-params-in-decls' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='readability-else-after-return' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='readability-container-size-empty' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='readability-inconsistent-declaration-parameter-name' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='readability-named-parameter' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='readability-static-accessed-through-instance' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='readability-braces-around-statements' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include


python ../ci_utils/run-clang-tidy.py -checks='modernize-use-using' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='modernize-use-override' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include

modernize-use-default-member-init
modernize-use-using
modernize-return-braced-init-list




python ../ci_utils/run-clang-tidy.py -checks='*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include

python ../ci_utils/run-clang-tidy.py -checks='readability*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include

python ../ci_utils/run-clang-tidy.py -checks='modernize*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='modernize*,-modernize-pass-by-value' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include

python ../ci_utils/run-clang-tidy.py -checks='performance*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='cppcoreguidelines*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include


python ../ci_utils/run-clang-tidy.py -checks='cppcoreguidelines*,-cppcoreguidelines-special-member-functions,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-type-reinterpret-cast,-cppcoreguidelines-pro-bounds-array-to-pointer-decay' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='cppcoreguidelines-special-member-functions*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include


python ../ci_utils/run-clang-tidy.py -checks='bugprone*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='cert*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='fuchsia*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include

python ../ci_utils/run-clang-tidy.py -checks='google*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='google*,-google-runtime-references,-google-readability-todo,-google-build-using-namespace' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include


google-readability-casting
google-readability-namespace-comments
google-explicit-constructor


python ../ci_utils/run-clang-tidy.py -checks='hicpp*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='llvm*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include
python ../ci_utils/run-clang-tidy.py -checks='misc*' -warnings-as-errors='*' -header-filter=/home/fernando/dev/new-repos/infrastructure/include











performance-faster-string-find
performance-unnecessary-value-param
misc-unused-parameters
hicpp-signed-bitwise
cppcoreguidelines-pro-type-reinterpret-cast
cert-dcl58-cpp
google-runtime-int
