// MethodPropertyVec3.hpp
#pragma once
#include <utility>
#include <type_traits>

template<class Owner, class Vec3,
         Vec3 (Owner::*Getter)() const,
         void (Owner::*Setter)(const Vec3&)>
struct MethodPropertyVec3 {
    using Scalar = decltype(std::declval<Vec3>().x);

    struct Component {
        Owner* self{};
        // 0 = x, 1 = y, 2 = z
        int idx{0};

        // lecture
        operator Scalar() const {
            Vec3 v = (self->*Getter)(); 
            return (idx==0)? v.x : (idx==1? v.y : v.z);
        }

        // écriture
        Component& operator=(const Scalar& s){
            Vec3 v = (self->*Getter)();
            if      (idx==0) v.x = s;
            else if (idx==1) v.y = s;
            else             v.z = s;
            (self->*Setter)(v);
            return *this;
        }

        // composés
        Component& operator+=(const Scalar& d){ Scalar s = Scalar(*this); return (*this) = (s + d); }
        Component& operator-=(const Scalar& d){ Scalar s = Scalar(*this); return (*this) = (s - d); }
        Component& operator*=(const Scalar& d){ Scalar s = Scalar(*this); return (*this) = (s * d); }
        Component& operator/=(const Scalar& d){ Scalar s = Scalar(*this); return (*this) = (s / d); }
    };

    Owner* self{};
    // sous-properties composantes
    Component x{self,0}, y{self,1}, z{self,2};

    MethodPropertyVec3() = default;
    explicit MethodPropertyVec3(Owner* s) : self(s), x{s,0}, y{s,1}, z{s,2} {}

    // lecture/écriture du vecteur entier
    operator Vec3() const { return (self->*Getter)(); }

    MethodPropertyVec3& operator=(const Vec3& v){ (self->*Setter)(v); return *this; }

    MethodPropertyVec3& operator+=(const Vec3& d){
        Vec3 v = (self->*Getter)();
        (self->*Setter)(v + d);
        return *this;
    }
    MethodPropertyVec3& operator-=(const Vec3& d){
        Vec3 v = (self->*Getter)();
        (self->*Setter)(v - d);
        return *this;
    }
    MethodPropertyVec3& operator*=(const Vec3& d){ // hadamard
        Vec3 v = (self->*Getter)();
        (self->*Setter)(v * d);
        return *this;
    }
    MethodPropertyVec3& operator/=(const Vec3& d){ // hadamard
        Vec3 v = (self->*Getter)();
        (self->*Setter)(v / d);
        return *this;
    }
};
