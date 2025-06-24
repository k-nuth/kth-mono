import urllib2
# import urllib.request
import json
import hashlib
import binascii



def encode_hash(hash):
    return binascii.hexlify(hash[::-1])

def double_sha256_from_hex(hex_string):

    try:
        hex_data = hex_string.decode("hex")
    except:
        hex_data = ''
        # print("Caught it!")

    # hex_data = hex_string.decode("hex")

    m = hashlib.sha256()
    m.update(hex_data)
    m2nd = hashlib.sha256()
    m2nd.update(m.digest())
    return m2nd.digest()



# # hex_string = "020000000114b236f77593f8b6a7c5737d08f88f99b4ba0ab1ebf33a2bf2bfa17ad0df8283140000006b483045022100cbb84bcdedb6940fc1ae9f17966801d01adef8bb1137e7848ea7e4cbe34809fc0220468da4968842888dfe720a97c4997273ab2060a31a6e99a2735ae6e4eceb4f2541210385ea31c4280a63bb1af61cb4b25dfae7d75996946ae5e0c380577d356d385cbbffffffff020000000000000000de6a4cdb4243484644550001007d1cf45e4b7dd74af8e2867b1c0586591e84af5ae90cd1b758e6d7321e9577a474e6a9e69693ed1c9bffb2ecb71d4083c49545e2aa062d2ed4bce0bc421e8a92571ba5c91f483aeda4a4f7e65c11ed1c6ca93a6e745da2ac88a0317486253d3c5fdc175e8770991d03a27153735285c65fd2df18d965ac2953999ab4328daa85e8aec9ec10e2133a202ebfca46d3724c5c903d232bb4f880e23ec074829d1148c0c803dfc9b54fd40d07593404df70041f0fafb692a079564bb7b287465652c98bff2fe4db1181c6489e45737ad9fd93314b44160000000000001976a9144a7e7ac9db1632ffcef9982bc1c31d1bef86dc1f88ac00000000"
# # 013c4e25e4c443859460801d1501266615df1c337dac963abfc7c023cc55b3f0
# hex_string = "01000000010000000000000000000000000000000000000000000000000000000000000000ffffffff27034e90131a4d696e656420627920416e74506f6f6c2035000102205c4625b4bc0000003d010000ffffffff026822a804000000001976a914c1770ee3a5ef09094b9a84680c6d73527b21a0af88ac0000000000000000266a24aa21a9ed85580a7539a51972c7de8bcb6247bb95ea112ec0ba437ce837708753c4dc14cb00000000"
# print(encode_hash(double_sha256_from_hex(hex_string)))


def process_raw(hex_string, out_file, err_file):
    # print("Block: %s" % bhash)

    encoded_hash = encode_hash(double_sha256_from_hex(hex_string))


    # https://tbch.blockdozer.com/api/rawtx/be4c1a2f06ca031f08cb62f1d4196d07890d6d5f4448310add7c3b220beae910
    # print(txhash)
    url = 'https://tbch.blockdozer.com/api/rawtx/' + encoded_hash

    # print("url: %s" % url)

    try:
        contents = urllib2.urlopen(url).read()
        # contents = urllib.request.urlopen(url).read()

        if 'Not found' not in contents:
            # print("TX OK: %s" % encoded_hash)
            d = json.loads(contents)
            print(d['rawtx'])
            out_file.write(d['rawtx'])
            out_file.write('\n')
        else:
            # print("TX Error: %s" % hex_string)
            err_file.write(hex_string)
    except:
        # print("url: %s" % url)
        # print("TX Error: %s" % hex_string)
        # print("TX Error: %s" % encoded_hash)
        err_file.write(hex_string)



with open("tx_existance.txt") as in_file:
    with open("tx_existance_checked.txt", "w") as out_file:
        with open("tx_existance_err.txt", "w") as err_file:
            for hex_string in in_file:
                process_raw(hex_string.rstrip(), out_file, err_file)

