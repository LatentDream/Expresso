default:
  just --list

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
