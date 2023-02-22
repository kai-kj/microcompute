import re
import sys


def generate_docs(code: str):
    doc = ""
    start = True

    code_section = ""
    doc_section = ""

    for line in code.splitlines():
        if re.search(r"^\/\*\*", line):  # documentation block start
            pass

        elif re.search(r"^ \*\/", line):  # documentation block end
            doc_section += "\n"

        elif re.search(r"^ \*", line):  # inside documentation block
            if doc_section != "" and code_section != "":  # new documentation block
                doc_section = re.sub(r"```\s*", "\n```\n", doc_section)

                if start:  # the first documentation block
                    doc += doc_section
                    start = False
                else:
                    doc += "----\n\n```c\n" + code_section.strip() + "\n```\n" + doc_section
                doc_section = ""
                code_section = ""

            if re.search(r"^ \*$", line):  # empty line
                doc_section += "\n"
            else:
                doc_section += re.sub(r"^ \* ", "", line) + "\n"

        else:  # code
            code_section += line + "\n"

    # add the last block
    doc += "\n----\n\n```c\n" + code_section.strip() + "\n```\n" + doc_section
    return doc


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("usage python3 doc_generator.py [code file] [doc file]")

    code = open(sys.argv[1], 'r').read()
    doc = generate_docs(code)
    open(sys.argv[2], "w").write(doc)
