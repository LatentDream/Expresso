default:
  just --list

prog := "./bin/linux/debug/Expresso"
prog-release := "./bin/linux/release/Expresso"

# Build ========================================================================

build:
    cd external/libquattro; cargo build --release
    cp -fi external/libquattro/target/release/libquattro.so external/libquattro.so
    make release=1

build-debug:
    cd external/libquattro; cargo build
    cp -fi external/libquattro/target/debug/libquattro.so external/libquattro.so
    make

# Clean ========================================================================

clean:
    cd external/libquattro; cargo clean
    make clean

# Run ==========================================================================

run:
    # Add the current directory to the library path
    LD_LIBRARY_PATH=include {{prog-release}}

run-debug:
    # Add the current directory to the library path
    LD_LIBRARY_PATH=include {{prog}}

# Profiling and benchmarking ===================================================

cpu-profile:
    build-debug
    valgrind --tool=callgrind --callgrind-out-file=callgrind.out --collect-jumps=yes --simulate-cache=yes {{prog}}

cpu-profile-check:
    build-debug
    kcachegrind callgrind.out

hyperfine:
    build
    hyperfine {{prog-release}}

mem-check:
    build-debug
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes {{prog}}

mem-profile:
    build-debug
    heaptrack -o /tmp/heaptrack.pbqdd.%p.zst {{prog}}

mem-profile-check:
    build-debug
    heaptrack_gui "/tmp/heaptrack.pbqdd.%p.zst.gz"
