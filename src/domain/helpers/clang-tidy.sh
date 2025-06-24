export PATH=/usr/local/Cellar/llvm/6.0.0/bin/:$PATH
export PATH=/Users/fernando/Downloads/clang+llvm-7.0.0-x86_64-apple-darwin/bin:$PATH


cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DUSE_CONAN=ON  -DENABLE_TESTS=OFF -DWITH_TESTS=OFF -DWITH_TOOLS=OFF -DCMAKE_BUILD_TYPE=Release -DJUST_KTH_SOURCES=OFF


python ../ci_utils/run-clang-tidy.py -checks='*' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include

python ../ci_utils/run-clang-tidy.py -checks='*,-fuchsia*,-llvm-header-guard,-modernize-pass-by-value,-cert-dcl21-cpp,-google-build-using-namespace,-google-runtime-references,-cert-err58-cpp,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-hicpp-no-array-decay' -warnings-as-errors='*' -header-filter=/home/fernando/dev/domain/include


python ../ci_utils/run-clang-tidy.py -checks='modernize*' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include
python ../ci_utils/run-clang-tidy.py -checks='modernize-use-using*' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include/
python ../ci_utils/run-clang-tidy.py -checks='*,-llvm-header-guard,-fuchsia*,-google-runtime-references,-cert-err58-cpp,-cppcoreguidelines-special-member-functions,-modernize-pass-by-value,-cert-dcl21-cpp,-google-explicit-constructor,-hicpp-explicit-conversions,-hicpp-member-init,-hicpp-signed-bitwise,-google-build-using-namespace,-hicpp-special-member-functions,-cppcoreguidelines-pro-type-member-init,-cppcoreguidelines-pro-bounds-pointer-arithmetic,-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-hicpp-no-array-decay' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include



python ../ci_utils/run-clang-tidy.py -checks='readability-braces-around-statements' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include -fix
python ../ci_utils/run-clang-tidy.py -checks='misc-unused-parameters' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include -fix
python ../ci_utils/run-clang-tidy.py -checks='readability-named-parameter' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include -fix
python ../ci_utils/run-clang-tidy.py -checks='readability-avoid-const-params-in-decls' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include -fix
python ../ci_utils/run-clang-tidy.py -checks='readability-else-after-return' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include -fix
python ../ci_utils/run-clang-tidy.py -checks='hicpp-use-equals-default' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include -fix
python ../ci_utils/run-clang-tidy.py -checks='llvm-include-order' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include -fix




python ../ci_utils/run-clang-tidy.py -checks='hicpp-use-override' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/domain/include -fix
python ../ci_utils/run-clang-tidy.py -checks='modernize-use-default-member-init' -warnings-as-errors='*' -header-filter=/Users/fernando/dev/sdomain/include -fix






