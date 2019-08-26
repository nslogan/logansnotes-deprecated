require 'commonmarker'
# require 'pygments'
require 'optparse'

# Maybe at some point this will take multiple files. Not right now.

options = {}
OptionParser.new do |parser|
	parser.banner = "Usage: build.rb [options]"

	parser.on("-i", "--input <input-file>", "Input file", "Input filename required!") do |in_file|
		options[:in_file] = in_file
	end

	parser.on("-o", "--output <output-file>", "Output file", "Output filename required!") do |out_file|
		options[:out_file] = out_file
	end
end.parse!

if not options[:in_file] or not options[:out_file]
	puts("Input and output filenames required!")
	exit 1
end

doc = CommonMarker.render_html( File.read( options[:in_file] ), :DEFAULT )

f = File.open( options[:out_file], 'w' )
f.write(doc)

# # I see, looks like I'll have to traverse the tree and highlight based on language tag. Makes sense that GitHub has a wrapper for their stuff. I'll have to see if I can find it. For now let's try one example.
# code = Pygments.highlight(File.read('/home/logan/Projects/ia-us1/bms-firmware/src/task.cpp'), :lexer => 'c++')

# f = File.open('task.html','w')
# f.write(code)

# f = File.open('task.css','w')
# f.write(Pygments.css)
