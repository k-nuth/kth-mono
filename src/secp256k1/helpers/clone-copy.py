import os
import os.path
from shutil import copyfile


PATH = '/Users/fernando/dev/kth/secp256k1/src/'
SOURCE_PATH = '/Users/fernando/dev/bitcoin-abc/src/secp256k1/src/'

# # result = [os.path.join(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames if os.path.splitext(f)[1] == '.txt']
# # result = [os.path.join(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames]
# result1 = [(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames]

# for dp, f in result1:
#     x = os.path.join(dp, f)

#     # print(dp)
#     # print(x)
#     # print(x[len(PATH):])

#     source_file = os.path.join(SOURCE_PATH, x[len(PATH):])   
#     print(source_file)
#     # print(os.path.isfile(source_file))

#     # print(x)

#     try:
#         copyfile(source_file, x)
#     except IOError as identifier:
#         pass

print('------------------------------------------------')

result2 = [(dp, f) for dp, dn, filenames in os.walk(SOURCE_PATH) for f in filenames]

for dp, f in result2:
    x = os.path.join(dp, f)

    # print(dp)
    # print(x)
    # print(x[len(PATH):])

    target_file = os.path.join(PATH, x[len(SOURCE_PATH):])   
    print(target_file)
    # print(os.path.isfile(source_file))

    dir_name, _ = os.path.split(target_file)
    print(dir_name)

    try:
        os.makedirs(dir_name) #, exist_ok=True)
    except OSError as identifier:
        pass


    # print(x)
    copyfile(x, target_file)

    # try:
    #     copyfile(x, target_file)
    # except IOError as identifier:
    #     pass

    
