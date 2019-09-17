#pragma once

#include <eosio/vm/exceptions.hpp>

#include <utility>
#include <string>
#include <vector>

namespace eosio { namespace vm {
   template <typename T>
   class unmanaged_vector {
      public:
         constexpr unmanaged_vector(size_t size=0) : _data(size) {}
         constexpr unmanaged_vector(const unmanaged_vector& v) = delete;
         constexpr unmanaged_vector(unmanaged_vector&& v) : _data(std::move(v._data)), _index(v._index) {}

         constexpr unmanaged_vector& operator=(unmanaged_vector&& v) {
            _data = std::move(v._data);
            _index = v._index;
            return *this;
         }

         constexpr inline void resize( size_t size ) {
            _data.resize(size);
         }

         constexpr inline void push_back( const T& val ) {
            if ( _index >= _data.size() )
               resize( _data.size() ? _data.size() * 2 : 1 );
            _data[_index++] = val;
         }

         constexpr inline void emplace_back( T&& val ) {
            if ( _index >= _data.size() )
               resize( _data.size() ? _data.size() * 2 : 1 );
            _data[_index++] = std::move(val);
         }

         constexpr inline void back() {
            return _data[_index];
         }

         constexpr inline void pop_back() {
            EOS_VM_ASSERT( _index-- > 0, wasm_vector_oob_exception, "vector pop out of bounds" );
         }

         constexpr inline T& at( size_t i ) {
            // TODO: fix this to be more robust, assuming that the read 'i' will be in close to the max index
            // this is the usage pattern currently by eos-vm, a better solution will be created for the abstraction
            // of the various points of validation
            if ( i >= _data.size() )
               resize( _data.size() ? _data.size()*2 : 1);
            return _data[i];
         }

         constexpr inline const T& at( size_t i )const {
            EOS_VM_ASSERT( i < _data.size(), wasm_vector_oob_exception, "vector read out of bounds" );
            return _data[i];
         }

         constexpr inline const T& operator[] (size_t i) const { return at(i); }
         constexpr inline T& operator[] (size_t i) { return at(i); }
         constexpr inline T* raw() const { return _data.data(); }
         constexpr inline size_t size() const { return _data.size(); }

      private:
         std::vector<T> _data;
         size_t _index = 0;
   };

   template <typename T, typename Allocator> 
   class managed_vector {
      public:
         constexpr managed_vector(Allocator& allocator, size_t size=0) :
            _size(size),
            _allocator(&allocator),
            _data(allocator.template alloc<T>( _size )) {
         }

         constexpr managed_vector(const managed_vector& mv) = delete;
         constexpr managed_vector(managed_vector&& mv) {
            _size  = mv._size;
            _allocator = mv._allocator;
            _data  = mv._data;
         }

         constexpr managed_vector& operator=(managed_vector&& mv) {
            _size  = mv._size;
            _allocator = mv._allocator;
            _data  = mv._data;
            return *this;
         }

         constexpr inline void set_owner( Allocator& alloc ) { _allocator = &alloc; }
         constexpr inline void resize( size_t size ) {
            if (size > _size) {
               T* ptr = _allocator->template alloc<T>( size );
               if (_size == 0)
                 _data = ptr;
            } else {
               _allocator->template reclaim<T>( _data + size, _size - size );
            }
            _size = size;
         }

         constexpr inline void push_back( const T& val ) {
            EOS_VM_ASSERT( _index < _size, wasm_vector_oob_exception, "vector write out of bounds" );
            _data[_index++] = val;
         }

         constexpr inline void emplace_back( T&& val ) {
            EOS_VM_ASSERT( _index < _size, wasm_vector_oob_exception, "vector write out of bounds" );
            _data[_index++] = std::move(val);
         }

         constexpr inline void back() {
            return _data[_index];
         }

         constexpr inline void pop_back() {
            EOS_VM_ASSERT( _index-- > 0, wasm_vector_oob_exception, "vector pop out of bounds" );
         }

         constexpr inline T& at( size_t i ) {
            EOS_VM_ASSERT( i < _size, wasm_vector_oob_exception, "vector read out of bounds" );
            return _data[i];
         }

         constexpr inline T& at( size_t i )const {
            EOS_VM_ASSERT( i < _size, wasm_vector_oob_exception, "vector read out of bounds" );
            return _data[i];
         }

         constexpr inline T& at_no_check( size_t i ) {
            return _data[i];
         }

         constexpr inline T& at_no_check( size_t i ) const {
            return _data[i];
         }

         constexpr inline T& operator[] (size_t i) const { return at(i); }
         constexpr inline T* raw() const { return _data; }
         constexpr inline size_t size() const { return _size; }
         constexpr inline void set( T* data, size_t size, size_t index=-1 ) { _size = size; _data = data; _index = index == -1 ? size - 1 : index; }
         constexpr inline void copy( T* data, size_t size ) {
           resize(size);
           std::copy_n(data, size, _data);
           _index = size-1;
         }

      private:
         size_t _size  = 0;
         Allocator* _allocator = nullptr;
         T*     _data  = nullptr;
         size_t _index = 0;
   };

   template <typename T>
   std::string vector_to_string( T&& vec ) {
     std::string str;
     str.reserve(vec.size());
     for (int i=0; i < vec.size(); i++)
       str[i] = vec[i];
     return str;
   }
}} // namespace eosio::vm
