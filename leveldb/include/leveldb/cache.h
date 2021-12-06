// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// A Cache is an interface that maps keys to values.  It has internal
// synchronization and may be safely accessed concurrently from
// multiple threads.  It may automatically evict entries to make room
// for new entries.  Values have a specified charge against the cache
// capacity.  For example, a cache where the values are variable
// length strings, may use the length of the string as the charge for
// the string.
//
// A builtin cache implementation with a least-recently-used eviction
// policy is provided.  Clients may use their own implementations if
// they want something more sophisticated (like scan-resistance, a
// custom eviction policy, variable cache sizing, etc.)

#ifndef STORAGE_LEVELDB_INCLUDE_CACHE_H_
#define STORAGE_LEVELDB_INCLUDE_CACHE_H_

#include <cstdint>

#include "leveldb/export.h"
#include "leveldb/slice.h"

namespace leveldb {

class LEVELDB_EXPORT Cache;

// Create a new cache with a fixed size capacity.  This implementation
// of Cache uses a least-recently-used eviction policy.
LEVELDB_EXPORT Cache* NewLRUCache(size_t capacity);

class LEVELDB_EXPORT Cache {
 public:
  Cache() = default;
  //说明cache不能被复制
  Cache(const Cache&) = delete;
  Cache& operator=(const Cache&) = delete;

  // Destroys all existing entries by calling the "deleter"
  // function that was passed to the constructor.
  //既然是支持继承，那么xigou函数肯定是要虚，否则会内存泄漏
  virtual ~Cache();

  // Opaque handle to an entry stored in the cache.
  //空对象，可以
  struct Handle {};

  //纯虚函数，子类必须要进行覆盖，主要作用是将数据插入到缓存中
  //1. 针对特殊的数据，可能需要特别的析构函数，因此提供deleter回调函数
  //2. 针对返回值，需要手动的调用this->Release(handle)函数，来释放
  virtual Handle* Insert(const Slice& key, void* value, size_t charge,
                         void (*deleter)(const Slice& key, void* value)) = 0;

  //针对返回值必须要调用this->Release(handle)函数进行释放
  virtual Handle* Lookup(const Slice& key) = 0;

  //释放handle句柄，具体原因我们在后面会进行讲解
  virtual void Release(Handle* handle) = 0;

  //获取key对应的值
  virtual void* Value(Handle* handle) = 0;

  //从缓存中删除指定的key
  virtual void Erase(const Slice& key) = 0;

  //缓存id，可能不同的客户端会共用相同的cache
  virtual uint64_t NewId() = 0;

 // 删除所有不活跃的 entry。
  //内存受限的应用程序可能希望调用此方法以减少内存使用。
  virtual void Prune() {}

  //计算整个缓存中的所有元素的大小
  virtual size_t TotalCharge() const = 0;

 private:
 //从in-use链表中移除
  void LRU_Remove(Handle* e);
  //从从in-use链表中移动到普通的表中
  void LRU_Append(Handle* e);
  //因为可能会被多个地方引用，所以不能简单的删除，涉及到引用计数
  void Unref(Handle* e);
  //真正的数据
  struct Rep;
  Rep* rep_;
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_CACHE_H_
