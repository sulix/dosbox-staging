libfpu = static_library('fpu', ['fpu.cpp'], include_directories : incdir, dependencies : [spdlog_dep])

libfpu_dep = declare_dependency(link_with : libfpu)

internal_deps += libfpu_dep
