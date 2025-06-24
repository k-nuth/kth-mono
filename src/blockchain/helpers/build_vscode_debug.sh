mkdir build
cd build
# rm -rf *
conan install .. -o use_domain=True -o mempool=True -o with_tests=True -s build_type=Debug -o db=pruned
conan build ..
