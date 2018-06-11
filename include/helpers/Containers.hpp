#ifndef UTILITY_CONTAINERS_HPP
#define UTILITY_CONTAINERS_HPP
#include <memory>
#include <numeric>
#include <cmath>

namespace helpers::containers
{
    template<typename T>
    struct Vector2D
    {
        using value_type = T;
        Vector2D()
        : x(T()), y(T())
        {}
        template<typename ValueType>
        Vector2D(ValueType _x, ValueType _y) : x(static_cast<T>(_x)), y(static_cast<T>(_y)) {}
        Vector2D(const Vector2D& pt) { *this = pt; }
        Vector2D& operator=(const Vector2D& pt)
        {
            if (this != &pt)
            {
                x = pt.x;
                y = pt.y;
            }
            return *this;
        }

        Vector2D(Vector2D&& pt) noexcept { *this = std::move(pt); }
        Vector2D& operator=(Vector2D&& pt) noexcept
        {
            if (this != &pt)
            {
                x = std::move(pt.x);
                y = std::move(pt.y);
            }
            return *this;
        }
        template<typename ValueType>
        Vector2D& operator*=(const ValueType& v)
        {
            x *= static_cast<T>(v);
            y *= static_cast<T>(v);
            return *this;
        }
        template<typename ValueType>
        Vector2D& operator/=(const ValueType& v)
        {
            x /= static_cast<T>(v);
            y /= static_cast<T>(v);
            return *this;
        }
        template<typename ValueType>
        Vector2D& operator+=(const Vector2D<ValueType>& pt)
        {
            x += static_cast<T>(pt.x);
            y += static_cast<T>(pt.y);
            return *this;
        }
        template<typename ValueType>
        Vector2D& operator-=(const Vector2D<ValueType>& pt)
        {
            x -= static_cast<T>(pt.x);
            y -= static_cast<T>(pt.y);
            return *this;
        }

        // TODO: it may be more generic with std::common_type
        template<typename ValueType>
        friend Vector2D operator*(const Vector2D& lpt, const ValueType& v)
        {
            Vector2D pt = lpt;
            pt *= v;
            return pt;
        }
        template<typename ValueType>
        friend Vector2D operator/(const Vector2D& lpt, const ValueType& v)
        {
            Vector2D pt = lpt;
            pt /= v;
            return pt;
        }
        template<typename ValueType>
        friend Vector2D operator+(const Vector2D& lpt, const Vector2D<ValueType>& rpt)
        {
            Vector2D pt = lpt;
            pt += rpt;
            return pt;
        }

        template<typename ValueType>
        friend Vector2D operator-(const Vector2D& lpt, const Vector2D<ValueType>& rpt)
        {
            Vector2D pt = lpt;
            pt -= rpt;
            return pt;
        }

        friend Vector2D operator-(const Vector2D& pt)
        {
            Vector2D pt_ = pt;
            pt_.x = -pt.x;
            pt_.y = -pt.y;
            return pt_;
        }

        friend bool operator==(const Vector2D& lhs, const Vector2D& rhs)
        {
            return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
        }

        friend bool operator!=(const Vector2D& lhs, const Vector2D& rhs)
        {
            return !(lhs == rhs);
        }

        T length() const
        {
            return static_cast<T>(std::sqrt(x*x + y*y));
        }

        T x;
        T y;
    };

    template <class T>
    using Size2D = Vector2D<T>;

    template <class T>
    using Point2D = Vector2D<T>;

    template <class T>
    struct Rect2D
    {
        using value_type = T;
        Rect2D() = default;
        Rect2D(const Point2D<T>& top_left, const Point2D<T>& bottom_right)
        : top_left(top_left), bottom_right(bottom_right)
        {}

        Rect2D(const T& top, const T& left, const T& bottom, const T& right)
        : top_left(Point2D<T>(left, top)),
          bottom_right(Point2D<T>(right, bottom))
        {}

        Rect2D(const Rect2D& r)
        {
            *this = r;
        }
        Rect2D& operator=(const Rect2D& r)
        {
            if (this != &r)
            {
                top_left = r.top_left;
                bottom_right = r.bottom_right;
            }
            return *this;
        }

        friend bool operator==(const Rect2D& lhs, const Rect2D& rhs)
        {
            return ((lhs.top_left == rhs.top_left) && (lhs.bottom_right == rhs.bottom_right));
        }
        friend bool operator!=(const Rect2D& lhs, const Rect2D& rhs)
        {
            return !(lhs == rhs);
        }

        // intersection
        friend bool operator&&(const Rect2D& lhs, const Rect2D& rhs)
        {
            if (lhs.bottom_right.x < rhs.top_left.x || rhs.bottom_right.x < lhs.top_left.x)
            {
                return false;
            }
            if (lhs.bottom_right.y < rhs.top_left.y || rhs.bottom_right.y < lhs.top_left.y)
            {
                return false;
            }
            return true;
        }

        T width() const
        {
            return bottom_right.x - top_left.x;
        }
        T height() const
        {
            return bottom_right.y - top_left.y;
        }
        Point2D<T> top_left;
        Point2D<T> bottom_right;
    };

    using AABB = Rect2D<uint32_t>;

    namespace
    {
        template<class IteratorType>
        class iterator_impl
        {
            using self_type = iterator_impl;
            using MatrixType = typename std::remove_cv_t<IteratorType>::higher_dimension_proxy;
            void check_traits() noexcept
            {
                static_assert(std::is_default_constructible_v<self_type>);
                static_assert(std::is_copy_constructible_v<self_type>);
                static_assert(std::is_copy_assignable_v<self_type>);
                static_assert(std::is_destructible_v<self_type>);
            }

        public:
            using value_type        = IteratorType;
            using pointer           = value_type*;
            using reference         = value_type&;
            using iterator_category = std::random_access_iterator_tag;
            using difference_type   = typename MatrixType::size_type;

            iterator_impl() noexcept
            {
                check_traits();
            }

            iterator_impl(const MatrixType& mat, difference_type idx) noexcept
            : _data(mat[idx])
            {
                check_traits();
                _current_ptr = _data.data_begin();
                _begin_ptr   = mat.data_begin();
            }

            self_type& operator=(const self_type& it) noexcept
            {
                _data        = it._data;
                _current_ptr = it._current_ptr;
                _begin_ptr   = it._begin_ptr;
                return *this;
            }

            difference_type idx() noexcept { return (_begin_ptr - _current_ptr) / _data._dimension_size; }
            reference operator*() noexcept { _data._data = _current_ptr; return _data; }
            pointer operator->() noexcept { _data._data = _current_ptr; return &_data; }
            self_type& operator++() noexcept { _current_ptr += _data._dimension_size; return *this; }
            self_type& operator--() noexcept { _current_ptr -= _data._dimension_size; return *this; }
            self_type operator++(int) noexcept { self_type i = *this; ++(*this); return i; }
            self_type operator--(int) noexcept { self_type i = *this; --(*this); return i; }
            self_type& operator+=(difference_type n) noexcept { _current_ptr += _data._dimension_size * n; return *this; }
            self_type& operator-=(difference_type n) noexcept { _current_ptr -= _data._dimension_size * n; return *this; }
            self_type operator+(difference_type n) noexcept { self_type i = *this; return i += n; }
            self_type operator-(difference_type n) noexcept { self_type i = *this; return i -= n; }
            friend difference_type operator-(const self_type& lhs, const self_type& rhs) noexcept { return (lhs._current_ptr - rhs._current_ptr) / lhs._data._dimesion_size; }
            friend self_type operator+(difference_type n, const self_type& rhs) noexcept { return rhs + n; }
            friend self_type operator-(difference_type n, const self_type& rhs) noexcept { return rhs - n; }
            friend bool operator<(const self_type& a, const self_type& b) noexcept { return (a - b) > 0; }
            friend bool operator>(const self_type& a, const self_type& b) noexcept { return b < a; }
            friend bool operator>=(const self_type& a, const self_type& b) noexcept { return !(a < b); }
            friend bool operator<=(const self_type& a, const self_type& b) noexcept { return !(a > b); }
            reference operator[](difference_type n) noexcept { return *(*this + n); }
            bool operator==(const self_type& rhs) noexcept { return (_current_ptr == rhs._current_ptr); }
            bool operator!=(const self_type& rhs) noexcept { return !(*this == rhs); }
        private:
            typename MatrixType::pointer       _current_ptr = nullptr;
            typename MatrixType::const_pointer _begin_ptr   = nullptr;
            std::remove_cv_t<value_type>       _data;
        };
    }

    template<typename T, size_t DIMS>
    class ProxyMatrix
    {
    public:
        using value_type      = T;
        using size_type       = std::size_t;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;

        using lower_dimension_proxy  = ProxyMatrix<T, DIMS - 1>;
        using higher_dimension_proxy = ProxyMatrix<T, DIMS + 1>;
        // TODO: difference_type, reverse_iterator, const_reverse_iterator;
        using iterator        = std::conditional_t<(DIMS >
                                                    1), iterator_impl<lower_dimension_proxy>, pointer>;
        using const_iterator  = std::conditional_t<(DIMS >
                                                    1), iterator_impl<const lower_dimension_proxy>, const_pointer>;
        using return_type = std::conditional_t<(DIMS > 1), lower_dimension_proxy, reference>;
    protected:
        using Dimensions = std::array<size_type, DIMS>;

        friend higher_dimension_proxy;
        friend iterator_impl<lower_dimension_proxy>;
        friend iterator_impl<const lower_dimension_proxy>;
        friend iterator_impl<ProxyMatrix>;
        friend iterator_impl<const ProxyMatrix>;
    public:
        ProxyMatrix(const ProxyMatrix& mat) noexcept;
        ProxyMatrix& operator=(const ProxyMatrix& mat) noexcept;
        ProxyMatrix(ProxyMatrix&& mat) noexcept;
        ProxyMatrix& operator=(ProxyMatrix&& mat) noexcept;

        ProxyMatrix(pointer data, std::initializer_list<size_type> dimensions);
        virtual ~ProxyMatrix() noexcept;
        size_type dimension_size() const noexcept;
        size_type dimension_size(size_type n) const noexcept;
        return_type operator[](size_type n);
        const return_type operator[](size_type n) const;
        iterator begin() noexcept;
        iterator end() noexcept;
        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;
        pointer data_begin() noexcept;
        pointer data_end() noexcept;
        const_pointer data_begin() const noexcept;
        const_pointer data_end() const noexcept;
    protected:
        ProxyMatrix() noexcept;
        template<typename Iterator>
        ProxyMatrix(pointer data, Iterator dimensions_begin, Iterator dimensions_end);
        template<typename Iterator>
        size_type assign_internal(pointer data, Iterator dimensions_begin, Iterator dimensions_end) noexcept;

        Dimensions  _dimensions;
        pointer     _data                   = nullptr;
        size_type   _dimension_size         = 0;
        size_type   _lower_dimension_size   = 0;
    };

    template<typename T, size_t DIMS>
    class Matrix : public ProxyMatrix<T, DIMS>
    {
        using parent_type = ProxyMatrix<T, DIMS>;
    public:
        using value_type      = typename parent_type::value_type;
        using size_type       = typename parent_type::size_type;
        using reference       = typename parent_type::reference;
        using const_reference = typename parent_type::const_reference;
        using pointer         = typename parent_type::pointer;
        using const_pointer   = typename parent_type::const_pointer;

        Matrix() noexcept;
        Matrix(const Matrix&) = delete;
        Matrix& operator=(const Matrix&) = delete;
        Matrix(Matrix&& mat) noexcept;
        Matrix& operator=(Matrix&& mat) noexcept;
        void create(std::initializer_list<size_type> dimensions);
    private:
        size_type _storage_size = 0;
        std::unique_ptr<value_type[]> _storage;
    };

    template<typename T, size_t DIMS>
    Matrix<T, DIMS>::Matrix() noexcept
    : parent_type()
    { }

    template<typename T, size_t DIMS>
    Matrix<T, DIMS>::Matrix(Matrix&& mat) noexcept
    {
        *this = std::move(mat);
    }

    template<typename T, size_t DIMS>
    Matrix<T, DIMS>& Matrix<T, DIMS>::operator=(Matrix&& mat) noexcept
    {
        if (this != &mat)
        {
            parent_type::operator=(std::move(mat));
            _storage_size = mat._storage_size;
            _storage      = std::move(mat._storage);
        }
        return *this;
    }

    template<typename T, size_t DIMS>
    void Matrix<T, DIMS>::create(std::initializer_list<size_type> dimensions)
    {
        if (dimensions.size() != parent_type::_dimensions.size())
        {
            throw std::runtime_error("Incorrect amount of dimensions provided");
        }
        size_type new_size = parent_type::assign_internal(nullptr, std::begin(dimensions), std::end(dimensions));
        if (new_size > _storage_size)
        {
            _storage_size = new_size;
            _storage.reset(new value_type[_storage_size]);
        }
        parent_type::_data = _storage.get();
    }

    template<typename T, size_t DIMS>
    ProxyMatrix<T, DIMS>::ProxyMatrix() noexcept
    { }

    template<typename T, size_t DIMS>
    ProxyMatrix<T, DIMS>::ProxyMatrix(const ProxyMatrix& mat) noexcept
    {
        *this = mat;
    }

    template<typename T, size_t DIMS>
    ProxyMatrix<T, DIMS>& ProxyMatrix<T, DIMS>::operator=(const ProxyMatrix& mat) noexcept
    {
        if (this != &mat)
        {
            _dimensions           = mat._dimensions;
            _data                 = mat._data;
            _dimension_size       = mat._dimension_size;
            _lower_dimension_size = mat._lower_dimension_size;
        }
        return *this;
    }

    template<typename T, size_t DIMS>
    ProxyMatrix<T, DIMS>::ProxyMatrix(ProxyMatrix&& mat) noexcept
    {
        *this = std::move(mat);
    }

    template<typename T, size_t DIMS>
    ProxyMatrix<T, DIMS>& ProxyMatrix<T, DIMS>::operator=(ProxyMatrix&& mat) noexcept
    {
        // NOTE: same as copy (since move is useless)
        *this = mat;
        return *this;
    }

    template<typename T, size_t DIMS>
    ProxyMatrix<T, DIMS>::~ProxyMatrix() noexcept
    {}

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::size_type ProxyMatrix<T, DIMS>::dimension_size() const noexcept
    {
        return _dimensions[0];
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::size_type ProxyMatrix<T, DIMS>::dimension_size(size_type n) const noexcept
    {
        return _dimensions[n];
    }

    template<typename T, size_t DIMS>
    ProxyMatrix<T, DIMS>::ProxyMatrix(pointer data, std::initializer_list<size_type> dimensions)
    {
        // TODO: static assert
        // static_assert(dimensions.size() == _dimensions.size(), "Incorrect amount of dimensions provided");
        if (dimensions.size() != _dimensions.size())
        {
            throw std::runtime_error("Incorrect amount of dimensions provided");
        }
        assign_internal(data, std::begin(dimensions), std::end(dimensions));
    }

    // TODO: iterator type_traits
    template<typename T, size_t DIMS>
    template<typename Iterator>
    ProxyMatrix<T, DIMS>::ProxyMatrix(pointer data, Iterator dimensions_begin, Iterator dimensions_end)
    {
        if (dimensions_end - dimensions_begin < (int)(_dimensions.size()))
        {
            throw std::runtime_error("Incorrect amount of dimensions provided");
        }
        assign_internal(data, dimensions_begin, dimensions_end);
    }

    template<typename T, size_t DIMS>
    template<typename Iterator>
    typename ProxyMatrix<T, DIMS>::size_type
    ProxyMatrix<T, DIMS>::assign_internal(pointer data, Iterator dimensions_begin, Iterator dimensions_end) noexcept
    {
        _data = data;
        std::copy(dimensions_begin, dimensions_end, std::begin(_dimensions));
        auto mult = [](const size_type& a, const size_type& b) { return a * b; };
        _dimension_size = std::accumulate(std::begin(_dimensions), std::end(_dimensions), (size_type) (1), mult);
        _lower_dimension_size = _dimension_size / _dimensions[0];
        return _dimension_size;
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::return_type ProxyMatrix<T, DIMS>::operator[](ProxyMatrix<T, DIMS>::size_type n)
    {
        if constexpr (std::is_same_v<return_type, reference>)
        {
            return _data[n];
        }
        else
        {
            return lower_dimension_proxy(_data + _lower_dimension_size * n, _dimensions.data() + 1,
                                       _dimensions.data() + _dimensions.size());
        }
    }

    template<typename T, size_t DIMS>
    const typename ProxyMatrix<T, DIMS>::return_type ProxyMatrix<T, DIMS>::operator[](ProxyMatrix<T, DIMS>::size_type n) const
    {
        if constexpr (std::is_same_v<return_type, reference>)
        {
            return _data[n];
        }
        else
        {
            return lower_dimension_proxy(_data + _lower_dimension_size * n, _dimensions.data() + 1,
                                         _dimensions.data() + _dimensions.size());
        }
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::pointer ProxyMatrix<T, DIMS>::data_begin() noexcept
    {
        return _data;
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::pointer ProxyMatrix<T, DIMS>::data_end() noexcept
    {
        return _data + _dimension_size;
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::const_pointer ProxyMatrix<T, DIMS>::data_begin() const noexcept
    {
        return _data;
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::const_pointer ProxyMatrix<T, DIMS>::data_end() const noexcept
    {
        return _data + _dimension_size;
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::iterator ProxyMatrix<T, DIMS>::begin() noexcept
    {
        if constexpr (std::is_same_v<return_type, reference>)
        {
            return iterator(data_begin());
        }
        else
        {
            return iterator(*this, 0);
        }
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::iterator ProxyMatrix<T, DIMS>::end() noexcept
    {
        if constexpr (std::is_same_v<return_type, reference>)
        {
            return iterator(data_end());
        }
        else
        {
            return iterator(*this, _dimensions[0]);
        }
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::const_iterator ProxyMatrix<T, DIMS>::begin() const noexcept
    {
        if constexpr (std::is_same_v<return_type, reference>)
        {
            return const_iterator(data_begin());
        }
        else
        {
            return const_iterator(*this, 0);
        }
    }

    template<typename T, size_t DIMS>
    typename ProxyMatrix<T, DIMS>::const_iterator ProxyMatrix<T, DIMS>::end() const noexcept
    {
        if constexpr (std::is_same_v<return_type, reference>)
        {
            return const_iterator(data_end());
        }
        else
        {
            return const_iterator(*this, _dimensions[0]);
        }
    }
}
#endif //UTILITY_CONTAINERS_HPP
