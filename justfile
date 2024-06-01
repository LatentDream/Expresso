default:
  just --list

prog := "./bin/linux/debug/Expresso"

cpu-profile:
    valgrind --tool=callgrind --callgrind-out-file=callgrind.out --collect-jumps=yes --simulate-cache=yes {{prog}}

cpu-profile-check:
    kcachegrind callgrind.out

hyperfine:
    hyperfine {{prog}}

mem-check:
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes {{prog}}

mem-profile:
    heaptrack -o /tmp/heaptrack.pbqdd.%p.zst {{prog}}

mem-profile-check:
    heaptrack_gui "/tmp/heaptrack.pbqdd.%p.zst.gz"
