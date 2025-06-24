# x = "qwertyui"

with open('temp-string.txt', 'r') as content_file:
    x = content_file.read()

# chunks, chunk_size = len(x), 3 # len(x)/4
chunks, chunk_size = len(x), 1000 # len(x)/4
l = [ x[i:i+chunk_size] for i in range(0, chunks, chunk_size) ]
# ['qw', 'er', 'ty', 'ui']

print(l)


new_str = '"\n"'.join(l)
print(new_str)


with open('temp-string-2.txt', "w") as text_file:
    text_file.write(new_str)

