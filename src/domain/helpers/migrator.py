# grep -r -n -i --include="*.cpp" "::from_data(" .
# grep -r -l -i --include="*.cpp" "::from_data(" .

import os

src_dir = '/home/fernando/dev/new-repos/domain/src'
include_dir = '/home/fernando/dev/new-repos/domain/include/kth/domain'

source_list = [
    # 'chain/header'
    # , 'chain/witness'
    # , 'chain/point'
    # , 'chain/output'
    # , 'chain/script'
    # , 'chain/input'
    # , 'chain/block'
    # , 'chain/transaction'

    # 'message/heading'
    # , 'message/alert'
    # , 'message/filter_clear'
    # , 'message/get_blocks'
    # , 'message/header'
    # , 'message/send_headers'
    # , 'message/inventory_vector'
    # , 'message/verack'
    # , 'message/version'
    # , 'message/get_address'
    # , 'message/pong'
    # , 'message/get_block_transactions'
    # , 'message/memory_pool'
    # , 'message/headers'
    # , 'message/ping'
    # , 'message/filter_load'
    # , 'message/inventory'
    # , 'message/address'
    # , 'message/compact_block'
    # , 'message/alert_payload'
    # , 'message/get_headers'
    # , 'message/get_data'
    # , 'message/reject'
    # , 'message/send_compact'
    # , 'message/block'
    # , 'message/merkle_block'
    # , 'message/prefilled_transaction'
    # , 'message/fee_filter'
    # , 'message/block_transactions'
    # , 'message/not_found'
    # , 'message/transaction'
    # , 'message/filter_add'
    # , 'wallet/ec_public'
    # , 'machine/operation'


    # 'message/filter_clear'
    # , 'message/send_headers'
    # , 'message/inventory_vector'
    # , 'message/verack'
    # , 'message/get_block_transactions'
    # , 'message/filter_load'
    # , 'message/get_headers'
    # , 'message/get_data'
    # , 'message/send_compact'
    # , 'message/block'
    # , 'message/prefilled_transaction'
    # , 'message/not_found'
    # , 'message/transaction'
    # , 'wallet/ec_public'


    # 'message/send_headers'
    # , 'message/inventory_vector'
    # , 'message/verack'
    # , 'message/get_block_transactions'
    # , 'message/get_headers'
    # , 'message/get_data'
    # , 'message/send_compact'
    # , 'message/block'
    # , 'message/prefilled_transaction'
    # , 'message/not_found'
    # , 'message/transaction'
    # , 'wallet/ec_public'


    # 'message/send_headers'
    # , 'message/verack'
    # , 'message/get_block_transactions'
    # , 'message/get_headers'
    # , 'message/get_data'
    # , 'message/block'
    # , 'message/prefilled_transaction'
    # , 'message/not_found'
    # , 'message/transaction'
    # , 'wallet/ec_public'

    # 'message/get_block_transactions'
    # , 'message/block'
    # , 'message/prefilled_transaction'
    # , 'message/transaction'
    # , 'wallet/ec_public'

    # 'message/block'
    # , 'message/transaction'
    # , 'wallet/ec_public'

    'wallet/ec_public'
]

def extract_function(content, frm):
    idx = content.index(frm)
    # idx = content.index('::from_data(reader')
    start = content.rfind('\n', 0, idx) + 1
    # start = idx

    # print(idx)
    idx = content.index('{', idx)
    # print(idx)

    deep = 0
    for i in range(idx + 1, len(content)):
        # print(content[i])

        if content[i] == '}':
            if deep == 0:
                end = i
                break
            deep = deep - 1
        elif content[i] == '{':
            deep = deep + 1

    # print(start)
    # print(end)
    # print(content[start:end-start])
    # print(content[start:end+1])
    return content[start:end+1]

def extract_function_decl(content, frm):
    idx = content.index(frm)
    start = content.rfind('\n', 0, idx) + 1
    end = content.index('\n', start)
    # print(start)
    # print(end)
    return content[start:end].strip()[:-1]


def comment_function(content):
    return '//' + content.replace('\n', '\n//')

def indent_function(content):
    return '    ' + content.replace('\n', '\n    ')


def move_reader_function_to_header(header_content, src_content, header_look, src_look, class_name):
    decl = extract_function_decl(header_content, header_look)
    func = extract_function(src_content, src_look)
    src_content = src_content.replace(func, comment_function(func))

    func_lines = func.split('\n')
    func_lines[0] = decl
    func = "\n".join(func_lines)

    func = indent_function(func)
    func = func.replace('reader&', 'R&')
    func = func.replace('from_data(', 'from_data(')
    func = '    template <typename R>\n' + func
    header_content = header_content.replace(header_look, '\n' + func + '\n\n    //' + header_look)

    return header_content, src_content

def move_writer_function_to_header(header_content, src_content, header_look, src_look, class_name):
    decl = extract_function_decl(header_content, header_look)
    func = extract_function(src_content, src_look)
    src_content = src_content.replace(func, comment_function(func))

    func_lines = func.split('\n')
    func_lines[0] = decl
    func = "\n".join(func_lines)

    func = indent_function(func)
    func = func.replace('writer&', 'W&')
    func = func.replace('to_data(', 'to_data(')
    func = '    template <typename W>\n' + func
    header_content = header_content.replace(header_look, '\n' + func + '\n\n    //' + header_look)

    return header_content, src_content


def process_pair(header, src, class_name):

    with open(src, 'r') as f:
        src_content = f.read()

    with open(header, 'r') as f:
        header_content = f.read()

    header_content, src_content = move_reader_function_to_header(header_content, src_content, 'bool from_data(reader', '::from_data(reader', class_name)
    header_content, src_content = move_reader_function_to_header(header_content, src_content, 'static ' + class_name + ' factory_from_data(reader', '::factory_from_data(reader', class_name)
    header_content, src_content = move_writer_function_to_header(header_content, src_content, 'void to_data(writer', '::to_data(writer', class_name)

    src_content = src_content.replace('::from_data(std::istream', '::from_data(std::istream')
    src_content = src_content.replace('return from_data(istream', 'return from_data(istream')
    src_content = src_content.replace('return from_data(source', 'return from_data(source')
    src_content = src_content.replace('::factory_from_data(std::istream', '::factory_from_data(std::istream')
    # src_content = src_content.replace('.from_data(data', '.from_data(data')
    src_content = src_content.replace('.from_data(stream', '.from_data(stream')

    src_content = src_content.replace('::to_data(std::ostream', '::to_data(std::ostream')
    src_content = src_content.replace('to_data(ostream,', 'to_data(ostream,')
    src_content = src_content.replace('to_data(sink,', 'to_data(sink,')

    header_content = header_content.replace('from_data(std::istream', 'from_data(std::istream')
    header_content = header_content.replace('factory_from_data(std::istream', 'factory_from_data(std::istream')
    header_content = header_content.replace('to_data(std::ostream', 'to_data(std::ostream')

    # print(src_content)
    with open(src, 'w') as f:
        f.write(src_content)

    # print(header_content)
    with open(header, 'w') as f:
        f.write(header_content)



def process_pair_2(header, src, class_name):

    with open(src, 'r') as f:
        src_content = f.read()

    with open(header, 'r') as f:
        header_content = f.read()

    # bool alert::from_data(uint32_t version, reader& source)

    header_content, src_content = move_reader_function_to_header(header_content, src_content, 'bool from_data(uint32_t version, reader', '::from_data(uint32_t version, reader', class_name)
    header_content, src_content = move_reader_function_to_header(header_content, src_content, 'static ' + class_name + ' factory_from_data(uint32_t version, reader', '::factory_from_data(uint32_t version, reader', class_name)
    header_content, src_content = move_writer_function_to_header(header_content, src_content, 'void to_data(uint32_t version, writer', '::to_data(uint32_t version, writer', class_name)

    src_content = src_content.replace('::from_data(uint32_t version, std::istream', '::from_data(uint32_t version, std::istream')
    src_content = src_content.replace('return from_data(version, istream', 'return from_data(version, istream')
    src_content = src_content.replace('return from_data(version, source', 'return from_data(version, source')
    src_content = src_content.replace('::factory_from_data(uint32_t version, std::istream', '::factory_from_data(uint32_t version, std::istream')
    # src_content = src_content.replace('.from_data(data', '.from_data(data')
    src_content = src_content.replace('.from_data(version, stream', '.from_data(version, stream')

    src_content = src_content.replace('::to_data(uint32_t version, std::ostream', '::to_data(uint32_t version, std::ostream')
    src_content = src_content.replace('to_data(version, ostream,', 'to_data(version, ostream,')
    src_content = src_content.replace('to_data(version, sink,', 'to_data(version, sink,')

    header_content = header_content.replace('from_data(uint32_t version, std::istream', 'from_data(uint32_t version, std::istream')
    header_content = header_content.replace('factory_from_data(uint32_t version, std::istream', 'factory_from_data(uint32_t version, std::istream')
    header_content = header_content.replace('to_data(uint32_t version, std::ostream', 'to_data(uint32_t version, std::ostream')

    # print(src_content)
    with open(src, 'w') as f:
        f.write(src_content)

    # print(header_content)
    with open(header, 'w') as f:
        f.write(header_content)


def process_pair_3(header, src, class_name):

    with open(src, 'r') as f:
        src_content = f.read()

    with open(header, 'r') as f:
        header_content = f.read()

    header_content, src_content = move_reader_function_to_header(header_content, src_content, 'bool from_data(uint32_t version, reader', '::from_data(uint32_t version, reader', class_name)
    header_content, src_content = move_reader_function_to_header(header_content, src_content, 'static ' + class_name + ' factory_from_data(uint32_t version, reader', '::factory_from_data(uint32_t version, reader', class_name)

    src_content = src_content.replace('::from_data(uint32_t version, std::istream', '::from_data(uint32_t version, std::istream')
    src_content = src_content.replace('return from_data(version, istream', 'return from_data(version, istream')
    src_content = src_content.replace('return from_data(version, source', 'return from_data(version, source')
    src_content = src_content.replace('::factory_from_data(uint32_t version, std::istream', '::factory_from_data(uint32_t version, std::istream')
    src_content = src_content.replace('.from_data(version, stream', '.from_data(version, stream')

    header_content = header_content.replace('from_data(uint32_t version, std::istream', 'from_data(uint32_t version, std::istream')
    header_content = header_content.replace('factory_from_data(uint32_t version, std::istream', 'factory_from_data(uint32_t version, std::istream')

    # print(src_content)
    with open(src, 'w') as f:
        f.write(src_content)

    # print(header_content)
    with open(header, 'w') as f:
        f.write(header_content)


def remove_dir(f):
    return f[f.index('/')+1:]


def process():
    source_list_2 = []
    source_list_3 = []
    for f in source_list:
        header = os.path.join(include_dir, f) + '.hpp'
        src = os.path.join(src_dir, f) + '.cpp'

        try:
            process_pair(header, src, remove_dir(f))
        except:
            # print('error with file: ' + f)
            source_list_2.append(f)

        # print(header)
        # print(src)
        # print(f)
        # print(remove_dir(f))

    # print(source_list_2)

    for f in source_list_2:
        header = os.path.join(include_dir, f) + '.hpp'
        src = os.path.join(src_dir, f) + '.cpp'

        try:
            process_pair_2(header, src, remove_dir(f))
        except:
            # print('22 error with file: ' + f)
            source_list_3.append(f)

        # print(header)
        # print(src)
        # print(f)
        # print(remove_dir(f))

    for f in source_list_3:
        header = os.path.join(include_dir, f) + '.hpp'
        src = os.path.join(src_dir, f) + '.cpp'

        try:
            process_pair_3(header, src, remove_dir(f))
        except:
            print('33 error with file: ' + f)

        # print(header)
        # print(src)
        # print(f)
        # print(remove_dir(f))

def main():
    process()


if __name__ == "__main__":
    main()