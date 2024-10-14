#include <cmath>
#include <cstdint>
#include <cstddef>

class Allocator {
  char* mem;

  const int max_mem;
  const int CHUNK_SIZE = 64;
  const int NUM_CHUNKS = max_mem / (CHUNK_SIZE + sizeof(int8_t));
  const int START_DATA = NUM_CHUNKS * CHUNK_SIZE;

public:

  Allocator(size_t max_mem) : max_mem(max_mem) {
    // zero initialized by default
    // note sizeof(char) == 1
    mem = new char[max_mem];
  }

  ~Allocator() {
    delete[] mem;
  }

  void* alloc(size_t size) {
    const size_t needed = std::ceil((double) size / CHUNK_SIZE);

    int i = 0;
    int found = 0;
    while (i < NUM_CHUNKS) {
      if (mem[START_DATA + i]) {
        i += mem[START_DATA + i];
        found = 0;
      }
      else {
        ++found;
        ++i;

        if (found == needed) {
          int start = i - needed;
          mem[START_DATA + start] = size;

          return mem + CHUNK_SIZE * start;
        }
      }
    }

    // Cannot find room
    return nullptr;
  }

  void free(void* ptr) {
    int offset = ((char*)mem - (char*)ptr) / CHUNK_SIZE;

    mem[START_DATA + offset] = 0;
  }
};
