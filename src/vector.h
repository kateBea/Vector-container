#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <new>
#include <cstring>
#include <exception>
#include <cstdint>
#include <utility>
#include <initializer_list>
#include <string_view>
#include <algorithm>

#define DEBUG_LOG(log_str)  std::cerr << log_str << '\n'

namespace kt
{
template <typename T>
class vector
{
public:
    using size_type = std::size_t;
    using index_type = std::size_t;
    using pointer_type = T*;

    ///
    /// default constructor
    ///
    vector()
        :   m_array{ nullptr }, m_count{}, m_capacity{}
    {

    }

    ///
    /// parametrized constructor. Takes block size in number of bytes
    ///
    vector(size_type block_size)
        :   m_array{ nullptr }, m_count{ block_size }, m_capacity{ block_size }
    {
        if (block_size != 0)
        {
            this->m_array = new (std::nothrow) T[this->m_count]{};
        }
    }

    ///
    /// parametrized constructor. Initializes the vector from
    /// with the elements from the list
    ///
    vector(std::initializer_list<T>&& content)
        :   m_array{ new (std::nothrow) T[content.size()] }, m_count{ content.size() }, m_capacity{ content.size() }
    {
        if (this->m_array)
            std::copy(content.begin(), content.end(), this->m_array);
    }

    ///
    /// copy constructor. Realocates new block and initializes it
    /// with a copy of the elements from p_vector
    ///
    vector(const vector& p_vector)
        :   m_array{ nullptr }, m_count{}, m_capacity{}
    {
        if (p_vector.m_count != 0)
        {
            this->m_array = new (std::nothrow) T[p_vector.m_count];
            this->m_count = p_vector.m_count;
            this->m_capacity = p_vector.m_capacity;

            if (this->m_array != nullptr)
                std::copy(p_vector.m_array, p_vector.m_array + p_vector.m_count,
                    this->m_array);
        }
    }

    ///
    /// assigment operator by deep copy
    ///
    vector& operator=(const vector& p_vector)
    {
        if (this != &p_vector)
        {
            // clean up previous block
            delete[] this->m_array;

            this->m_array = new (std::nothrow) T[p_vector.m_count];
            this->m_count = p_vector.m_count;

            if (this->m_array != nullptr)
                std::copy(p_vector.m_array, p_vector.m_array + p_vector.m_count,
                    this->m_array);
        }

        return *this;
    }

    ///
    /// move constructor
    ///
    vector(vector&& p_vector)
        :   m_array{ p_vector.m_array }, m_count{ p_vector.m_count }
    {
        if (p_vector.m_count != 0)
        {
            p_vector.m_array = nullptr;
            p_vector.m_count = 0;
        }
    }

    ///
    /// destructor
    ///
    ~vector()
    {
        // cleanup
        delete[] this->m_array;
    }

    ///
    /// assigment operator
    ///
    vector& operator=(vector&& p_vector)
    {
        if (this != &p_vector)
        {
            this->m_array = p_vector.m_array;
            this->m_count = p_vector.m_count;

            p_vector.m_array = nullptr;
            p_vector.m_count = 0;
        }

        return *this;
    }

    ///
    /// amount of elements in the vector
    ///
    auto size() const -> size_type
    {
        return this->m_count;
    }

    ///
    /// returns the size of the block of memory held by the vector
    ///
    auto capacity() const -> size_type
    {
        return this->m_capacity;
    }

    ///
    /// return true if the vector is empty, false otherwise
    ///
    auto empty() const -> bool
    {
        return this->m_array == nullptr;
    }

    ///
    /// return reference to element at postion "index"
    ///
    auto operator[](size_type index) -> T&
    {
        return this->m_array[index];
    }

    ///
    /// return constant sreference to element at postion "index"
    ///
    auto operator[](size_type index) const -> const T&
    {
        return this->m_array[index];
    }

    ///
    /// return constant sreference to element at postion "index"
    /// throws exceptions if index is not valid or vector is empty
    ///
    auto at(size_type index) -> T&
    {
        try
        {
            if (this->m_count == 0)
                throw empty_vector{};
            if (not (index > 0 and index < this->m_count))
                throw out_of_bounds{};

        }
        catch (const out_of_bounds& oob) { std::cerr << oob.what() << '\n'; }
        catch (const empty_vector& ema) { std::cerr << ema.what() << '\n'; }
        catch(...) { std::cerr << "Other exceptions thrown\n"; }

        return this->m_array[index];
    }

    ///
    /// reserve a block of memory
    ///
    auto reserve(size_type block_size) -> void
    {
        // keep in mind this function can be called at any point and state of
        // the vector in the program

    }

    ///
    /// Insert one element at the end of the vector
    ///
    auto push_back(const T& info) -> void
    {
        // if capacity == count:
        // allocate new block
        // copy data from old block to new block
        // add new data
        // delete old block

        // otherwise (capacity > count)
        // put new element at the end
        // increase count by 1
        if (this->m_capacity > this->m_count)
        {
            m_array[m_count] = info;
            this->m_count += 1;
        }
        else
        {
            reallocate();

            this->m_array[m_count] = info;
            this->m_count += 1;
        }
    }

    auto pop_back() -> void
    {
        this->m_count -= (this->m_count == 0) ? 0 : 1;
        this->m_array[m_count - 1].~T();
    }

    auto clear() -> void
    {
        delete[] this->m_array;

        this->m_capacity = 0;
        this->m_count = 0;
        this->m_array = nullptr;
    }

    auto resize(size_type block_size) -> void;

    //auto begin() -> some_terator;
    //auto end() -> some_iterator;

    //auto cbegin() -> some_terator;
    //auto cend() -> some_iterator;

private:
    void reallocate()
    {
        size_type new_block_size{ (!this->m_capacity) ? 1 : (m_capacity * 2) };
        pointer_type new_block{ new (std::nothrow) T[new_block_size]{} };

        if (not new_block)
        {
            std::cerr << "Failed to allocate new lock of memory\n";
            return;
        }

        std::copy(this->m_array, this->m_array + this->m_count, new_block);

        delete[] this->m_array;

        this->m_array = new_block;
        this->m_capacity = new_block_size;

    }

    class out_of_bounds : public std::exception
    {
    public:
        out_of_bounds() = default;
        ~out_of_bounds() = default;

        const char* what() const noexcept override
        {
            return this->m_exc.data();
        }

    private:
        std::string_view m_exc{ "[out_of_bounds]: Index out of bounds\n" };
    };

    class empty_vector : public std::exception
    {
    public:
        empty_vector() = default;
        ~empty_vector() = default;

        const char* what() const noexcept override
        {
            return this->m_exc.data();
        }

    private:
        std::string_view m_exc{ "[empty_vector]: vector has no elements\n" };
    };

    pointer_type m_array;
    size_type m_count;
    size_type m_capacity;

    // CONSTRAINTS:
    // m_capacity >= m_count >= 0
};

}   // END KATE NAMESPACE
#endif // END VECTOR