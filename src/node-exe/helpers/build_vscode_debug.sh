mkdir build
cd build

# rm -rf *
# rm -rf bin
conan install .. -s build_type=Debug
conan build ..
