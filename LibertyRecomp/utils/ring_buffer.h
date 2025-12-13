/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */
#pragma once

using ring_size_t = uint32_t;
class RingBuffer {
 public:
  RingBuffer(uint8_t* buffer, size_t capacity)
      : buffer_(buffer), capacity_(static_cast<ring_size_t>(capacity)), read_offset_(0), write_offset_(0) {}

  uint8_t* buffer() const { return buffer_; }
  ring_size_t capacity() const { return capacity_; }
  bool empty() const { return read_offset_ == write_offset_; }

  ring_size_t read_offset() const { return read_offset_; }
  uintptr_t read_ptr() const {
    return uintptr_t(buffer_) + static_cast<uintptr_t>(read_offset_);
  }

  // todo: offset/ capacity_ is probably always 1 when its over, just check and
  // subtract instead
  void set_read_offset(size_t offset) { read_offset_ = offset % capacity_; }

  ring_size_t write_offset() const { return write_offset_; }
  uintptr_t write_ptr() const { return uintptr_t(buffer_) + write_offset_; }
  void set_write_offset(size_t offset) {
    write_offset_ = static_cast<ring_size_t>(offset) % capacity_;
  }
  ring_size_t write_count() const {
    if (read_offset_ == write_offset_) {
      return capacity_;
    } else if (write_offset_ < read_offset_) {
      return read_offset_ - write_offset_;
    } else {
      return (capacity_ - write_offset_) + read_offset_;
    }
  }

  void AdvanceRead(size_t count);
  void AdvanceWrite(size_t count);

  struct ReadRange {
    const uint8_t* first;

    const uint8_t* second;
    ring_size_t first_length;
    ring_size_t second_length;
  };
  ReadRange BeginRead(size_t count);
  void EndRead(ReadRange read_range);

  size_t Read(uint8_t* buffer, size_t count);
  template <typename T>
  size_t Read(T* buffer, size_t count) {
    return Read(reinterpret_cast<uint8_t*>(buffer), count);
  }

  template <typename T>
  T Read() {
    static_assert(std::is_fundamental<T>::value,
                  "Immediate read only supports basic types!");

    T imm;
    size_t read = Read(reinterpret_cast<uint8_t*>(&imm), sizeof(T));
    assert(read == sizeof(T));
    return imm;
  }

  size_t Write(const uint8_t* buffer, size_t count);
  template <typename T>
  size_t Write(const T* buffer, size_t count) {
    return Write(reinterpret_cast<const uint8_t*>(buffer), count);
  }

  template <typename T>
  size_t Write(T& data) {
    return Write(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
  }

 private:
  uint8_t* buffer_ = nullptr;
  ring_size_t capacity_ = 0;
  ring_size_t read_offset_ = 0;
  ring_size_t write_offset_ = 0;
};