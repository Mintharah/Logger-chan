#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <mutex>
#include <optional>
#include <vector>

template <typename T> class RingBuffer {
private:
  std::vector<std::optional<T>> buffer;
  size_t head;
  size_t tail;
  bool full;
  mutable std::mutex mx;

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
    std::lock_guard<std::mutex> lock(mx);

    if (full) {
      return false;
    }

    buffer[tail] = std::move(item);
    tail = (tail + 1) % buffer.size();
    full = (tail == head);
    return true;
  }

  bool tryPop(T &item) {
    std::lock_guard<std::mutex> lock(mx);

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

  bool isFull() const { return full; }

  size_t size() const {
    //std::lock_guard<std::mutex> lock(mx);
    if (full)
      return buffer.size();
    if (tail >= head)
      return tail - head;
    return buffer.size() - head + tail;
  }
};

#endif