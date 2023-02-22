import re
import sys


class Lines:
    def __init__(self, text):
        self.current = 0
        self.lines = text.splitlines(True)

    def _get_line(self, index):
        if index >= len(self.lines): return None
        else: return self.lines[index]

    def next(self):
        self.current += 1
        return self._get_line(self.current - 1)

    def peek(self):
        return self._get_line(self.current)

def doc_section_start(line):
    return re.search(r"^\/\*\*\s*(\w*)", line)

def doc_section_end(line):
    return re.search(r"^ \*\/", line)

def parse_doc_line(line):
    if m := re.search(r"^ \* (.*)", line): return m.groups()[0] + "\n"
    elif m := re.search(r"^ \*(.*)", line): return m.groups()[0] + "\n"
    else: raise ("unexpected line: \"" + line[:-1] + "\"")

def parse_doc_section(lines, prepend=""):
    doc = ""
    while True:
        line = lines.peek()
        if not line: raise ("unexpected EOF")
        if doc_section_end(line): break
        else: doc += prepend + parse_doc_line(lines.next())

    return doc

def parse_code_section(file):
    code = ""
    while True:
        line = lines.peek()
        if not line or doc_section_start(line): break
        else: code += lines.next()

    return code


def generate_docs(lines):
    doc = ""

    while line := lines.next():
        if doc_type := doc_section_start(line):
            doc_type = doc_type.groups()[0]

            if doc_type == "code":
                doc_section = parse_doc_section(lines, "> ")
                lines.next()
                code_section = parse_code_section(lines).strip() + "\n"
                doc += "```c\n" + code_section + "```\n" + doc_section + "\n<br/>\n\n"

            elif doc_type == "":
                doc_section = parse_doc_section(lines)
                doc += doc_section

            else: raise ("unknown documentation type " + doc_type)
    
    return doc


if __name__ == "__main__":
    if len(sys.argv) != 3: raise("provide 2 arguments")

    lines = Lines(open(sys.argv[1], "r").read())
    doc = generate_docs(lines)
    open(sys.argv[2], "w").write(doc)