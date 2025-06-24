mkdir build
cd build
conan install ..  -o use_domain=False -o db=new_full -s build_type=Debug
conan build ..