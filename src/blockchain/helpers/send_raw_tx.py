import requests
import json

ok_txs = 0
error_19 = 0
error_other = 0

def send_raw_tx(rawtx):
    global ok_txs
    global error_19
    global error_other

    url = "http://127.0.0.1:18332"
    headers = {'Content-type': 'text/plain;'}
    datatx = '{"jsonrpc": "1.0", "id":"curltest", "method": "sendrawtransaction", "params": ["' + rawtx + '"] }'
    r = requests.post(url, data=datatx, headers=headers)
    # rsp = requests.post(url, json=datas, headers=headers)


    # print(r.status_code, r.reason)
    # print(r.text)
    d = json.loads(r.text)
    # print(d)
    # print(d['rawtx'])
    # out_file.write(d['rawtx'])
    # out_file.write('\n')

    # {u'id': u'curltest', u'error': {u'message': u'Failed to submit transaction.', u'code': 19}}

    if d['error'] is not None:
        if d['error']['code'] != 19:
            error_19 = error_19 + 1
            print(d['error'])
        else:
            error_other = error_other + 1
    else:
        ok_txs = ok_txs + 1

with open('txs.txt') as fp:
    rawtxs = fp.readlines()

line = 1
for rawtx in rawtxs:
    print("line " + str(line))
    rawtx = rawtx[:len(rawtx)-1]
    send_raw_tx(rawtx)
    line = line + 1
    # break

print("ok_txs      " + str(ok_txs))
print("error_19    " + str(error_19))
print("error_other " + str(error_other))

