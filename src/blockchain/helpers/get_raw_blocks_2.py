import urllib2
# import urllib.request
import json

def process_block(bhash, out_file):
    # https://tbch.blockdozer.com/api/rawblock/0000000007b20792fe44c67a5c216e420032858025ea037329cdc5964e5e5859
    # print(bhash)
    url = 'https://tbch.blockdozer.com/api/rawblock/' + bhash
    contents = urllib2.urlopen(url).read()

    # contents = urllib.request.urlopen(url).read()
    d = json.loads(contents)
    print(d['rawblock'])
    out_file.write(d['rawblock'])
    out_file.write('\n')

# ------------------------------------


blocks_hashes = [
                #   '0000000003753a55c5ffd906f5d3056f2af5a7df1c301e2852d2b6e217fdc781'
                #   '00000000097c37ce4d663cd3c250d76caad2aa847f22dd1c89ba21ec7e2cd1c3'
                    # '00000000034bc377cf95445ce0ce0cd775a2f970adc25a22360216f549eb9890'
                  '0000000007229dcc89169deca11ff6424eeb999806431a8943bc1b976afbe9f4'
                ]

with open("blocks.txt", "w") as out_file:
    for bhash in blocks_hashes:
        process_block(bhash, out_file)




