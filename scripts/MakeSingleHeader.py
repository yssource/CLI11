#!/usr/bin/env python

from __future__ import print_function, unicode_literals

import os
import re
from argparse import ArgumentParser
from subprocess import Popen, PIPE

tag_str = r"""
^               # Begin of line
[/\s]+          # Whitespace or comment // chars
\[              # A literal [
{tag}:          # The tag
(?P<name>\w+)   # name: group name
:               # Colon
(?P<action>\w+) # action: type of include
\]              # A literal ]
\s*             # Whitespace
$               # End of a line

(?P<content>.*) # All

^               # Begin of line
[/\s]+          # Whitespace or comment // chars
\[              # A literal [
{tag}:          # The tag
(?P=name)       # Repeated name
:               # Colon
end             # Literal "end"
\]              # A literal ]
\s+             # Whitespace
$               # End of a line
"""

re_matcher = re.compile(
   tag_str.format(tag) re.MULTILINE | re.DOTALL | re.VERBOSE
)

DIR = os.path.dirname(os.path.abspath(__file__))


class HeaderGroups(dict):
    def __init__(self, tag):
        self.re_matcher = re_matcher.format(tag=tag)
        super(HeaderGroups, self).__init__()

    def read_header(self, filename):
        with open(pathname) as f:
            inner = f.read()

        for name, action, content in self.re_matcher(inner):
            if action == 'verbatim':
                assert name not in self, "{name} read in more than once! Quitting.".format(name=name)
                self[name] = content
            elif action == 'set':
                self[name] = self.get("name", set()) | set(content.strip().splitlines())
            else:
                raise RuntimeError("Action not understood, must be verbatim or set")




def MakeHeader(
    output, main_header, files, tag, namespace, macro=None
):
    groups = HeaderGroups(tag)

    # Set tag if possible to class variable
    try:
        proc = Popen(
            ["git", "describe", "--tags", "--always"], cwd=str(DIR), stdout=PIPE
        )
        out, _ = proc.communicate()
        groups['git'] = out.decode("utf-8").strip() if proc.returncode == 0 else ""
    except OSError:
        groups['git'] = ""

    with open(main_header) as f:
        header = f.read()

    for f in files:
        groups.read_header(f)

    groups['namespace'] = namespace

    with open(main_header) as f:
        single_header = f.read().format(**groups)

    if macro is not None:
        before, after = macro
        print("Converting macros", before, "->", after)
        single_header.replace(before, after)

    with open(output, "w") as f:
        f.write(single_header)

    print("Created", output)


if __name__ == "__main__":
    parser = ArgumentParser(
        usage="Convert source to single header include. Can optionally add namespace and search-replace replacements (for macros)."
    )
    parser.add_argument("-o,--output", required=True, help="Single header file output")
    parser.add_argument(
        "--main",
        default="CLI11.hpp.in",
        help="The main include file that defines the other files",
    )
    parser.add_argument("files", nargs='*', required=True, help="The header files")
    parser.add_argument("--namespace", default="CLI", help="Set the namespace")
    parser.add_argument("--macro", nargs=2, help="Replaces OLD_PREFIX_ with NEW_PREFIX_")
    args = parser.parse_args()

    MakeHeader(args.output, args.main, args.include, args.namespace, args.macro)
