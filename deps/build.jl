# build liboneapi_support with C wrappers for C++ APIs

using Scratch, CMake_jll, oneAPI_Level_Zero_Headers_jll, Pkg

oneAPI = Base.UUID("8f75cd03-7ff8-4ecb-9b8f-daf728133b1b")

# get scratch directories
conda_dir = get_scratch!(oneAPI, "conda")
install_dir = get_scratch!(oneAPI, "deps")
rm(install_dir; recursive=true)

# install the toolchain
try
    using Conda
catch err
    # Sometimes, Conda fails to import because its environment is missing.
    # That's probably caused by a missing build, but Pkg should do that...
    Pkg.build("Conda")
    using Conda
end
if !isfile(joinpath(conda_dir, "condarc-julia.yml"))
    Conda.create(conda_dir)
    # conda#8850
    mkpath(joinpath(conda_dir, "conda-meta"))
    touch(joinpath(conda_dir, "conda-meta", "history"))
end
Conda.add("dpcpp_linux-64", conda_dir; channel="intel")
Conda.add("mkl-devel-dpcpp", conda_dir; channel="intel")

Conda.list(conda_dir)

run(pipeline(`nm -D $(Conda.lib_dir(conda_dir))/libmkl_sycl.so`, `grep clean_device_info`))

# XXX: isn't there a Conda package providing ze_api.hpp?
include_dir =  joinpath(oneAPI_Level_Zero_Headers_jll.artifact_dir, "include")

withenv("PATH"=>"$(ENV["PATH"]):$(Conda.bin_dir(conda_dir))",
        "LD_LIBRARY_PATH"=>Conda.lib_dir(conda_dir)) do
mktempdir() do build_dir
    run(```$(cmake()) -DCMAKE_CXX_FLAGS="-isystem $include_dir"
                      -DCMAKE_INSTALL_RPATH=$(Conda.lib_dir(conda_dir))
                      -DCMAKE_INSTALL_PREFIX=$install_dir
                      -S $(@__DIR__) -B $build_dir```)
    run(`$(cmake()) --build $(build_dir) --parallel $(Sys.CPU_THREADS)`)
    run(`$(cmake()) --install $(build_dir)`)
end
end
