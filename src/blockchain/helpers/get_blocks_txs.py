import urllib2
# import urllib.request
import json

def process_tx(txhash, out_file):
    # https://tbch.blockdozer.com/api/rawtx/be4c1a2f06ca031f08cb62f1d4196d07890d6d5f4448310add7c3b220beae910
    # print(txhash)
    url = 'https://tbch.blockdozer.com/api/rawtx/' + txhash
    contents = urllib2.urlopen(url).read()
    # contents = urllib.request.urlopen(url).read()
    d = json.loads(contents)
    print(d['rawtx'])
    out_file.write(d['rawtx'])
    out_file.write('\n')

def process_block(bhash, out_file):
    print("Block: %s" % bhash)

    # https://tbch.blockdozer.com/api/block/0000000007b20792fe44c67a5c216e420032858025ea037329cdc5964e5e5859
    url = 'https://tbch.blockdozer.com/api/block/' + bhash
    contents = urllib2.urlopen(url).read()
    # contents = urllib.request.urlopen(url).read()

    d = json.loads(contents)
    # print(d['tx'])

    for txhash in d['tx'][1:]:
        process_tx(txhash, out_file)


# ------------------------------------

blocks_hashes = ['0000000007b20792fe44c67a5c216e420032858025ea037329cdc5964e5e5859'
                , '000000000401d3de3324702452e220923b5db9dedfd2d7f11bf984ca307ca98a'
                , '00000000034a69fd27cabbe485163a2ee0ed7d42365ec15ec4b16c85e8b4c482'
                , '0000000004312aabb5ab5ad7cdcc92b8a676b48472962c3d784cde3984bda536'
                , '000000000a4b2ed8d66786ff5656fd26c6d2b3f22f129bb351aa5e3658781e0a'
                , '0000000000c2bc4634fe7db5ee063d3009af80d959d0b343aa6b0737a2ef94bc'
                , '00000000f4a7129d5b54556dc2021a490ff9d359c7202b292425b016e4331141'
                , '0000000005017a548c078eb80475b4e4f48ceaa14f46e563664686e290fbc74b'
                , '0000000003753a55c5ffd906f5d3056f2af5a7df1c301e2852d2b6e217fdc781'
                , '00000000097c37ce4d663cd3c250d76caad2aa847f22dd1c89ba21ec7e2cd1c3'
                , '00000000034bc377cf95445ce0ce0cd775a2f970adc25a22360216f549eb9890'
                , '0000000007229dcc89169deca11ff6424eeb999806431a8943bc1b976afbe9f4'
                , '00000000067fe7fdb8efcdb0000452864d380150dd91604762c9608305463f67'
                , '000000000748ae16cb3db532a5208c63e84610c90d37ab25afb50b01867da0de'
                , '000000007ad7ef1ad9cab28eb5274f8f1481083aa5b9547d2fb7c94891cfc3a5'
                , '00000000a583c567a545b5425b28f992e77e7c17b2495ba62d209eb804350009'
                , '0000000009ca859e50184233942b7feaa1a432a2cbbbb154be5efd3cfc081b5e'
                , '0000000007c5c4334cd42740fdce738472c8dff290f2ef33b6bbbc2ce4470170'
                , '0000000011f7711501e5e282f3abfb716314e2b065b7a76368bc8f55769278b9'
                , '00000000bf59753b5e87397862dab145d594d8e73909f9c757d0e1b7cb9bb50b'
                , '000000003fe6152d067738bcd98f297599100ca651f370d7fa85301a32124e16']

with open("txs.txt", "w") as out_file:
    for bhash in blocks_hashes:
        process_block(bhash, out_file)

