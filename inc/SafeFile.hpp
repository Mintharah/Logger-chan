#ifndef SAFE_FILE
#define SAFE_FILE

#include <unistd.h>

class SafeFile {
private:
  int fd = -1;

public:
  SafeFile() = default;
  explicit SafeFile(int fd);

  SafeFile(const SafeFile&) = delete; // copy
  SafeFile& operator=(const SafeFile&) = delete;

  SafeFile(SafeFile&& other); // move
  SafeFile& operator=(SafeFile&& other);

  ~SafeFile();

  bool isOpen() const;
  int get() const;
};

#endif