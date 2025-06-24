mkdir build
cd build
conan install .. -o use_domain=True -o db_transaction_unconfirmed=False -o db_spends=False -o db_history=False -o db_stealth=False -o db_unspent_legacy=False -o db_legacy=True -o db_new=True
conan build ..