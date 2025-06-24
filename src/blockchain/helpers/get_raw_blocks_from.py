import urllib2
# import urllib.request
import json

# block_counter = 1281975
# bhash = '00000000000002325f93bc5d91f422c590ca3d4fb0aed14ab9fcf8817be2a629'

# block_counter = 1282234
# bhash = '00000000000024c283ff5e61895d70883065503f6ed5c87f99a1c5f44cbbdd23'

# block_counter = 1282356
# bhash = '00000000000001b0134ea7d9b2cf527d1365cdd717320631eddaa8e7c8a3b6b0'

block_counter = 1282427
bhash = '00000000000001cb9040d377b3f8ec77674827ea4299932957d5f71724c8749a'



def process_raw_block(bhash, out_file):
    # https://tbch.blockdozer.com/api/rawblock/0000000007b20792fe44c67a5c216e420032858025ea037329cdc5964e5e5859
    # print(bhash)
    url = 'https://tbch.blockdozer.com/api/rawblock/' + bhash
    contents = urllib2.urlopen(url).read()

    # contents = urllib.request.urlopen(url).read()
    d = json.loads(contents)
    # print(d['rawblock'])
    out_file.write(d['rawblock'])
    out_file.write('\n')


def process_block(bhash, out_file):
    global block_counter

    print(block_counter)
    block_counter = block_counter + 1

    process_raw_block(bhash, out_file)

    # https://tbch.blockdozer.com/api/block/000000000000045e79e1e2757ca0acdeaa7f0fa1a924dbfe9a040dcfd37ad8d0
    # print(bhash)
    url = 'https://tbch.blockdozer.com/api/block/' + bhash
    contents = urllib2.urlopen(url).read()
    # contents = urllib.request.urlopen(url).read()

    d = json.loads(contents)
    # print(d)

    if 'nextblockhash' in d:
        # print(d['nextblockhash'])
        next_hash = d['nextblockhash']
        process_block(next_hash, out_file)
    else:
        print('no hay proximo')

    # out_file.write(d['rawblock'])
    # out_file.write('\n')


# ------------------------------------


with open("raw_blocks_from_1282427.txt", "w") as out_file:
    process_block(bhash, out_file)




