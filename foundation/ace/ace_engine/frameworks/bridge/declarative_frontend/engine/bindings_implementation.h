/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_ACE_FRAMEWORKS_DECLARATIVE_FRONTEND_ENGINE_BINDINGS_IMPLEMENTATION_H
#define FOUNDATION_ACE_FRAMEWORKS_DECLARATIVE_FRONTEND_ENGINE_BINDINGS_IMPLEMENTATION_H

#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/log/log.h"
#include "base/memory/ace_type.h"
#include "bindings_defines.h"
#include "frameworks/bridge/common/utils/function_traits.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/engine/js_types.h"

namespace OHOS::Ace::Framework {

enum MethodOptions : uint8_t {
    NONE = 0,
    RETURN_SELF = 1 << 0, // for chaining
    STRICT_TYPE_CHECK = 1 << 1,
};

class IFunctionBinding {
public:
    IFunctionBinding(const char* name, MethodOptions options) : name_(name), options_(options) {}
    virtual ~IFunctionBinding() {}

    const char* Name() const
    {
        return name_;
    }

    MethodOptions Options() const
    {
        return options_;
    }

private:
    const char* name_;
    MethodOptions options_;
};

template<typename Class, typename ReturnType, typename... Args>
class FunctionBinding : public IFunctionBinding {
    using FunctionPtr = ReturnType (Class::*)(Args...);

public:
    FunctionBinding(const char* name, MethodOptions options, FunctionPtr func)
        : IFunctionBinding(name, options), func_(func)
    {}

    ~FunctionBinding() override = default;

    FunctionPtr Get()
    {
        return func_;
    }

private:
    FunctionPtr func_;
};

template<typename ReturnType, typename... Args>
class StaticFunctionBinding : public IFunctionBinding {
    using FunctionPtr = ReturnType (*)(Args...);

public:
    StaticFunctionBinding(const char* name, MethodOptions options, FunctionPtr func)
        : IFunctionBinding(name, options), func_(func)
    {}
    ~StaticFunctionBinding() override = default;

    FunctionPtr Get()
    {
        return func_;
    }

private:
    FunctionPtr func_;
};

/**
 *  \brief A class template that binds C++ classes to Javascript.
 *
 *  This class is the entry point for binding classes and methods to Javascript regardless of which engine is
 *  underneath. In the following text, a class C is the C++ class to be bound to Javascript, and engine-specific
 *  implementation is abbeviated as ESI.
 *
 *  Methods and member variables registered to javascript are equivalent to a property in a javascript object.
 *  \p Method(...) is used to register methods. If a member function is registered directly, the ESI should take care of
 *  the value conversion between Javascript and C++ types.
 *
 *  If more control is required than just mere conversions between JS and C++ (such as error checks, weak types, storing
 * JS objects etc.), methods can be bound to a signature that enables this control. The signautre is <tt> void(*)(const
 * JSCallbackInfo&)</tt>. This is done to work regardless of the engine underneath. See \p JSRef , \p JSRefPtr for more
 * info on usage.
 *
 *  If a direct communication with the engine is required when communicating with javascript, member functions and
 * callbacks with engine-specific signatures can be registered to Javascript and the embedder can access the engine APIs
 * directly. Additional options can be passed to \p Method(...) when binding member functions, such as the object can
 * return itself in case of functions that do not return a value, so that in Javascript one can do so-called "chaining":
 *  \code{.js}
 *  object.width(10).height(10).top(0).left(20)
 *  \endcode
 *
 *  There is a general constructor for every bound class. When calling \p Bind however, the constructor argument types
 *  must be passes as template arguments (see example) so that the automatic conversion checks and converts the values.
 *
 *
 *
 *  Engine-specific implementation guide:
 *  This code is using the curiously recurring template pattern (CRTP) idiom as neither function templates nor static
 *  methods can be overloaded. Therefore, an ESI "inherits" from this class because these class methods call methods
 *  that have to be defined, otherwise the compiler will report an error. As the binding itself is considerably
 *  engine-specific, this class implements only engine-agnostic attributes such as class names, method names, member
 *  function pointers etc. The actual value conversion is to be implemented by the engine implementation, and choosing
 *  the engine to be used should be conducted by the build system.
 *
 *  ESIs are expected to define several aliases:
 *  1. one called \p JSClass with the class C and the ESI class template as template
 *  arguments to this class:
 *  \code{.cpp}
 *  template<typename C>
 *  using JSClass = JSClassImpl<C, MyJSEngineClassImpl>
 *  \endcode
 *
 *  Inside the ImplDetail class template:
 *  2.  \p BindingTarget that corresponds to an ESI object template
 *  \code{.cpp}
 *  // A v8 object template
 *  using BindingTarget = v8::Local<v8::ObjectTemplate>;
 *  // A QJS object template
 *  using BindingTarget = JSValue;
 *  \endcode
 *
 *  3. \p FunctionCallback and \p MemberFunctionCallback corresponding to ESI callback signatures:
 *  \code{.cpp}
 *  // v8 callback signatures
 *  using FunctionCallback = void (*)(const v8::FunctionCallbackInfo<v8::Value>&);
 *  using MemberFunctionCallback = void (C::*)(const v8::FunctionCallbackInfo<v8::Value>&);
 *  // QJS callback signatures
 *  using FunctionCallback = JSValue (*)(JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv);
 *  using MemberFunctionCallback = JSValue (C::*)(JSContext* ctx, JSValueConst thisObj, int argc, JSValueConst* argv);
 *  \endcode
 *
 *
 *  \tparam C The C++ class to be bound
 *  \tparam ImplDetail an engine-specific class template that takes class C as a template argument
 *
 *  \example Binding classes TwoDPoint and ThreeDPoint as "Point2" and "Point3" in javascript, and registering its
 * methods.
 *
 *  \code{.cpp}
 *
 *  // We are using V8 engine for this example:
 *  template<typename C>
 *  using JSClass = JSClassImpl<C, V8Class>;
 *
 *  // Somewhere in engine-initialization:
 *  JSClass<TwoDPoint>::Declare("Point2");
 *  JSClass<TwoDPoint>::Method("setX", &TwoDPoint::SetX, MethodOptions::RETURN_SELF);
 *  JSClass<TwoDPoint>::Method("getX", &TwoDPoint::GetX);
 *  JSClass<TwoDPoint>::Method("setY", &TwoDPoint::SetY, MethodOptions::RETURN_SELF);
 *  JSClass<TwoDPoint>::Method("getY", &TwoDPoint::GetY);
 *  JSClass<TwoDPoint>::Method("print", &TwoDPoint::Print);
 *  JSClass<TwoDPoint>::StaticMethod("parse", &TwoDPoint::Parse);
 *  JSClass<TwoDPoint>::Bind<float, float>(globalObject);   // Note the template arguments. Here we are specifying
 *                                                          // that we're expecting the JS constructor to accept
 *                                                          // two "float" arguments
 *
 *  JSClass<ThreeDPoint>::Declare("Point3");
 *  JSClass<ThreeDPoint>::Method("setZ", &ThreeDPoint::SetZ, MethodOptions::RETURN_SELF);
 *  JSClass<ThreeDPoint>::Method("getZ", &ThreeDPoint::GetZ);
 *  JSClass<ThreeDPoint>::Inherit<TwoDPoint>();
 *  JSClass<ThreeDPoint>::Bind<float, float, float>(globalObject);
 *                                                           // Note the template arguments. Here we are specifying
 *                                                           // that we're expecting the JS constructor to accept
 *                                                           // three "float" arguments
 *  \endcode
 *
 * \code{.js}
 *                                            // C++ call tree
 * let point = new Point2(1,2);               // V8Class<TwoDPoint>::InternalConstructor<float, float>
 * point.print();                             // V8Class<TwoDPoint>::InternalMethodCallback<void>
 *                                            //     "Point(1,2)"
 * let other = Point2.parse("(3,4)");         // V8Class<TwoDPoint>::JSStaticMethodCallback
 * console.log("point.x is " + point.getX()); // V8Class<TwoDPoint>::InternalMethodCallback<float>
 *                                            //     "point.x is 1"
 * console.log("other.x is " + other.getX()); // V8Class<TwoDPoint>::InternalMethodCallback<float>
 *                                            //     "other.x is 3"
 * point.setX(5).setY(10);                    // V8Class<TwoDPoint>::InternalMethodCallback<void, float>
 *                                            // V8Class<TwoDPoint>::InternalMethodCallback<void, float>
 * point.print();                             // V8Class<TwoDPoint>::InternalMethodCallback<void>
 *                                            //     "Point(5,10)"
 * let anotherPoint = new Point3(1,2,3);      // V8Class<ThreeDPoint>::InternalConstructor<float, float, float>
 * anotherPoint.setX(5).setY(6).setZ(7)       // V8Class<TwoDPoint>::InternalMethodCallback<void, float>
 *                                            // V8Class<TwoDPoint>::InternalMethodCallback<void, float>
 *                                            // V8Class<ThreeDPoint>::InternalMethodCallback<void, float>
 * anotherpoint.print();                      // V8Class<ThreeDPoint>::InternalMethodCallback<void>
 *                                            //     "Point(5,6,7)"
 * \endcode
 * \class JSClassImpl
 */
template<typename C, template<typename> typename ImplDetail>
class JSClassImpl {
public:
    JSClassImpl() = delete;

    /**
     *  Declare class C that will be exposed with the given \p name in Javascript
     *  \note This must be always called first before any other registrations. The engine-specific implementations
     *  should instantiate object templates with this call
     *  \param name A string literal
     *  \static
     */
    static void Declare(const char* name);

    /**
     *  Register a method that is a member of a class C or its base class.
     *  \note Trying to bind a method of unrelated classes will result in a compile error
     *  \param name The name of the method that will be exposed as in Javascript
     *  \param func A member-function pointer belonging to class C's base class
     *  \param options Method options flags, default value is NONE
     *
     *  \tparam Base A base class to \p C . No need to specify, since it will be deducted from the function pointer
     *  \tparam R The return type of \p func . No need to specify, since it will be deducted from the function pointer
     *  \tparam Args... Types of function arguments of \p func . No need to specify, since they will be deducted from
     * the function pointer
     *  \static
     */
    template<typename Base, typename R, typename... Args>
    static void Method(const char* name, R (Base::*func)(Args...), MethodOptions options = MethodOptions::NONE);

    /**
     *  Register a static method of class C.
     *  \param name The name of the method that will be exposed as in Javascript
     *  \param func A static function
     *  \param options Method options flags, default value is NONE
     *
     *  \tparam R The return type of \p func . No need to specify, since it will be deducted from the function pointer
     *  \tparam Args... Types of function arguments of \p func . No need to specify, since they will be deducted from
     * the function pointer
     *  \static
     */
    template<typename R, typename... Args>
    static void StaticMethod(const char* name, R (*func)(Args...), MethodOptions options = MethodOptions::NONE);

    /**
     *  Register a static method of class C with a void(*)(const JSCallbackInfo&) signature.
     *  \param name The name of the method that will be exposed as in Javascript
     *  \param func A static function with void(*)(const JSCallbackInfo&) signature
     *  \static
     */
    static void StaticMethod(const char* name, JSFunctionCallback func);

    /**
     *  Register a method that is a member of a related class T with an engine-specific callback signature
     *
     *  \tparam T A class that is either equivalent or a base to C
     *  \param name The name of the method that will be exposed as in Javascript
     *  \param callback A member-function pointer belonging to class C's class with engine-specific signature
     *  \static
     */
    template<typename T>
    static void CustomMethod(const char* name, MemberFunctionCallback<T> callback);

    /**
     *  Register a method with an engine-specific callback signature
     *
     *  \param name The name of the method that will be exposed as in Javascript
     *  \param callback A function pointer with the engine-specific signature
     *  \static
     */
    static void CustomMethod(const char* name, FunctionCallback callback);

    /**
     *  Register a method with an generic callback signature
     *
     *  \param name The name of the method that will be exposed as in Javascript
     *  \param callback A function pointer with the engine-specific signature
     *  \static
     */
    template<typename T>
    static void CustomMethod(const char* name, JSMemberFunctionCallback<T> callback);

    template<typename T>
    static void CustomProperty(const char* name, MemberFunctionGetCallback<T> getter,
        MemberFunctionSetCallback<T> setter);

    static void CustomProperty(const char* name, FunctionGetCallback getter,
        FunctionSetCallback setter);

    template<typename T>
    static void CustomProperty(const char* name, JSMemberFunctionCallback<T> getter,
        JSMemberFunctionCallback<T> setter);
    /**
     *  Register a static method with an engine-specific callback signature
     *
     *  \param name The name of the method that will be exposed as in Javascript
     *  \param callback A function pointer with the engine-specific signature
     *  \static
     */
    static void CustomStaticMethod(const char* name, FunctionCallback callback);

    static void ExoticGetter(ExoticGetterCallback callback);
    static void ExoticSetter(ExoticSetterCallback callback);
    static void ExoticHasProperty(ExoticHasPropertyCallback callback);

    template<typename T>
    static void StaticConstant(const char* name, T value);

    /**
     *  Bind the class to Javascript with a custom constructor that has engine-specific callback signature
     *
     *  \param bindTarget An object template to bind this class to.
     *  \param ctor Constructor
     *  \static
     */
    static void Bind(BindingTarget bindTarget, FunctionCallback ctor);

    /**
     *  Bind the class to Javascript with custom constructor, destructor and GC mark callbacks.
     *  If no destructor callback is specified, the C++ instance is simply "delete"-d on garbage collection sweeps.
     *
     *  \param bindTarget An object template to bind this class to
     *  \param ctor Constructor with void(*)(const JSCallbackInfo&) signature
     *  \param dtor Destructor with void(*)(C* instance) signature (optional)
     *  \param gcMark A GC mark callback with void(*)(C* instance, const JSGCMarkCallbackInfo&) signature (optional)
     *  \static
     */
    static void Bind(BindingTarget bindTarget, JSFunctionCallback ctor, JSDestructorCallback<C> dtor = nullptr,
        JSGCMarkCallback<C> gcMark = nullptr);

    /**
     *  Bind the class to Javascript with optional destructor and GC mark callbacks.
     *  If no destructor callback is specified, the C++ instance is simply "delete"-d on garbage collection sweeps.
     *
     *  \tparam Args... A list of argument types that the constructor of class C accepts.
     *  \param bindTarget An object template to bind this class to.
     *  \param dtor Destructor with void(*)(C* instance) signature (optional)
     *  \param gcMark A GC mark callback with void(*)(C* instance, const JSGCMarkCallbackInfo&) signature (optional)
     *  \static
     */
    template<typename... Args>
    static void Bind(
        BindingTarget bindTarget, JSDestructorCallback<C> dtor = nullptr, JSGCMarkCallback<C> gcMark = nullptr);

    /**
     *  Inherit all bound methods and properties from \p Base
     *  \note A binding for the base class must exist beforehand with
     *  \code JSClassImpl<Base,Impl>::Declare("MyBaseClass") \endcode
     *
     *  \tparam Base A base class of C
     */
    template<typename Base>
    static void Inherit();

    static IFunctionBinding* GetFunctionBinding(int id);
    static IFunctionBinding* GetGetFunctionBinding(int id);
    static IFunctionBinding* GetSetFunctionBinding(int id);

    /**
     *  Get the Javascript name of class C
     *  \return The javascript name
     */
    static const char* JSName();

    /**
     *  Create new instance of declared class
     *  \return new JS object instance
     */
    static JSRef<JSObject> NewInstance();

private:
    static std::string jsName;
    /*  OPTIMIZE(cvetan): Functions can be stored at compile-time with an additional index as a template parameter, for
        example:

        template<int N, typename RetType, typename... Args>
        static RetType(C::*function_)(Args...);

        But only if current approach proves to be a bottleneck.
    */
    static std::unordered_map<int, IFunctionBinding*> functions_;
    static std::unordered_map<int, IFunctionBinding*> getFunctions_;
    static std::unordered_map<int, IFunctionBinding*> setFunctions_;
    static int nextFreeId_;
};

}; // namespace OHOS::Ace::Framework

#include "bindings_implementation.inl"

#endif // FOUNDATION_ACE_FRAMEWORKS_DECLARATIVE_FRONTEND_ENGINE_BINDINGS_IMPLEMENTATION_H
