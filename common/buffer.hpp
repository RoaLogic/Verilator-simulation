/////////////////////////////////////////////////////////////////////
//   ,------.                    ,--.                ,--.          //
//   |  .--. ' ,---.  ,--,--.    |  |    ,---. ,---. `--' ,---.    //
//   |  '--'.'| .-. |' ,-.  |    |  |   | .-. | .-. |,--.| .--'    //
//   |  |\  \ ' '-' '\ '-'  |    |  '--.' '-' ' '-' ||  |\ `--.    //
//   `--' '--' `---'  `--`--'    `-----' `---' `-   /`--' `---'    //
//                                             `---'               //
//    Base Class for Circular Buffers                              //
//                                                                 //
/////////////////////////////////////////////////////////////////////
//                                                                 //
//             Copyright (C) 2023 Roa Logic BV                     //
//             www.roalogic.com                                    //
//                                                                 //
//     This source file may be used and distributed without        //
//   restriction provided that this copyright statement is not     //
//   removed from the file and that any derivative work contains   //
//   the original copyright notice and the associated disclaimer.  //
//                                                                 //
//      THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY        //
//   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED     //
//   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     //
//   FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR        //
//   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,           //
//   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES      //
//   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE     //
//   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR          //
//   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    //
//   LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT    //
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT    //
//   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           //
//   POSSIBILITY OF SUCH DAMAGE.                                   //
//                                                                 //
/////////////////////////////////////////////////////////////////////
/*!
 * @file buffer.hpp
 * @author Richard Herveille
 * @brief UniqueId base class
 * @version 0.1
 * @date 17-may-2023
 * @copyright See beginning of file
 */

#ifndef BUFFER_HPP
#define BUFFER_HPP

#define BUFFER_HPP

#include <atomic>

namespace RoaLogic
{
namespace common
{
    /**
     * The Iterator
     */
    template <class T> class ringbufferIterator;

 
    /**
     * @class ringbuffer
     * @author Richard Herveille
     * @brief Template class for a circular buffer
     * @version 0.1
     * @date 17-may-2023
     *
     * @details This is a template class for a circular buffer
     * 
     */
    template <class T=uint8_t> class ringbuffer
    {
        protected:
            using buffer_t = std::unique_ptr<T[]>;

            std::mutex   _mutex;    //semaphore
            buffer_t     _buffer;   //buffer of elements
            const size_t _maxSize;  //maximum number of elements
            size_t       _head;     //write pointer
            size_t       _tail;     //read pointer
            bool         _full;     //buffer full
            bool         _empty;    //buffer empty

        public:
            using iterator = ringbufferIterator<T>;
            friend class ringbufferIterator<T>;

            /**
             * @brief Constructor
             */
            ringbuffer(size_t size) : _buffer(std::unique_ptr<T[]>(new T[size])),
                                      _maxSize(size),
                                      _head(0),
                                      _tail(0),
                                      _full(false),
                                      _empty(true) {}


            /**
             * @brief Destructor
             */
            ~ringbuffer() { }


            /**
             * @brief Clears buffer
             */
            virtual void clear() {
                std::lock_guard<std::mutex> lock(_mutex);
                _head =0;
                _tail =0;
                _full =false;
                _empty=true;
            }


            /**
             * @brief  Is the buffer full?
             * @return true when buffer is full, false otherwise
             */ 
            virtual bool full() const { return _full; }


            /**
             * @brief  Is the buffer empty?
             * @return true when buffer is empty, false otherwise
             */ 
            virtual bool empty() const { return _empty; }


            /**
             * @brief  The maximum number of elements that can be stored in the buffer
             * @return Maximum number of elements that can be stored in the buffer
             */
            virtual size_t max_size() const { return _maxSize; }


            /**
             * @brief  Returns the number of elements stored in the buffer
             * @return The number of elements in the buffer
             */
            size_t size() {
                if (full()) return max_size;
                if (_head >= _tail) return (_head - _tail);
                return (max_size() + _head - _tail);
            }


            /**
             * @brief Insert element at the end of the buffer
             *
             * @param &data           Element to insert into the buffer
             * @param OverwriteOnFull Specify behaviour when the buffer is full. When true overwrite oldest data, otherwise ignore the request
             */
            virtual void push_back(T& data, bool OverwriteOnFull=true) {
                std::lock_guard<std::mutex> lock(_mutex);

                //check full and OverWriteOnFull
                if (full() && !OverwriteOnFull) return;

                //Insert element in buffer
                _buffer[_head++] = data;

                //check buffer overrung
                if (_head == max_size()) _head = 0;

                //update flags
                _empty = false;
                _full  = _head == _tail;
            }


            /**
             * @brief  Retrieve element from the front of the buffer
             * @return T() if the buffer is empty, otherwise the next element in the buffer
             */
            virtual T pop_front() {
                std::lock_guard<std::mutex> lock(_mutex);

                //return empty element if the buffer is empty
                if (empty()) return T();

                //retrieve element from buffer
                auto element = _buffer[_tail++];

                //check buffer overrun
                if (_tail == max_size()) _tail = 0;

                //update flags
                _full  = false;
                _empty = _tail == _head;

                //return result
                return element;
            }


            /**
             * @brief First element
             * @return Reference to the first element
             */
            T& front() { 
                if (empty()) return &T();
                return &_buffer[_tail];
            }

            T& front() const {
                if (empty()) return &T();
                return &_buffer[_tail];
            }


            /**
             * @brief  Iterator begin
             * @return Iterator pointing to the head of the ring buffer
             */
            iterator begin() const { return iterator(*this, _head); }

            /**
             * @brief  Iterator end
             * @return Iterator pointing to the tail of the ring buffer
             */
            iterator end() const {
                if (full()) return iterator(*this, _tail = 0 ? max_size() -1 : _tail -1);
                return iterator(*this, _tail);
            }
    };





    /**
     * @class ringbufferIter
     * @author Richard Herveille
     * @brief Iterator class for a circular buffer
     * @version 0.1
     * @date 17-may-2023
     *
     * @details This is the Iterator class for a circular buffer
     * 
     */
    template <class T> class ringbufferIterator
    {
        private:
            ringbuffer<T>& _rb;
            size_t& _ptr;

        public:
            using iterator = ringbufferIterator<T>;
            using const_iterator = const ringbufferIterator<T>;
            using difference_type = ptrdiff_t;
            using size_type = size_t;
            using value_type = T;
            using pointer = value_type*;
            using const_pointer = const value_type*;
            using reference = value_type&;

            /**
             * @brief  Constructor
             */
            ringbufferIterator(ringbuffer<T>& ringBuffer, size_t& ptr) : _rb(ringBuffer), _ptr(ptr) {}

            reference operator*() const { return &_rb[_ptr]; }
            pointer operator->() { return _rb[_ptr]; }

            //addition
            /*
            iterator& operator+(size_t N) {
                _ptr += N;

                while (_ptr >= _rb.max_size()) _ptr -= _rb.max_size();
                while (_ptr < 0) _ptr += _rb.max_size();
            }
            */

            //prefix increment
            iterator& operator++() {
                //advance
                if (++_ptr == _rb.max_size()) _ptr = 0;

                //return iterator
                return *this;
            }

            //postfix operator
            iterator operator++(int) {
                //store current state
                iterator tmp = *this;

                //call prefix increment
                ++(*this);

                //return state
                return tmp;
            }
 
            //equality operators
            friend bool operator==(const iterator& a, const iterator& b) { return a._rb == b._rb; }
            friend bool operator!=(const iterator& a, const iterator& b) { return !(a==b);        } 
    };


}
}

#endif
