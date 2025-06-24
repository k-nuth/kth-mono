import requests
import json

ok_blks = 0
error_19 = 0
error_other = 0

def send_raw_block(rawblock):
    global ok_blks
    global error_19
    global error_other

    url = "http://127.0.0.1:18332"
    headers = {'Content-type': 'text/plain;'}

    # curl --user myusername --data-binary '{"jsonrpc": "1.0", "id":"curltest", "method": "submitblock", "params": ["mydata"] }' -H 'content-type: text/plain;' http://127.0.0.1:8332/

    datatx = '{"jsonrpc": "1.0", "id":"curltest", "method": "submitblock", "params": ["' + rawblock + '"] }'
    r = requests.post(url, data=datatx, headers=headers)


    # print(r.status_code, r.reason)
    # print(r.text)
    d = json.loads(r.text)
    # print(d)
    # print(d['rawblock'])
    # out_file.write(d['rawblock'])
    # out_file.write('\n')


    if d['error'] is not None:
        if d['error']['code'] != 19:
            error_19 = error_19 + 1
            print(d['error'])
        else:
            error_other = error_other + 1
    else:
        ok_blks = ok_blks + 1

with open('blocks1.txt') as fp:
    rawblocks = fp.readlines()

line = 1
for rawblock in rawblocks[0:4]:
    print("line " + str(line))
    rawblock = rawblock[:len(rawblock)-1]
    send_raw_block(rawblock)
    line = line + 1
    # break

print("ok_blks      " + str(ok_blks))
print("error_19    " + str(error_19))
print("error_other " + str(error_other))

