import argparse
import cmarkgfm
from bs4 import BeautifulSoup
import re

from pygments import highlight
from pygments.lexers.asm import GasLexer
from pygments.formatters import HtmlFormatter

from mako.template import Template

def flash_block(content):
	# <div class="flash mb-3"><svg aria-hidden="true" class="octicon octicon-alert mr-2" height="16" viewBox="0 0 16 16" width="16"><path fill-rule="evenodd" d="M15.98 7.83l-.97-5.95C14.84.5 13.13 0 12 0H5.69c-.2 0-.38.05-.53.14L3.72 1H2C.94 1 0 1.94 0 3v4c0 1.06.94 2.02 2 2h2c.91 0 1.39.45 2.39 1.55.91 1 .88 1.8.63 3.27-.08.5.06 1 .42 1.42.39.47.98.76 1.56.76 1.83 0 3-3.71 3-5.01l-.02-.98h2.04c1.16 0 1.95-.8 1.98-1.97 0-.11-.02-.21-.02-.21zm-1.97 1.19h-1.99c-.7 0-1.03.28-1.03.97l.03 1.03c0 1.27-1.17 4-2 4-.5 0-1.08-.5-1-1 .25-1.58.34-2.78-.89-4.14C6.11 8.75 5.36 8 4 8V2l1.67-1H12c.73 0 1.95.31 2 1l.02.02 1 6c-.03.64-.38 1-1 1h-.01z"></path></svg>The Pygments <code>lexers.asm.GasLexer</code> doesn't support the macro family of syntax so the backslash above is marked as invalid. I may consider submitting a patch for better gas support in the <a href="https://bitbucket.org/birkenfeld/pygments-main/src/default/pygments/lexers/asm.py">lexer</a> if I get around to it.</div>

	html = cmarkgfm.github_flavored_markdown_to_html(content)

	return '<div class="flash mb-3">' + html + '</div>'


# TODO: (?) Probably want higher level options to this like 'build.py <command> <options>' - I want to be able to spit out the highlight CSS but I only need to do that once for the whole site (pretty sure). The other option is to have a separate file that does the CSS generation.

parser = argparse.ArgumentParser(description='')
parser.add_argument('-i,--input', dest='in_file', required=True, help='Input file')
parser.add_argument('-o,--output', dest='out_file', required=True, help='Output file')

args = parser.parse_args()

# Read the markdown document
with open(args.in_file,'r') as f:
	markdown = f.read()

# Pre-process the document
re_flash_block = re.compile(r'^!flash ({.+})?\((.+)\)',re.MULTILINE)

# for match in re_flash_block.finditer(markdown):
# 	# JSON arguments: match.group(1)
# 	# MarkDown content: match.group(2)

# Alright, so I need to write a simple line processor that is context-aware -
# the only context I care about is if we're in a code block or not; if we are,
# we don't do any pre-processing. This pre-processor consumes lines and can
# leave the line alone or replace it with a pre-processed line. It probably
# makes sense for now to only support a single level of pre-processing. The
# pre-processors can have a priority (for now it will just be the order they're
# added to the list).

# Convert to HTML
html = cmarkgfm.github_flavored_markdown_to_html(markdown)

# TODO: Get lexer name from class name and dynamically import the lexer (probably need a name-to-lexer map)
soup = BeautifulSoup(html,features="html.parser")
for m in soup.find_all('code', class_=re.compile('language-.+')):
	# TODO: Get lexer name from regex search - I don't know if I can get that from the search result (probably not), but I can just re-run the regex on the class name list and get it from there
	code = m.text
	# HtmlFormatter(linenos=True)
	# TODO: Add class name calculated from comment above (e.g. `highlight-source-<lexer>`)
	# TODO: Consider usage of `wrapcode=True` option to add a <code> tag inside the <pre> tag - this is canonical HTML5 but *not* what GH does
	# NOTE: If the short Pygments highlight class names clash with class names from other styles you can use the `classprefix` option to set a prefix, for example, 'pyg-'
	code_html = BeautifulSoup(highlight(code, GasLexer(), HtmlFormatter()),features="html.parser")
	# We replace the parent because Pygments spits out a <pre> warpped in a <div> by default. This behavior can be disabled by setting `nowrap=True` in the the HtmlFormatter. If I disable the behavior then I need to add the classes to the parent <pre> tag.
	# If I intended to keep replacing the parent I should verify that the parent is in fact a <pre> like I expect.
	# FYI: Github uses <div class="highlight highlight-source-<lexer>"><pre></pre></div>
	m.parent.replace_with(code_html)

css = HtmlFormatter().get_style_defs('.highlight')

# TODO: Maybe pass the build directory name in (?)
# TODO: Consider where this generated file goes
# TODO: Consider whether this file needs to be generated every site build (what makes this file out of date? The Pygments version?)
# TODO: 'site/css' is now a depdir of the build if this is the output directory
with open('site/css/highlight.css','w') as f:
	f.write(css)

# Generate page
page = Template(filename='tools/templates/main.mako').render(
	page_title='Test',
	page_content=str(soup)
)

# TODO: Put the HTML into a template
with open(args.out_file,'w') as f:
	f.write(page)
