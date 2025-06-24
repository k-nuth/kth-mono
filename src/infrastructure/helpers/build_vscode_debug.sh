cd build

# rm -rf *

conan install .. -o tests=True -s build_type=Debug 

conan build ..