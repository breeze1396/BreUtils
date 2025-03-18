/**
https://github.com/palacaze/sigslot/tree/master?tab=License-1-ov-file
*/

#pragma once
#include <atomic>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <thread>
#include <vector>
#include <typeinfo>


namespace bre {

template <typename...>
class Signal;


namespace trait {

/// represent a list of types
template <typename...> struct typelist {};

/**
    可以转换为弱指针概念以用于追踪目的的指针必须实现 to_weak() 函数，
    以便利用参数依赖查找（ADL）来转换该类型并使其可用。
 */

template <typename T>
std::weak_ptr<T> to_weak(std::weak_ptr<T> w) {
    return w;
}

template <typename T>
std::weak_ptr<T> to_weak(std::shared_ptr<T> s) {
    return s;
}

// tools
namespace detail {


template <typename, typename = void>
struct has_call_operator : std::false_type {};

template <typename F>
struct has_call_operator<F, std::void_t<decltype(&std::remove_reference<F>::type::operator())>>
    : std::true_type {};

template <typename, typename, typename = void, typename = void>
struct is_callable : std::false_type {};

template <typename F, typename P, typename... T>
struct is_callable<F, P, typelist<T...>,
        std::void_t<decltype(((*std::declval<P>()).*std::declval<F>())(std::declval<T>()...))>>
    : std::true_type {};

template <typename F, typename... T>
struct is_callable<F, typelist<T...>,
        std::void_t<decltype(std::declval<F>()(std::declval<T>()...))>>
    : std::true_type {};


template <typename T, typename = void>
struct is_weak_ptr : std::false_type {};

template <typename T>
struct is_weak_ptr<T, std::void_t<decltype(std::declval<T>().expired()),
                             decltype(std::declval<T>().lock()),
                             decltype(std::declval<T>().reset())>>
    : std::true_type {};

template <typename T, typename = void>
struct is_weak_ptr_compatible : std::false_type {};

template <typename T>
struct is_weak_ptr_compatible<T, std::void_t<decltype(to_weak(std::declval<T>()))>>
    : is_weak_ptr<decltype(to_weak(std::declval<T>()))> {};

template <typename...>
struct is_signal : std::false_type {};

template <typename L, typename... T>
struct is_signal<Signal<L, T...>>
    : std::true_type {};

} // namespace detail


/// determine if a pointer is convertible into a "weak" pointer
template <typename P>
constexpr bool is_weak_ptr_compatible_v = detail::is_weak_ptr_compatible<std::decay_t<P>>::value;

/// determine if a type T (Callable or Pmf) is callable with supplied arguments
template <typename L, typename... T>
constexpr bool is_callable_v = detail::is_callable<T..., L>::value;

template <typename T>
constexpr bool is_weak_ptr_v = detail::is_weak_ptr<T>::value;

template <typename T>
constexpr bool has_call_operator_v = detail::has_call_operator<T>::value;

template <typename T>
constexpr bool is_pointer_v = std::is_pointer<T>::value;

template <typename T>
constexpr bool is_func_v = std::is_function<T>::value;

template <typename T>
constexpr bool is_pmf_v = std::is_member_function_pointer<T>::value;

template <typename S>
constexpr bool is_signal_v = detail::is_signal<S>::value;

} // namespace trait

namespace detail {

/**
    下面的 function_traits 和 object_pointer 系列模板用于
    绕过 slot_base 实现中的类型擦除。它们用于比较存储的函数和对象
    与另一个函数或对象，以实现断开连接的目的
 */

/*
    函数指针和成员函数指针的大小在不同的编译器之间有所不同，
    并且虚拟成员与非虚拟成员相比也不同。
    在某些编译器上，多重继承也会有影响。
    因此，我们形成了一个足够大的联合体来存储任何类型的函数指针。
 */
namespace mock {

struct a { virtual ~a() = default; void f(); virtual void g(); static void h(); };
struct b { virtual ~b() = default; void f(); virtual void g(); };
struct c : a, b { void f(); void g() override; };
struct d : virtual a { void g() override; };

union fun_types {
    decltype(&d::g) dm;
    decltype(&c::g) mm;
    decltype(&c::g) mvm;
    decltype(&a::f) m;
    decltype(&a::g) vm;
    decltype(&a::h) s;
    void (*f)();
    void *o;
 };

} // namespace mock

/*
* 此结构用于存储函数指针。
* 这对于通过函数指针断开插槽连接是必需的。
* 它假设底层实现是可平凡复制的。
 */
struct func_ptr {
    func_ptr() : sz{0} {
        std::uninitialized_fill(std::begin(data), std::end(data), '\0');
    }

    template <typename T>
    void store(const T &t) {
        const auto *b = reinterpret_cast<const char*>(&t);
        sz = sizeof(T);
        std::memcpy(data, b, sz);
    }

    template <typename T>
    const T* as() const {
        if (sizeof(T) != sz) {
            return nullptr;
        }
        return reinterpret_cast<const T*>(data);
    }

private:
    alignas(sizeof(mock::fun_types)) char data[sizeof(mock::fun_types)];
    size_t sz;
};


template <typename T, typename = void>
struct function_traits {
    static void ptr(const T &/*t*/, func_ptr &/*d*/) {
    }

    static bool eq(const T &/*t*/, const func_ptr &/*d*/) {
        return false;
    }

    static constexpr bool is_disconnectable = false;
    static constexpr bool must_check_object = true;
};

template <typename T>
struct function_traits<T, std::enable_if_t<trait::is_func_v<T>>> {
    static void ptr(T &t, func_ptr &d) {
        d.store(&t);
    }

    static bool eq(T &t, const func_ptr &d) {
        const auto *r = d.as<const T*>();
        return r && *r == &t;
    }

    static constexpr bool is_disconnectable = true;
    static constexpr bool must_check_object = false;
};

template <typename T>
struct function_traits<T*, std::enable_if_t<trait::is_func_v<T>>> {
    static void ptr(T *t, func_ptr &d) {
        function_traits<T>::ptr(*t, d);
    }

    static bool eq(T *t, const func_ptr &d) {
        return function_traits<T>::eq(*t, d);
    }

    static constexpr bool is_disconnectable = true;
    static constexpr bool must_check_object = false;
};

template <typename T>
struct function_traits<T, std::enable_if_t<trait::is_pmf_v<T>>> {
    static void ptr(T t, func_ptr &d) {
        d.store(t);
    }

    static bool eq(T t, const func_ptr &d) {
        const auto *r = d.as<const T>();
        return r && *r == t;
    }

    static constexpr bool is_disconnectable = true;
    static constexpr bool must_check_object = true;
};

// 对于函数对象，假设我们是在寻找调用操作符。
template <typename T>
struct function_traits<T, std::enable_if_t<trait::has_call_operator_v<T>>> {
    using call_type = decltype(&std::remove_reference<T>::type::operator());

    static void ptr(const T &/*t*/, func_ptr &d) {
        function_traits<call_type>::ptr(&T::operator(), d);
    }

    static bool eq(const T &/*t*/, const func_ptr &d) {
        return function_traits<call_type>::eq(&T::operator(), d);
    }

    static constexpr bool is_disconnectable = function_traits<call_type>::is_disconnectable;
    static constexpr bool must_check_object = function_traits<call_type>::must_check_object;
};

template <typename T>
func_ptr get_function_ptr(const T &t) {
    func_ptr d;
    function_traits<std::decay_t<T>>::ptr(t, d);
    return d;
}

template <typename T>
bool eq_function_ptr(const T& t, const func_ptr &d) {
    return function_traits<std::decay_t<T>>::eq(t, d);
}

/*
* obj_ptr 用于存储指向对象的指针。
* 需要 object_pointer 特性来正确处理可跟踪的对象，因为它们可能不是指针。
 */
using obj_ptr = const void*;

template <typename T>
obj_ptr get_object_ptr(const T &t);

template <typename T, typename = void>
struct object_pointer {
    static obj_ptr get(const T&) {
        return nullptr;
    }
};

template <typename T>
struct object_pointer<T*, std::enable_if_t<trait::is_pointer_v<T*>>> {
    static obj_ptr get(const T *t) {
        return reinterpret_cast<obj_ptr>(t);
    }
};

template <typename T>
struct object_pointer<T, std::enable_if_t<trait::is_weak_ptr_v<T>>> {
    static obj_ptr get(const T &t) {
        auto p = t.lock();
        return get_object_ptr(p);
    }
};

template <typename T>
struct object_pointer<T, std::enable_if_t<!trait::is_pointer_v<T> &&
                                          !trait::is_weak_ptr_v<T> &&
                                          trait::is_weak_ptr_compatible_v<T>>>
{
    static obj_ptr get(const T &t) {
        return t ? reinterpret_cast<obj_ptr>(t.get()) : nullptr;
    }
};

template <typename T>
obj_ptr get_object_ptr(const T &t) {
    return object_pointer<T>::get(t);
}

template <typename B, typename D, typename ...Arg>
inline std::shared_ptr<B> make_shared(Arg && ... arg) {
    return std::static_pointer_cast<B>(std::make_shared<D>(std::forward<Arg>(arg)...));
}


/* 
slot_state 保存与插槽类型无关的状态，用于通过连接和 scoped_connection 对象间接与插槽进行交互。
 */
class slot_state {
public:
    constexpr slot_state(int gid) noexcept
        : m_index(0)
        , m_group(gid)
        , m_connected(true)
        , m_blocked(false)
    {}

    virtual ~slot_state() = default;

    virtual bool connected() const noexcept { return m_connected; }

    bool disconnect() noexcept {
        bool ret = m_connected.exchange(false);
        if (ret) {
            do_disconnect();
        }
        return ret;
    }

    bool blocked() const noexcept { return m_blocked.load(); }
    void block()   noexcept { m_blocked.store(true); }
    void unblock() noexcept { m_blocked.store(false); }

protected:
    virtual void do_disconnect() {}

    auto index() const {
        return m_index;
    }

    auto& index() {
        return m_index;
    }

    int group() const {
        return m_group;
    }

private:
    template <typename...>
    friend class ::bre::Signal;

    std::size_t m_index;     // index into the array of slot pointers inside the signal
    const int m_group;  // slot group this slot belongs to
    std::atomic<bool> m_connected;
    std::atomic<bool> m_blocked;
};

} // namespace detail


namespace detail {

// interface for cleanable objects, used to cleanup disconnected slots
struct cleanable {
    virtual ~cleanable() = default;
    virtual void clean(slot_state *) = 0;
};

template <typename...>
class slot_base;

template <typename... T>
using slot_ptr = std::shared_ptr<slot_base<T...>>;


template <typename... Args>
class slot_base : public slot_state {
public:
    using base_types = trait::typelist<Args...>;

    explicit slot_base(cleanable &c, int gid)
        : slot_state(gid)
        , cleaner(c)
    {}
    ~slot_base() override = default;

    // method effectively responsible for calling the "slot" function with
    // supplied arguments whenever emission happens.
    virtual void call_slot(Args...) = 0;

    template <typename... U>
    void Emit(U && ...u) {
        if (slot_state::connected() && !slot_state::blocked()) {
            call_slot(std::forward<U>(u)...);
        }
    }

    // check if we are storing callable c
    template <typename C>
    bool has_callable(const C &c) const {
        auto p = get_callable();
        return eq_function_ptr(c, p);
    }

    template <typename C>
    std::enable_if_t<function_traits<C>::must_check_object, bool>
    has_full_callable(const C &c) const {
        return has_callable(c) && check_class_type<std::decay_t<C>>();
    }

    template <typename C>
    std::enable_if_t<!function_traits<C>::must_check_object, bool>
    has_full_callable(const C &c) const {
        return has_callable(c);
    }

    // check if we are storing object o
    template <typename O>
    bool has_object(const O &o) const {
        return get_object() == get_object_ptr(o);
    }

protected:
    void do_disconnect() final {
        cleaner.clean(this);
    }

    // retieve a pointer to the object embedded in the slot
    virtual obj_ptr get_object() const noexcept {
        return nullptr;
    }

    // retieve a pointer to the callable embedded in the slot
    virtual func_ptr get_callable() const noexcept {
        return get_function_ptr(nullptr);
    }

    // retieve a pointer to the callable embedded in the slot
    virtual const std::type_info& get_callable_type() const noexcept {
        return typeid(nullptr);
    }

private:
    template <typename U>
    bool check_class_type() const {
        return typeid(U) == get_callable_type();
    }


private:
    cleanable &cleaner;
};

/*
 * A slot object holds state information, and a callable to to be called
 * whenever the function call operator of its slot_base base class is called.
 */
template <typename Func, typename... Args>
class slot final : public slot_base<Args...> {
public:
    template <typename F, typename Gid>
    constexpr slot(cleanable &c, F && f, Gid gid)
        : slot_base<Args...>(c, gid)
        , func{std::forward<F>(f)} {}

protected:
    void call_slot(Args ...args) override {
        func(args...);
    }

    func_ptr get_callable() const noexcept override {
        return get_function_ptr(func);
    }

    const std::type_info& get_callable_type() const noexcept override {
        return typeid(func);
    }


private:
    std::decay_t<Func> func;
};


/*
 * A slot object holds state information, an object and a pointer over member
 * function to be called whenever the function call operator of its slot_base
 * base class is called.
 */
template <typename Pmf, typename Ptr, typename... Args>
class slot_pmf final : public slot_base<Args...> {
public:
    template <typename F, typename P>
    constexpr slot_pmf(cleanable &c, F && f, P && p, int gid)
        : slot_base<Args...>(c, gid)
        , pmf{std::forward<F>(f)}
        , ptr{std::forward<P>(p)} {}

protected:
    void call_slot(Args ...args) override {
        ((*ptr).*pmf)(args...);
    }

    func_ptr get_callable() const noexcept override {
        return get_function_ptr(pmf);
    }

    obj_ptr get_object() const noexcept override {
        return get_object_ptr(ptr);
    }

    const std::type_info& get_callable_type() const noexcept override {
        return typeid(pmf);
    }

private:
    std::decay_t<Pmf> pmf;
    std::decay_t<Ptr> ptr;
};


} // namespace detail


/**
 * @tparam T... the argument types of the emitting and slots functions.
 */
template <typename... T>
class Signal final : public detail::cleanable {
    using slot_base = detail::slot_base<T...>;
    using slot_ptr = detail::slot_ptr<T...>;
    struct group_type { std::vector<slot_ptr> slts; int gid; };
public:
    Signal() noexcept : m_block(false) {}
    ~Signal() override {
        Disconnect_all();
    }

    Signal(const Signal&) = delete;
    Signal & operator=(const Signal&) = delete;

    Signal(Signal && o) /* not noexcept */
        : m_block{o.m_block.load()} {
        std::unique_lock<std::mutex> lock(o.m_mutex);
        std::swap(m_slots, o.m_slots);
    }

    Signal & operator=(Signal && o) /* not noexcept */ {
        std::unique_lock<std::mutex> lock1(m_mutex, std::defer_lock);
        std::unique_lock<std::mutex> lock2(o.m_mutex, std::defer_lock);
        std::lock(lock1, lock2);

        std::swap(m_slots, o.m_slots);
        m_block.store(o.m_block.exchange(m_block.load()));
        return *this;
    }

    /**
     * Emit a signal
     *
     * Effect: All non blocked and connected slot functions will be called
     *         with supplied arguments.
     * Safety: With proper locking (see pal::signal), emission can happen from
     *         multiple threads simultaneously. The guarantees only apply to the
     *         signal object, it does not cover thread safety of potentially
     *         shared state used in slot functions.
     *
     * @param a... arguments to emit
     */
    template <typename... U>
    void Emit(U && ...a) const {
        if (m_block) {
            return;
        }

        // Reference to the slots to execute them out of the lock
        // a copy may occur if another thread writes to it.
        const std::vector<group_type>& ref = slots_reference();

        for (const auto &group : ref) {
            for (const auto &s : group.slts) {
                s->Emit(a...);
            }
        }
    }

    /**
     * Connect a callable of compatible arguments
     *
     * Effect: Creates and stores a new slot responsible for executing the
     *         supplied callable for every subsequent signal emission.
     *
     * @param c a callable
     * @param gid an identifier that can be used to order slot execution
     */
    template <typename Callable>
    void Connect(Callable && c, int gid = 0) {
        using slot_t = detail::slot<Callable, T...>;
        auto s = make_slot<slot_t>(std::forward<Callable>(c), gid);
        add_slot(std::move(s));
        return;
    }


    /**
     * Overload of connect for pointers over member functions
     *
     * @param pmf a pointer over member function
     * @param ptr an object pointer
     * @param gid an identifier that can be used to order slot execution
     */
    template <typename Pmf, typename Ptr>
    void Connect(Pmf && pmf, Ptr && ptr, int gid = 0) {
        using slot_t = detail::slot_pmf<Pmf, Ptr, T...>;
        auto s = make_slot<slot_t>(std::forward<Pmf>(pmf), std::forward<Ptr>(ptr), gid);
        add_slot(std::move(s));
        return;
    }


    /**
     * Disconnect slots bound to a callable
     *
     * Effect: Disconnects all the slots bound to the callable in argument.
     * Safety: Thread-safety depends on locking policy.
     *
     * If the callable is a free or static member function, this overload is always
     * available. However, RTTI is needed for it to work for pointer to member
     * functions, function objects or and (references to) lambdas, because the
     * C++ spec does not mandate the pointers to member functions to be unique.
     *
     * @param c a callable
     * @return the number of disconnected slots
     */
    template <typename Callable>
    std::enable_if_t<(trait::is_callable_v<trait::typelist<T...>, Callable> ||
                      trait::is_pmf_v<Callable>) &&
                     detail::function_traits<Callable>::is_disconnectable, size_t>
    Disconnect(const Callable &c) {
        return disconnect_if([&] (const auto &s) {
            return s->has_full_callable(c);
        });
    }

    /**
     * Disconnect slots bound to this object
     *
     * Effect: Disconnects all the slots bound to the object or tracked object
     *         in argument.
     * Safety: Thread-safety depends on locking policy.
     *
     * The object may be a pointer or trackable object.
     *
     * @param obj an object
     * @return the number of disconnected slots
     */
    template <typename Obj>
    std::enable_if_t<!trait::is_callable_v<trait::typelist<T...>, Obj> &&
                     !trait::is_pmf_v<Obj>, size_t>
    disconnect(const Obj &obj) {
        return disconnect_if([&] (const auto &s) {
            return s->has_object(obj);
        });
    }

    /**
     * Disconnect slots bound both to a callable and object
     *
     * Effect: Disconnects all the slots bound to the callable and object in argument.
     * Safety: Thread-safety depends on locking policy.
     *
     * For naked pointers, the Callable is expected to be a pointer over member
     * function. If obj is trackable, any kind of Callable can be used.
     *
     * @param c a callable
     * @param obj an object
     * @return the number of disconnected slots
     */
    template <typename Callable, typename Obj>
    size_t Disconnect(const Callable &c, const Obj &obj) {
        return disconnect_if([&] (const auto &s) {
            return s->has_object(obj) && s->has_callable(c);
        });
    }

    /**
     * Disconnect slots in a particular group
     *
     * Effect: Disconnects all the slots in the group id in argument.
     * Safety: Thread-safety depends on locking policy.
     *
     * @param gid a group id
     * @return the number of disconnected slots
     */
    size_t Disconnect(int gid) {
        std::unique_lock<std::mutex> lock(m_mutex);
        for (auto &group : m_slots) {
            if (group.gid == gid) {
                size_t count = group.slts.size();
                group.slts.clear();
                return count;
            }
        }
        return 0;
    }

    /**
     * Disconnects all the slots
     * Safety: Thread safety depends on locking policy
     */
    void Disconnect_all() {
        std::unique_lock<std::mutex> lock(m_mutex);
        clear();
    }

    void Block() noexcept {
        m_block.store(true);
    }
    void Unblock() noexcept {
        m_block.store(false);
    }
    bool Blocked() const noexcept {
        return m_block.load();
    }

    /**
     * Get number of connected slots
     * Safety: thread safe
     */
    size_t slot_count() noexcept {
        const std::vector<group_type>& ref = slots_reference();
        size_t count = 0;
        for (const auto &g : ref) {
            count += g.slts.size();
        }
        return count;
    }

protected:
    /**
     * remove disconnected slots
     */
    void clean(detail::slot_state *state) override {
        std::unique_lock<std::mutex> lock(m_mutex);
        const auto idx = state->index();
        const auto gid = state->group();

        // find the group
        for (auto &group : m_slots) {
            if (group.gid == gid) {
                auto &slts = group.slts;

                // ensure we have the right slot, in case of concurrent cleaning
                if (idx < slts.size() && slts[idx] && slts[idx].get() == state) {
                    std::swap(slts[idx], slts.back());
                    slts[idx]->index() = idx;
                    slts.pop_back();
                }

                return;
            }
        }
    }

private:
    // used to get a reference to the slots for reading
    inline const std::vector<group_type>& slots_reference() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_slots;
    }

    // create a new slot
    template <typename Slot, typename... A>
    inline auto make_slot(A && ...a) {
        return detail::make_shared<slot_base, Slot>(*this, std::forward<A>(a)...);
    }

    // add the slot to the list of slots of the right group
    void add_slot(slot_ptr &&s) {
        const int gid = s->group();

        std::unique_lock<std::mutex> lock(m_mutex);
        auto &groups = m_slots;

        // find the group
        auto it = groups.begin();
        while (it != groups.end() && it->gid < gid) {
            it++;
        }

        // create a new group if necessary
        if (it == groups.end() || it->gid != gid) {
            it = groups.insert(it, {{}, gid});
        }

        // add the slot
        s->index() = it->slts.size();
        it->slts.push_back(std::move(s));
    }

    // disconnect a slot if a condition occurs
    template <typename Cond>
    size_t disconnect_if(Cond && cond) {
        std::unique_lock<std::mutex> lock(m_mutex);
        auto &groups = m_slots;

        size_t count = 0;

        for (auto &group : groups) {
            auto &slts = group.slts;
            size_t i = 0;
            while (i < slts.size()) {
                if (cond(slts[i])) {
                    std::swap(slts[i], slts.back());
                    slts[i]->index() = i;
                    slts.pop_back();
                    ++count;
                } else {
                    ++i;
                }
            }
        }
        return count;
    }

    // to be called under lock: remove all the slots
    void clear() {
        m_slots.clear();
    }

private:
    mutable std::mutex m_mutex;
    std::vector<group_type> m_slots;
    std::atomic<bool> m_block;
};



} // namespace bre
