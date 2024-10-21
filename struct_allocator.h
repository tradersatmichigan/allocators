#include <cstddef>
#include <cstdint>
#include <memory>

template<class C, size_t MAX_MEM>
class Allocator {
  struct Chunk {
    C arr[64];
    int64_t free;
    uint64_t next;
  };

  static constexpr int MAX_CHUNKS = MAX_MEM / sizeof(Chunk);

  Chunk* begin;
  uint64_t next;

public:
  Allocator() {
    // Notice we rely on this being zero initialized
    begin = new Chunk[MAX_CHUNKS];
    next = 0;
  }

  C* get() {
    // no free slots
    if (next >= MAX_CHUNKS) return nullptr;

    Chunk& c =  begin[next];
    /*
    * Notice we have an invariant that next must point to an availible 
    * chunk. therefore, we can take a chunk that says it isn't free to 
    * mean that it hasn't been initialized yet. This allows us to 
    * amortize the initialization over the calls to make it O(1)
    */
    if (!c.free) {
      c.free = (1LL << 63);
      c.free |= c.free - 1;
      c.next = next + 1;
    }

    // search chunk and update next
    for (int i = 0; i < 64; ++i) {
      if (c.free & (1LL << i)) {
        C* ptr = c.arr + i;
        c.free ^= (1LL << i);

        if (!c.free) {
          next = c.next;
        }

        return ptr;
      }
    }
    
    // unreachable
    exit(1);
  }

  void erase(C* ptr) {
    uint64_t offset = ((char*) ptr - (char*) begin) / sizeof(Chunk);
    Chunk& c = begin[offset];

    if (!c.free) {
      c.next = next;
      next = offset;
    }

    int internal_offset = ptr - c.arr;
    c.free ^= (1LL << internal_offset);
  }
};
