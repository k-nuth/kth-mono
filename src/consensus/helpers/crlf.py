import re

with open('conanfile.py', 'r') as content_file:
    content = content_file.read()

linend = re.compile(r'\r')
splited = linend.split(content)

print(len(splited))
print(splited)
