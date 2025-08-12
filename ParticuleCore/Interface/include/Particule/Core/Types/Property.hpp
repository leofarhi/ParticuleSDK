#ifndef PROPERTY_HPP
#define PROPERTY_HPP
#include <utility>
#include <type_traits>

namespace Particule::Core {

    template<class T, class Getter, class Setter>
    class Property {
        Getter getter_;
        Setter setter_;
    public:
        using value_type = T;

        // Construction par lambdas/foncteurs (pas de coût virtuel)
        constexpr Property(Getter g, Setter s)
            : getter_(std::move(g)), setter_(std::move(s)) {}

        // Lecture
        [[nodiscard]] constexpr T get() const
            noexcept(noexcept(getter_())) { return getter_(); }

        // Écriture
        constexpr void set(T v)
            noexcept(noexcept(setter_(std::move(v)))) { setter_(std::move(v)); }

        // Conversion implicite -> lecture (comme une variable)
        constexpr operator T() const
            noexcept(noexcept(getter_())) { return getter_(); }

        // Affectation -> écriture
        constexpr Property& operator=(T v)
            noexcept(noexcept(setter_(std::move(v)))) {
            setter_(std::move(v));
            return *this;
        }

        // Accès explicites si tu préfères
        [[nodiscard]] constexpr T operator()() const
            noexcept(noexcept(getter_())) { return getter_(); }

        // Opérateurs usuels (simples et suffisants)
        constexpr Property& operator+=(const T& d)
            noexcept(noexcept(setter_(getter_() + d))) {
            setter_(getter_() + d); return *this;
        }
        constexpr Property& operator-=(const T& d)
            noexcept(noexcept(setter_(getter_() - d))) {
            setter_(getter_() - d); return *this;
        }
        constexpr Property& operator*=(const T& d)
            noexcept(noexcept(setter_(getter_() * d))) {
            setter_(getter_() * d); return *this;
        }
        constexpr Property& operator/=(const T& d)
            noexcept(noexcept(setter_(getter_() / d))) {
            setter_(getter_() / d); return *this;
        }

        // ++ / -- (préfixe & postfixe)
        constexpr Property& operator++()
            noexcept(noexcept(setter_(getter_() + T{1}))) {
            setter_(getter_() + T{1}); return *this;
        }
        constexpr T operator++(int)
            noexcept(noexcept(getter_()) && noexcept(setter_(T{}))) {
            T old = getter_();
            setter_(old + T{1});
            return old;
        }
        constexpr Property& operator--()
            noexcept(noexcept(setter_(getter_() - T{1}))) {
            setter_(getter_() - T{1}); return *this;
        }
        constexpr T operator--(int)
            noexcept(noexcept(getter_()) && noexcept(setter_(T{}))) {
            T old = getter_();
            setter_(old - T{1});
            return old;
        }
    };

    // Helper de déduction (ergonomie maximale)
    template<class T, class G, class S>
    [[nodiscard]] constexpr auto make_property(G&& g, S&& s) {
        return Property<T, std::decay_t<G>, std::decay_t<S>>(
            std::forward<G>(g), std::forward<S>(s)
        );
    }

    // Bonus ergonomie : lier directement une lvalue sans écrire les lambdas
    template<class T>
    [[nodiscard]] constexpr auto property_ref(T& ref) {
        return make_property<T>(
            [&ref]{ return ref; },
            [&ref](T v){ ref = std::move(v); }
        );
    }


    template<class Owner, class T,
            T   (Owner::*Getter)() const,
            void(Owner::*Setter)(const T&)>
    struct MethodProperty {
        Owner* self{};

        // lecture implicite
        operator T() const { return (self->*Getter)(); }

        // écriture
        MethodProperty& operator=(const T& v){ (self->*Setter)(v); return *this; }

        // opérateurs utiles
        MethodProperty& operator+=(const T& d){
            T w = (self->*Getter)();
            (self->*Setter)(w + d);
            return *this;
        }
        MethodProperty& operator-=(const T& d){
            T w = (self->*Getter)();
            (self->*Setter)(w - d);
            return *this;
        }
        MethodProperty& operator*=(const T& d){ // Hadamard si T=Vector3
            T w = (self->*Getter)();
            (self->*Setter)(w * d);
            return *this;
        }
        MethodProperty& operator/=(const T& d){
            T w = (self->*Getter)();
            (self->*Setter)(w / d);
            return *this;
        }
    };

}

/*
EXEMPLES:

#include <iostream>
// #include "Property.hpp"

struct Test {
private:
    int _var = 0;

public:
    // On déclare le membre
    Property<int,
             decltype([this]{ return _var; }),
             decltype([this](int v){ _var = v; })> var;

    // On l’initialise proprement dans la liste d'init (capture de this)
    Test()
    : var(
        [this]{ return _var; },           // getter
        [this](int v){ _var = v; }        // setter
      )
    {}
};

int main() {
    Test t;
    t.var = 42;           // set()
    std::cout << (int)t.var << "\n";  // get() -> 42
    t.var += 8;           // 50
    ++t.var;              // 51
    std::cout << t.var << "\n";
}


ou 


struct Test2 {
private:
    int value = 10;
public:
    // Liaison directe au champ 'value'
    decltype(property_ref(value)) prop = property_ref(value);

    // Rien à faire dans le constructeur
};

*/


#endif // PROPERTY_HPP