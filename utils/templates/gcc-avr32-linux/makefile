program_name=test

source_dirs=. ../mxsrclib ../mxsrclib/arch/linux
compiler_options=$(compiler_options_ext) -Wno-deprecated -fpack-struct=1
linker_options=$(linker_options_ext)

out_file         := $(program_name)_$(out_file_suffix)
source_dirs      := $(addprefix ../,$(source_dirs))
search_wildcards := $(addsuffix /*.cpp,$(source_dirs))

$(out_file): $(notdir $(patsubst %.cpp,%.o,$(wildcard $(search_wildcards))))
	$(compiler) $(linker_options) $^ -o ../$@

VPATH := $(source_dirs)

%.o: %.cpp
	$(compiler) $(compiler_options) -c -MD $(addprefix -I,$(source_dirs)) $<

include $(wildcard *.d)
