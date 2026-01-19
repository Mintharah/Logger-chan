#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <optional>
#include <vector>

template <typename T> class RingBuffer {
private:
  std::vector<std::optional<T>> buffer;
  size_t head;
  size_t tail;
  bool full;

public:
  explicit RingBuffer(size_t size)
      : buffer(size), head(0), tail(0), full(false) {}

  /* Removing copy */
  RingBuffer(const RingBuffer &) = delete;
  RingBuffer &operator=(const RingBuffer &) = delete;

  /* Only move */
  RingBuffer(RingBuffer &&) = default;
  RingBuffer &operator=(RingBuffer &&) = default;

  bool tryPush(T &&item) {
    if (full) {
      return false;
    }

    buffer[tail] = std::move(item);
    tail = (tail + 1) % buffer.size();
    full = (tail == head);
    return true;
  }

  bool tryPop(T &item) {
    if (isEmpty()) {
      return false;
    }

    item = std::move(*buffer[head]);
    buffer[head].reset();
    head = (head + 1) % buffer.size();

    full = false;
    return true;
  }

  bool isEmpty() const {
    return (!full && (head == tail));
  }

  bool isFull() const {
    return full;
  }
};

#endif