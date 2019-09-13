import argparse
import cmarkgfm
from bs4 import BeautifulSoup
import re

from pygments import highlight
from pygments.lexers.asm import GasLexer
from pygments.formatters import HtmlFormatter

import mako

# TODO: (?) Probably want higher level options to this like 'build.py <command> <options>' - I want to be able to spit out the highlight CSS but I only need to do that once for the whole site (pretty sure). The other option is to have a separate file that does the CSS generation.

parser = argparse.ArgumentParser(description='')
parser.add_argument('-i,--input', dest='in_file', required=True, help='Input file')
parser.add_argument('-o,--output', dest='out_file', required=True, help='Output file')

args = parser.parse_args()

with open(args.in_file,'r') as f:
	html = cmarkgfm.github_flavored_markdown_to_html(f.read())

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
from mako.template import Template
page = Template(filename='tools/templates/main.mako').render(
	page_title='Test',
	page_content=str(soup)
)

# TODO: Put the HTML into a template
with open(args.out_file,'w') as f:
	f.write(page)
