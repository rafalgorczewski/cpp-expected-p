#ifndef EXPECTED_HPP
#define EXPECTED_HPP

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace hqrp {

    template <typename T>
    concept bool OStreamStreamable = requires(T t) {
        { std::cerr << t }
        ->std::ostream &;
    };

    struct nullexpect_t {};

    inline constexpr nullexpect_t nullexpect{};

    template <typename E, typename U>
    class expected {
       public:
        constexpr expected() = default;
        constexpr expected(const E & e)
            : value_variant(ValueVariant::Expected) {
            new (&toe.e) E(e);
        }
        constexpr expected(const U & u)
            : value_variant(ValueVariant::Unexpected) {
            new (&toe.u) U(u);
        }
        constexpr expected(nullexpect_t) : expected() {
        }
        constexpr expected(const expected & rhs)
            : value_variant(rhs.value_variant) {
            if (value_variant == ValueVariant::Expected) {
                new (&toe.e) E(rhs);
            } else if (value_variant == ValueVariant::Unexpected) {
                new (&toe.u) U(rhs);
            }
        }
        constexpr expected & operator=(const expected & rhs) {
            if (this != &rhs) {
                if (rhs.value_variant == ValueVariant::Expected) {
                    operator=(rhs.toe.e);
                } else if (rhs.value_variant == ValueVariant::Unexpected) {
                    operator=(rhs.toe.u);
                }
            }
            return *this;
        }
        constexpr expected(expected && rhs) noexcept(
            std::is_nothrow_move_constructible_v<E> &&
                std::is_nothrow_move_constructible_v<U>)
            : value_variant(rhs.value_variant) {
            if (value_variant == ValueVariant::Expected) {
                new (&toe.e) E(std::move(rhs.toe.e));
            } else if (value_variant == ValueVariant::Unexpected) {
                new (&toe.u) U(std::move(rhs.toe.u));
            }
        }
        constexpr expected & operator=(expected && rhs) noexcept(
            noexcept(operator=(std::declval<E>())) &&
            noexcept(operator=(std::declval<U>()))) {
            if (this != &rhs) {
                if (rhs.value_variant == ValueVariant::Expected) {
                    operator=(std::move(rhs.toe.e));
                } else if (rhs.value_variant == ValueVariant::Unexpected) {
                    operator=(std::move(rhs.toe.u));
                }
            }
            return *this;
        }
        ~expected() {
            if (value_variant == ValueVariant::Expected) {
                toe.e.~E();
            } else if (value_variant == ValueVariant::Unexpected) {
                toe.u.~U();
            }
        }

        template <typename... Args>
        constexpr void emplace_expected(Args &&... args) {
            if (value_variant == ValueVariant::Expected) {
                toe.e.~E();
            } else if (value_variant == ValueVariant::Unexpected) {
                toe.u.~U();
            }
            new (&toe.e) E(std::forward<Args>(args)...);
            value_variant = ValueVariant::Expected;
        }

        template <typename... Args>
        constexpr void emplace_unexpected(Args &&... args) {
            if (value_variant == ValueVariant::Expected) {
                toe.e.~E();
            } else if (value_variant == ValueVariant::Unexpected) {
                toe.u.~U();
            }
            new (&toe.u) U(std::forward<Args>(args)...);
            value_variant = ValueVariant::Unexpected;
        }

        constexpr expected & operator=(const E & rhs) {
            if (value_variant == ValueVariant::Expected) {
                toe.e = rhs;
            } else {
                if (value_variant == ValueVariant::Unexpected) {
                    toe.u.~U();
                }
                new (&toe.e) E(rhs);
                value_variant = ValueVariant::Expected;
            }
            return *this;
        }

        constexpr expected & operator=(E && rhs) noexcept(
            std::is_nothrow_move_constructible_v<E> &&
                std::is_nothrow_move_assignable_v<E>) {
            if (value_variant == ValueVariant::Expected) {
                toe.e = std::move(rhs);
            } else {
                if (value_variant == ValueVariant::Unexpected) {
                    toe.u.~U();
                }
                new (&toe.e) E(std::move(rhs));
                value_variant = ValueVariant::Expected;
            }
            return *this;
        }

        constexpr expected & operator=(const U & rhs) {
            if (value_variant == ValueVariant::Unexpected) {
                toe.u = rhs;
            } else {
                if (value_variant == ValueVariant::Expected) {
                    toe.e.~E();
                }
                new (&toe.u) U(rhs);
                value_variant = ValueVariant::Unexpected;
            }
            return *this;
        }

        constexpr expected & operator=(U && rhs) noexcept(
            std::is_nothrow_move_constructible_v<U> &&
                std::is_nothrow_move_assignable_v<U>) {
            if (value_variant == ValueVariant::Unexpected) {
                toe.u = std::move(rhs);
            } else {
                if (value_variant == ValueVariant::Expected) {
                    toe.e.~E();
                }
                new (&toe.u) U(std::move(rhs));
                value_variant = ValueVariant::Unexpected;
            }
            return *this;
        }

        constexpr expected & operator=(nullexpect_t) {
            if (value_variant == ValueVariant::Expected) {
                toe.e.~E();
            } else {
                if (value_variant == ValueVariant::Unexpected) {
                    toe.u.~U();
                }
                value_variant = ValueVariant::NoValue;
            }
            return *this;
        }

        constexpr E & get_expected() {
            if (value_variant == ValueVariant::Expected) {
                return toe.e;
            }
            throw std::logic_error(
                "Trying to use expected with expected value, but it contains "
                "unexpected value.");
        }

        constexpr const E & get_expected() const {
            if (value_variant == ValueVariant::Expected) {
                return toe.e;
            }
            throw std::logic_error(
                "Trying to use expected with expected value, but it contains "
                "unexpected value.");
        }

        constexpr U & get_unexpected() {
            if (value_variant == ValueVariant::Unexpected) {
                return toe.u;
            }
            throw std::logic_error(
                "Trying to use expected with unexpected value, but it contains "
                "expected value.");
        }

        constexpr const U & get_unexpected() const {
            if (value_variant == ValueVariant::Unexpected) {
                return toe.u;
            }
            throw std::logic_error(
                "Trying to use expected with unexpected value, but it contains "
                "expected value.");
        }

        constexpr E & value_or_terminate() {
            if (value_variant == ValueVariant::Expected) {
                return toe.e;
            }
            std::cerr << "Process terminated. Error message:" << std::endl;
            if constexpr (OStreamStreamable<U>) {
                std::cerr << "\"" << toe.u << "\"" << std::endl;
            } else {
                std::cerr << "Error type is not streamable via OStream."
                          << std::endl;
            }
            std::terminate();
        }

        constexpr const E & value_or_terminate() const {
            if (value_variant == ValueVariant::Expected) {
                return toe.e;
            }
            std::cerr << "Process terminated. Error message:" << std::endl;
            if constexpr (OStreamStreamable<U>) {
                std::cerr << "\"" << toe.u << "\"" << std::endl;
            } else {
                std::cerr << "Error type is not streamable via OStream."
                          << std::endl;
            }
            std::terminate();
        }

        constexpr E & value_or(const E & e) noexcept {
            if (value_variant == ValueVariant::Expected) {
                return toe.e;
            }
            return e;
        }

        constexpr const E & value_or(const E & e) const noexcept {
            if (value_variant == ValueVariant::Expected) {
                return toe.e;
            }
            return e;
        }

        constexpr explicit operator bool() const noexcept {
            if (value_variant == ValueVariant::Expected) {
                return true;
            }
            return false;
        }

        constexpr E & operator*() {
            return get_expected();
        }

        constexpr const E & operator*() const {
            return get_expected();
        }

        constexpr E * operator->() {
            return &get_expected();
        }

        constexpr const E * operator->() const {
            return &get_expected();
        }

       private:
        enum class ValueVariant { Expected, Unexpected, NoValue };

        union type_or_error {
            constexpr type_or_error() {
            }
            constexpr type_or_error(const type_or_error &) {
            }
            constexpr type_or_error & operator=(const type_or_error &) {
                return *this;
            }
            ~type_or_error() {
            }

            E e;
            U u;
        };

        type_or_error toe;
        ValueVariant value_variant = ValueVariant::NoValue;
    };

}  // namespace hqrp

#endif EXPECTED_HPP
