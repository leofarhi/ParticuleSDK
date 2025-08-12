#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <Particule/Core/System/Basic.hpp>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <cassert>
#include <algorithm>

namespace Particule::Core {

    template <class T>
    class Matrix {
    std::vector<std::size_t> dims_;     // tailles par dimension
    std::vector<std::size_t> strides_;  // pas par dimension (layout row-major)
    std::vector<T>           data_;     // stockage contigu

    FORCE_INLINE void build_strides() noexcept {
        const std::size_t n = dims_.size();
        strides_.assign(n, 1);
        for (std::size_t i = n; i-- > 1; )
        strides_[i - 1] = strides_[i] * dims_[i];
    }

    template <class... SizeTs,
                std::enable_if_t<(sizeof...(SizeTs) >= 1) &&
                                (std::conjunction_v<std::is_integral<SizeTs>...>), int> = 0>
    FORCE_INLINE void init(SizeTs... ds) {
        dims_ = { static_cast<std::size_t>(ds)... };
        build_strides();
        data_.assign(total_size(), T{});
    }

    template <class... Idx>
    [[nodiscard]] FORCE_INLINE std::size_t flatten(Idx... idxs) const noexcept {
        static_assert(std::conjunction_v<std::is_integral<Idx>...>, "indices must be integral");
        const std::size_t n = sizeof...(Idx);
        assert(n == dims_.size() && "bad number of indices");
        const std::size_t is[] = { static_cast<std::size_t>(idxs)... };
        std::size_t off = 0;
        for (std::size_t i = 0; i < n; ++i) {
        assert(is[i] < dims_[i] && "index out of bounds");
        off += is[i] * strides_[i];
        }
        return off;
    }

    public:
    using value_type = T;

    Matrix() = default;

    // Construction par liste d’arguments : Matrix<int>(20,20,3)
    template <class... SizeTs,
                std::enable_if_t<(sizeof...(SizeTs) >= 1) &&
                                (std::conjunction_v<std::is_integral<SizeTs>...>), int> = 0>
    explicit Matrix(SizeTs... ds) { init(ds...); }

    // Construction depuis une liste : Matrix<int> m{20,20,3};
    explicit Matrix(std::initializer_list<std::size_t> dims) {
        dims_.assign(dims.begin(), dims.end());
        build_strides();
        data_.assign(total_size(), T{});
    }

    FORCE_INLINE explicit Matrix(const std::vector<std::size_t>& dimensions): dims_(dimensions)
    {
        build_strides();
        data_.assign(total_size(), T{});
    }

    [[nodiscard]] FORCE_INLINE std::size_t rank() const noexcept { return dims_.size(); }
    [[nodiscard]] FORCE_INLINE const std::vector<std::size_t>& dims() const noexcept { return dims_; }
    [[nodiscard]] FORCE_INLINE std::size_t dim(std::size_t i) const noexcept { return dims_[i]; }

    [[nodiscard]] FORCE_INLINE std::size_t total_size() const noexcept {
        std::size_t s = 1;
        for (std::size_t d : dims_) s *= d;
        return s;
    }

    FORCE_INLINE void fill(const T& v) {
        std::fill(data_.begin(), data_.end(), v);
    }

    // Accès
    template <class... Idx>
    FORCE_INLINE T& get(Idx... idxs) noexcept {
        return data_[flatten(idxs...)];
    }
    template <class... Idx>
    FORCE_INLINE const T& get(Idx... idxs) const noexcept {
        return data_[flatten(idxs...)];
    }

    template <class... Idx>
    FORCE_INLINE T& operator()(Idx... idxs) noexcept {
        return data_[flatten(idxs...)];
    }
    template <class... Idx>
    FORCE_INLINE const T& operator()(Idx... idxs) const noexcept {
        return data_[flatten(idxs...)];
    }

    template <class... Idx>
    FORCE_INLINE void set(const T& v, Idx... idxs) noexcept {
        data_[flatten(idxs...)] = v;
    }

    [[nodiscard]] FORCE_INLINE T* data() noexcept { return data_.data(); }
    [[nodiscard]] FORCE_INLINE const T* data() const noexcept { return data_.data(); }
    };

    #undef FORCE_INLINE

}

/*
Matrix<int> m(20,20,3);
m.set(42, 1,2,0);
int a = m.get(1,2,0);
int b = m(1,2,0); // même chose
*/

#endif // MATRIX_HPP