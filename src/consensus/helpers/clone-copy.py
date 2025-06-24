import os
import os.path
from shutil import copyfile

# # ABC (obsolete)
# PATH = '/Users/fernando/dev/kth/kth/consensus/src/bch-rules/'
# SOURCE_PATH = '/Users/fernando/dev/abc/bitcoin-abc/src/'

#  BCHN
PATH = '/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/'
SOURCE_PATH = '/Users/fernando/dev/bchn/bitcoin-cash-node-master/src/'
# PATH = '/home/fernando/dev/kth/consensus/src/bch-rules/'
# SOURCE_PATH = '/home/fernando/dev/bchn/bitcoin-cash-node/src/'

# # Core
# PATH = '/Users/fernando/dev/kth/kth/consensus/src/btc-rules/'
# SOURCE_PATH = '/Users/fernando/dev/bitcoin-core/bitcoin/src/'

def replace(kth_files):
    for dp, f in kth_files:
        x = os.path.join(dp, f)

        # print(dp)
        # print(x)
        # print(x[len(PATH):])

        source_file = os.path.join(SOURCE_PATH, x[len(PATH):])
        print(source_file)
        # print(os.path.isfile(source_file))
        # print(x)

        if os.path.isfile(source_file):
            os.makedirs(os.path.dirname(x), exist_ok=True)
            # shutil.copy(src_fpath, dest_fpath)
            copyfile(source_file, x)
        else:
            os.remove(x)


# # kth_files = [os.path.join(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames if os.path.splitext(f)[1] == '.txt']
# # kth_files = [os.path.join(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames]

# kth_files = [(dp, f) for dp, dn, filenames in os.walk(PATH) for f in filenames]
# print(kth_files)
# replace(kth_files)


# # # ------------------------------------------------------------------------------------------


# primitives/token.h


missing_files = [
    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/primitives/', 'token.h')
    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/util/', 'heapoptional.h')
    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/', 'core_memusage.h')

    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/primitives/', 'block.h')

    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/', 'random.h')
    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/', 'psbt.h')


    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/crypto/', 'chacha20.h')
    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/script/', 'sign.h')

    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/', 'streams.h')

    # ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/support/allocators/', 'zeroafterfree.h')

    ('/Users/fernando/dev/kth/kth-dev/consensus/src/bch-rules/support/', 'cleanse.h')

]

# missing_files = [
#     ('/home/fernando/dev/kth/consensus/src/bch-rules/crypto/', 'siphash.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/', 'random.cpp')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/crypto/', 'chacha20.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/', 'random.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/util/', 'saltedhashers.cpp')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/crypto/', 'siphash.cpp')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/consensus/', 'consensus.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/', 'coins.cpp')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/script/', 'script_execution_context.cpp')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/util/', 'saltedhashers.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/primitives/', 'blockhash.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/', 'indirectmap.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/', 'memusage.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/', 'compressor.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/', 'coins.h')
#     # ('/home/fernando/dev/kth/consensus/src/bch-rules/script/', 'script_execution_context.h')
# ]





# #include <crypto/siphash.h>

# missing_files = [
#     # ('/Users/fernando/dev/kth/kth/consensus/src/bch-rules/crypto/', 'siphash.h'),
#     # ('/Users/fernando/dev/kth/kth/consensus/src/bch-rules/compat/', 'cpuid.h'),
#     ('/Users/fernando/dev/kth/kth/consensus/src/bch-rules/config/', 'bitcoin-config.h'),
# ]



# # # missing_files = [
# # #     # ('/Users/fernando/dev/kth/consensus/src/bch-rules/', 'span.h'),
# # #     # ('/Users/fernando/dev/kth/consensus/src/bch-rules/util/', 'strencodings.h'),
# # #     # ('/Users/fernando/dev/kth/consensus/src/bch-rules/', 'attributes.h'),
# # #     # ('/Users/fernando/dev/kth/consensus/src/bch-rules/script/', 'script_metrics.h'),
# # #     # ('/Users/fernando/dev/kth/consensus/src/bch-rules/script/', 'bitfield.h'),
# # #     # ('/Users/fernando/dev/kth/consensus/src/bch-rules/util/', 'bitmanip.h'),
# # #     # ('/Users/fernando/dev/kth/consensus/src/bch-rules/script/', 'bitfield.cpp'),
# # #     # ('/Users/fernando/dev/kth/consensus/src/bch-rules/util/', 'strencodings.cpp'),
# # #     ('/Users/fernando/dev/kth/consensus/src/bch-rules/util/', 'string.h'),
# # # ]
# #     # ('/Users/fernando/dev/kth/consensus/src/bch-rules/config/', 'bitcoin-config.h'),


# # missing_files = [
# #     # ('/Users/fernando/dev/kth/consensus/src/btc-rules/', 'span.h'),
# #     # ('/Users/fernando/dev/kth/consensus/src/btc-rules/util/', 'strencodings.h'),
# #     # ('/Users/fernando/dev/kth/consensus/src/btc-rules/', 'attributes.h'),
# #     # ('/Users/fernando/dev/kth/consensus/src/btc-rules/script/', 'script_metrics.h'),
# #     # ('/Users/fernando/dev/kth/consensus/src/btc-rules/script/', 'bitfield.h'),
# #     # ('/Users/fernando/dev/kth/consensus/src/btc-rules/util/', 'bitmanip.h'),
# #     # ('/Users/fernando/dev/kth/consensus/src/btc-rules/script/', 'bitfield.cpp'),
# #     # ('/Users/fernando/dev/kth/consensus/src/btc-rules/util/', 'strencodings.cpp'),
# #     # ('/Users/fernando/dev/kth/consensus/src/btc-rules/util/', 'string.h'),
# #     ('/Users/fernando/dev/kth/consensus/src/btc-rules/compat/', 'cpuid.h'),
# # ]


print(missing_files)
replace(missing_files)

# # # ------------------------------------------------------------------------------------------

