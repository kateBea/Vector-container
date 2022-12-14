#ifndef VECTOR_HH
#define VECTOR_HH

// C++ standard library includes
#include <new>
#include <cstdio>
#include <memory>
#include <cstring>
#include <cstdint>
#include <utility>
#include <iterator>
#include <algorithm>
#include <exception>
#include <string_view>
#include <initializer_list>

#define DEBUG_LOG(log_str)  std::cerr << log_str << '\n'

namespace kt
{
template <typename T>
class vector
{
public:
    using value_type            = T;
    using size_type             = std::size_t;
    using reference_type        = T&;
    using pointer_type          = T*;
    using const_reference_type  = const T&;

    class iterator
    {
    public:
        explicit iterator(pointer_type ptr) : p{ ptr } { }

        // prefix increment
        auto operator++() -> iterator
        {
            ++p;
            return *this;
        }

        // postfix increment
        auto operator++(int) -> iterator
        {
            auto res{ p };
            ++p;
            return iterator{ res };
        }

        // prefix increment
        auto operator--() -> iterator
        {
            --p;
            return *this;
        }

        // postfix increment
        auto operator--(int) -> iterator
        {
            auto res{ p };
            --p;
            return iterator{ res };
        }

        auto operator+(size_type count) -> iterator
        {
            return iterator{ this->p + count };
        }

        auto operator-(size_type count) -> iterator
        {
            return iterator{ this->p - count };
        }

        auto operator!=(const iterator& other) -> bool
        {
            return this->p != other.p;
        }

        auto operator==(const iterator& other) -> bool
        {
            return this->p == other.p;
        }

        auto operator*() -> reference_type { return *p; }
        auto operator->() -> pointer_type { return p; }

        auto raw() const -> pointer_type { return p; }

    private:
        pointer_type p{};
    };

    class const_iterator
    {
    public:
        explicit const_iterator(pointer_type ptr) : p{ ptr } { }

        // prefix increment
        auto operator++() -> const_iterator
        {
            ++p;
            return *this;
        }

        // postfix increment
        auto operator++(int) -> const_iterator
        {
            auto res{ p };
            ++p;
            return const_iterator{ res };
        }

        // prefix increment
        auto operator--() -> const_iterator
        {
            --p;
            return *this;
        }

        // postfix increment
        auto operator--(int) -> const_iterator
        {
            auto res{ p };
            --p;
            return const_iterator{ res };
        }

        auto operator+(size_type count) const -> const_iterator
        {
            return const_iterator{ this->p + count };
        }

        auto operator-(size_type count) const -> const_iterator
        {
            return const_iterator{ this->p - count };
        }

        auto operator!=(const const_iterator& other) const -> bool
        {
            return this->p != other.p;
        }

        auto operator==(const const_iterator& other) const -> bool
        {
            return this->p == other.p;
        }

        auto operator*() const -> const_reference_type { return *p; }
        auto operator->() const -> pointer_type { return p; }

        auto raw() const -> pointer_type { return p; }

    private:
        pointer_type p{};
    };

    ///
    /// Default constructor
    ///
    vector()
        :   m_array{ nullptr }, m_count{}, m_capacity{}
    {

    }

    ///
    /// Parametrized constructor. Reserve space to hold at least "count" elements
    ///
    vector(size_type count)
        :   m_array{ nullptr }, m_count{ 0 }, m_capacity{ count }
    {
        if (count != 0)
            this->m_array = static_cast<pointer_type>(::operator new(sizeof(value_type) * count, std::nothrow));

        if (not this->m_array)
        {
            std::printf("could not allocate block of memory...");
            this->m_capacity = 0;
        }
    }

    ///
    /// Parametrized constructor. Initializes vector
    /// with the elements from "content"
    ///
    vector(std::initializer_list<T>&& content)
        :   m_array{ static_cast<pointer_type>(::operator new(sizeof(value_type) * content.size(), std::nothrow)) },
            m_count{ content.size() }, m_capacity{ content.size() }
    {
        if (this->m_array)
            std::memcpy(static_cast<void*>(this->m_array), static_cast<const void*>(content.begin()),
                content.size() * sizeof(value_type));
            // std::copy(content.begin(), content.end(), this->m_array);
        else
        {
            std::printf("could not allocate block of memory...");
            this->m_count = 0;
            this->m_capacity = 0;
        }
    }

    ///
    /// Parametrized constructor. Initialize this vector with elements
    /// within the ranged given by "first" and "last"
    ///
    vector(iterator first, iterator last)
        :   m_array{ nullptr }, m_count{}, m_capacity{}
    {
        // represents the amount of bytes between first and last
        size_type new_block_size{ sizeof(value_type) * std::distance(first.raw(), last.raw()) };

        if (new_block_size != 0)
        {
            // new_block_size represents the size in bytes of the new block
            this->m_array = static_cast<pointer_type>(::operator new(new_block_size, std::nothrow));

            if (this->m_array)
            {
                std::memcpy(static_cast<void*>(this->m_array), static_cast<const void*>(first.raw()),
                    new_block_size );
                //std::copy(first.raw(), last.raw(), this->m_array);
                this->m_count = new_block_size / sizeof(value_type);
                this->m_capacity = new_block_size / sizeof(value_type);

            }
            else
            {
                std::printf("could not allocate block of memory...");
            }
        }
    }

    ///
    /// Parametrized constructor. Initialize this vector with "count"
    /// elements starting from "begin"
    ///
    vector(iterator first, size_type count)
        :   m_array{ nullptr }, m_count{ count }, m_capacity{ count }
    {
        // TODO: still needs testing
        if (count != 0)
        {
            this->m_array = static_cast<pointer_type>(::operator new(sizeof(value_type) * count, std::nothrow));

            if (this->m_array)
            {
                std::memcpy(static_cast<void*>(this->m_array), static_cast<const void*>(first.raw()),
                    count * sizeof(value_type));
                // std::copy(first.raw(), first.raw() + count, this->m_array);
                this->m_count = count;
                this->m_capacity = count;
            }
            else
            {
                std::printf("could not allocate block of memory...");
                this->m_count = 0;
                this->m_capacity = 0;
            }
        }
    }

    ///
    /// Copy constructor. Initialize this vector with elements from "other"
    ///
    vector(const vector& other)
        :   m_array{ nullptr }, m_count{}, m_capacity{}
    {
        if (other.m_count != 0)
        {
            this->m_array = static_cast<pointer_type>(::operator new(sizeof(value_type) * other.m_count, std::nothrow));

            if (this->m_array)
            {
                std::memcpy(static_cast<void*>(this->m_array), static_cast<const void*>(other.m_array),
                    other.m_count * sizeof(value_type));
                // std::copy(other.m_array, other.m_array + other.m_count, this->m_array);
                this->m_count = other.m_count;
                this->m_capacity = other.m_capacity;
            }
            else
            {
                std::printf("could not allocate block of memory...");
                this->m_count = 0;
                this->m_capacity = 0;
            }
        }
    }

    ///
    /// Assigment operator. Deep copy of "other"
    ///
    vector& operator=(const vector& other)
    {
        if (this != &other)
        {
            // clean up previous block
            for (size_type index{}; index < m_count; ++index)
                this->m_array[index].~T();
            ::operator delete(this->m_array);

            this->m_array = static_cast<pointer_type>(::operator new(sizeof(value_type) * other.m_count, std::nothrow));

            if (this->m_array)
            {
                std::memcpy(static_cast<void*>(this->m_array), static_cast<const void*>(other.m_array),
                    other.m_count * sizeof(value_type));
                // std::copy(other.m_array, other.m_array + other.m_count, this->m_array);
                this->m_count = other.m_count;
                this->m_capacity = other.m_capacity;
            }
            else
                std::printf("could not allocate block of memory...");
        }

        return *this;
    }

    ///
    /// Move constructor
    ///
    vector(vector&& other)
        :   m_array{ other.m_array }, m_count{ other.m_count }, m_capacity{ other.m_capacity }
    {
        if (other.m_capacity != 0)
        {
            other.m_array = nullptr;
            other.m_count = 0;
            other.m_capacity = 0;
        }
    }

    ///
    /// Destructor
    ///
    ~vector()
    {
        // cleanup
        for (size_type index{}; index < m_count; ++index)
            this->m_array[index].~T();

        ::operator delete(this->m_array);
    }

    ///
    /// Assigment operator
    ///
    vector& operator=(vector&& other)
    {
        if (this != &other)
        {
            this->m_array = other.m_array;
            this->m_count = other.m_count;
            this->m_capacity = other.m_capacity;

            other.m_array = nullptr;
            other.m_count = 0;
            other.m_capacity = 0;
        }

        return *this;
    }

    ///
    /// Amount of elements in the vector
    ///
    auto size() const -> size_type
    {
        return this->m_count;
    }

    ///
    /// Returns the size of the underlying block of memory held by this vector
    ///
    auto capacity() const -> size_type
    {
        return this->m_capacity;
    }

    ///
    /// Return true if this vector has no elements, fase otherwise
    ///
    auto empty() const -> bool
    {
        return this->m_count == 0;
    }

    ///
    /// Returns reference to element at postion "index"
    ///
    auto operator[](size_type index) -> reference_type
    {
        return this->m_array[index];
    }

    ///
    /// Returns constant reference to element at postion "index"
    ///
    auto operator[](size_type index) const -> const_reference_type
    {
        return this->m_array[index];
    }

    ///
    /// Return reference to element at postion "index" throws "out_of_bounds"
    /// exception if index is not within the range of valid elements
    /// or "empty_vector" if the vector has no elements
    ///
    auto at(size_type index) -> reference_type
    {
        try
        {
            if (this->m_count == 0)
                throw empty_vector{};
            if (not (index < this->m_count))
                throw out_of_bounds{};

        }
        catch (const out_of_bounds& oob)
        {
            std::printf("%s", oob.what());
        }
        catch (const empty_vector& ema)
        {
            std::printf("%s", ema.what());
        }
        catch(...)
        {
            std::printf("Other exceptions thrown");
        }

        return this->m_array[index];
    }

    ///
    /// Return constant reference to element at postion "index" throws "out_of_bounds"
    /// exception if index is not within the range of valid elements or "empty_vector"
    /// if the vector has no elements
    ///
    auto at(size_type index) const -> const_reference_type
    {
        try
        {
            if (this->m_count == 0)
                throw empty_vector{};
            if (not (index < this->m_count))
                throw out_of_bounds{};

        }
        catch (const out_of_bounds& oob)
        {
            std::printf("%s", oob.what());
        }
        catch (const empty_vector& ema)
        {
             std::printf("%s", ema.what());
        }
        catch(...)
        {
            std::printf("Other exceptions thrown");
        }

        return this->m_array[index];
    }

    ///
    /// Reserve a block of memory to hold count elements
    ///
    auto reserve(size_type count) -> void
    {
        // TODO

        // this function can be called at any point and
        // state of the vector in the program

    }

    ///
    /// Adjust vector to contain count elements
    ///
    auto resize(size_type count) -> void;

    ///
    /// Insert elements at the end
    ///
    template <typename... Args>
    auto emplace_back(Args&&... args) -> void
    {
        if (this->m_count == this->m_capacity)
            reallocate();

        // construct in place
        new(&this->m_array[this->m_count++]) value_type(std::forward<Args>(args)...);
    }

    ///
    /// Concatenate the contents of this vector and "other"
    /// This vector contains the result of the concatenation
    ///
    auto append(const vector& other) -> void
    {
        if (not other.empty())
        {
            pointer_type new_block{ static_cast<pointer_type>
                (::operator new(sizeof(value_type) * (this->m_count + other.m_count), std::nothrow)) };

            if (new_block)
            {
                std::memcpy(static_cast<void*>(new_block), static_cast<const void*>(this->m_array),
                    this->m_count * sizeof(value_type));
                std::memcpy(static_cast<void*>(new_block + this->m_count), static_cast<const void*>(other.m_array),
                    other.m_count * sizeof(value_type));

                ::operator delete(static_cast<void*>(this->m_array));

                this->m_array = new_block;
                this->m_count = this->m_count + other.m_count;
                this->m_capacity = this->m_count + other.m_count;

            }
            else
            {
                std::printf("failed to concatenate. Could not allocate block of memory...");
                return;
            }
        }
    }

    ///
    /// Destroy the last n elements. If there is less than
    /// count elements, it empties the vector
    ///
    auto remove_n(size_type count) -> void
    {
        if (count < this->m_count)
        {
            std::for_each(this->m_array,
                this->m_array + count, [](reference_type info) -> void { info.~T(); });

            this->m_count -= count;
        }
        else
        {
            std::for_each(this->m_array,
                this->m_array + this->m_count, [](reference_type info) -> void { info.~T(); });

            this->m_count = 0;
        }
    }

    ///
    /// Insert one element at the end of the vector
    ///
    auto push_back(const_reference_type info) -> void
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
            new(&this->m_array[this->m_count]) value_type(info);
            this->m_count += 1;
        }
        else
        {
            reallocate();

            // if reallocate fails, m_size will remain same as m_capacity
            // preventing from appending new element
            if (this->m_capacity == this->m_count)
            {
                std::printf("could not insert new element due to error while reallocating...");
                return;
            }

            new(&this->m_array[this->m_count]) value_type(info);
            this->m_count += 1;
        }
    }

    ///
    /// Insert one element at the end of the vector
    /// with support for move semantics
    ///
    auto push_back(T&& info) -> void
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
            new(&this->m_array[this->m_count]) value_type(std::move(info));
            this->m_count += 1;
        }
        else
        {
            reallocate();

            // if reallocate fails, m_size will remain same as m_capacity
            // preventing from appending new element
            if (this->m_capacity == this->m_count)
            {
                std::printf("could not insert new element due to error while reallocating...");
                return;
            }

            new(&this->m_array[this->m_count]) value_type(std::move(info));
            this->m_count += 1;
        }
    }

    ///
    /// Remove the last element from the vector
    ///
    auto pop_back() -> void
    {
        if (this->m_count != 0)
        {
            this->m_array[this->m_count - 1].~T();
            this->m_count -= 1;
        }

    }

    ///
    /// Remove all elements from the vector
    ///
    auto clear() -> void
    {
        std::for_each(this->m_array,
            this->m_array + this->m_count, [](T& info) -> void { info.~T(); });

        this->m_count = 0;
    }

    ///
    /// Returns an iterator to the beginning of the vector
    ///
    auto begin() -> iterator
    {
        return iterator{ this->m_array };
    }

    ///
    /// Returns an iterator to the element past of the vector
    ///
    auto end() -> iterator
    {
        return iterator{ this->m_array + this->m_count };
    }

    ///
    /// Returns a constant iterator to the beginning of the vector
    ///
    auto begin() const -> const_iterator
    {
        return const_iterator{ this->m_array };
    }

    ///
    /// Returns a constant iterator past the last element of the vector
    ///
    auto end() const -> const_iterator
    {
        return const_iterator{ this->m_array + this->m_count };
    }

    ///
    /// Returns a constant iterator to the beginning of the vector
    ///
    auto cbegin() const -> const_iterator
    {
        return const_iterator{ this->m_array };
    }

    ///
    /// Returns a constant iterator past the last element of the vector
    ///
    auto cend() const -> const_iterator
    {
        return const_iterator{ this->m_array + this->m_count };
    }


private:
    static constexpr size_type grow_factor{ 2 };

    void reallocate()
    {
        size_type new_block_count{ (!this->m_capacity) ? 1 : (this->m_capacity * grow_factor) };
        pointer_type new_block{ static_cast<pointer_type>(::operator new(sizeof(T) * new_block_count, std::nothrow)) };

        if (not new_block)
        {
            std::printf("Failed to allocate new block of memory");
            return;
        }

        std::memcpy(static_cast<void*>(new_block), static_cast<const void*>(this->m_array),
            this->m_count * sizeof(value_type));

        ::operator delete(static_cast<void*>(this->m_array));

        this->m_array = new_block;
        this->m_capacity = new_block_count;

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

}   // END KT NAMESPACE

#endif
