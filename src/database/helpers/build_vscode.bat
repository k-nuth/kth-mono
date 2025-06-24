mkdir build
cd build
conan install .. -o use_domain=False -o db=new_full  -o tests=True
conan build ..