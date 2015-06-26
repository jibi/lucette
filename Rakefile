require 'rake'
require 'rake/clean'

NAME = 'lucette'

CC = ENV['CC'] || 'clang'
CFLAGS = "#{ENV['CFLAGS']} -Wall -pedantic -I include -g"
LDFLAGS = "#{ENV['LDFLAGS']} -lasound -lfftw3 -lm"

SOURCES = FileList['src/*.c']
OBJECTS = SOURCES.ext('o')
CLEAN.include(OBJECTS).include(NAME)

task :default => NAME

file NAME => OBJECTS do
	sh "#{CC} #{LDFLAGS} #{OBJECTS} -o #{NAME}"
end

rule '.o' => '.c' do |file|
	sh "#{CC} #{CFLAGS} -c #{file.source} -o #{file}"
end

