program_name=tmpllin

source_dirs=. ../mxsrclib ../mxsrclib/arch/cur
cfg=-Wno-deprecated

out_file         := $(program_name)_$(out_file_suffix)
source_dirs      := $(addprefix ../,$(source_dirs))
search_wildcards := $(addsuffix /*.cpp,$(source_dirs))

$(out_file): $(notdir $(patsubst %.cpp,%.o,$(wildcard $(search_wildcards))))
	$(compiler) $^ -o ../$@

VPATH := $(source_dirs)

%.o: %.cpp
	$(compiler) $(cfg) -c -MD $(addprefix -I,$(source_dirs)) $<

include $(wildcard *.d)
