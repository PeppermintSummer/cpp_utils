cc        := g++
name      := pro
workdir   := workspace
srcdir    := src
objdir    := objs
stdcpp    := c++11
cuda_home := /datav/software/trt8cuda112cudnn8
cpp_pkg   := /datav/software/trtpy/cpp-packages
syslib    := /datav/software/lib
cuda_arch := 

# 定义cpp的路径查找和依赖项mk文件
cpp_srcs := $(shell find $(srcdir) -name "*.cpp")
cpp_objs := $(cpp_srcs:.cpp=.cpp.o)
cpp_objs := $(cpp_objs:$(srcdir)/%=$(objdir)/%)
cpp_mk   := $(cpp_objs:.cpp.o=.cpp.mk)

# 定义cu文件的路径查找和依赖项mk文件
cu_srcs := $(shell find $(srcdir) -name "*.cu")
cu_objs := $(cu_srcs:.cu=.cu.o)
cu_objs := $(cu_objs:$(srcdir)/%=$(objdir)/%)
cu_mk   := $(cu_objs:.cu.o=.cu.mk)

# 定义opencv和cuda需要用到的库文件
link_opencv    := opencv_core opencv_imgproc opencv_imgcodecs
link_cuda      := cudart cudnn cublas cuda
link_trtpro    := protobuf
link_tensorRT  := nvinfer nvparsers nvinfer_plugin
link_sys       := stdc++ dl
link_librarys  := $(link_cuda) $(link_tensorRT) $(link_sys) $(link_trtpro) $(link_opencv)

# 定义cuda和opencv的库路径
nvcc              := $(cuda_home)/bin/nvcc -ccbin=$(cc)
include_cuda      := $(cuda_home)/include/cuda
include_tensorRT  := $(cuda_home)/include/tensorRT
include_protobuf  := $(cuda_home)/include/protobuf
include_opencv    := ${cpp_pkg}/opencv4.2/include

lib_all       := $(cuda_home)/lib64 $(trtpro_lib) $(syslib)
lib_opencv    := $(cpp_pkg)/opencv4.2/lib

include_paths := src    \
	src/tensorRT        \
	src/tensorRT/common \
	src/common          \
    $(include_cuda)     \
	$(include_tensorRT) \
	$(include_protobuf) \
	$(include_opencv)

library_paths := $(lib_all) $(lib_opencv)

empty := 
library_path_export := $(subst $(empty) $(empty),:,$(library_paths))

run_paths     := $(foreach item,$(library_paths),-Wl,-rpath=$(item))
include_paths := $(foreach item,$(include_paths),-I$(item))
library_paths := $(foreach item,$(library_paths),-L$(item))
link_librarys := $(foreach item,$(link_librarys),-l$(item))
defined       := $(foreach item,$(defined),-D$(item))

cpp_compile_flags := -std=$(stdcpp) -w -g -O0 -m64 -fPIC -fopenmp -pthread $(defined)
cu_compile_flags  := -std=$(stdcpp) -w -g -O0 -m64 $(cuda_arch) -Xcompiler "$(cpp_compile_flags)" $(defined)
link_flags        := -pthread -fopenmp -Wl,-rpath='$$ORIGIN'

cpp_compile_flags += $(include_paths)
cu_compile_flags  += $(include_paths)
link_flags        += $(library_paths) $(link_librarys) $(run_paths)

ifneq ($(MAKECMDGOALS), clean)
-include $(cpp_mk) $(cu_mk)
endif

$(name)   : $(workdir)/$(name)

all       : $(name)
run       : $(name)
	@cd $(workdir) && ./$(name) $(run_args)

$(workdir)/$(name) : $(cpp_objs) $(cu_objs)
	@echo Link $@
	@mkdir -p $(dir $@)
	@$(cc) $^ -o $@ $(link_flags)

$(objdir)/%.cpp.o : $(srcdir)/%.cpp
	@echo Compile CXX $<
	@mkdir -p $(dir $@)
	@$(cc) -c $< -o $@ $(cpp_compile_flags)

$(objdir)/%.cu.o : $(srcdir)/%.cu
	@echo Compile CUDA $<
	@mkdir -p $(dir $@)
	@$(nvcc) -c $< -o $@ $(cu_compile_flags)

$(objdir)/%.cpp.mk : $(srcdir)/%.cpp
	@echo Compile depends C++ $<
	@mkdir -p $(dir $@)
	@$(cc) -M $< -MF $@ -MT $(@:.cpp.mk=.cpp.o) $(cpp_compile_flags)
    
$(objdir)/%.cu.mk : $(srcdir)/%.cu
	@echo Compile depends CUDA $<
	@mkdir -p $(dir $@)
	@$(nvcc) -M $< -MF $@ -MT $(@:.cu.mk=.cu.o) $(cu_compile_flags)

clean :
	@rm -rf $(objdir) $(workdir)/$(name)
.PHONY : clean run $(name)
export LD_LIBRARY_PATH:=$(library_path_export)