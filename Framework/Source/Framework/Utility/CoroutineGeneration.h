#pragma once
#include <experimental/coroutine>

using std::experimental::coroutine_handle;
using std::experimental::suspend_always;
using std::experimental::suspend_never;

struct generator
{
    struct promise_type
    {
        int value_;
        //getRetrunObjは必ず生成
        generator get_return_object()
        {
            std::cout << "get_return_object呼び出し" << std::endl;
            return generator(*this);
        }

        //初めにResume状態にする、neverはすぐに次に進む
        suspend_always initial_suspend()
        {
            std::cout << "initial_suspend呼び出し" << std::endl;
            return {};
        }

        //終わったらResume状態、終了後自動破棄しない
        suspend_always final_suspend()
        {
            std::cout << "final_suspend呼び出し" << std::endl;
            return {};
        }

        //co_yieldのたびに呼ばれる、値をコピーするためのメソッド
        suspend_always yield_value(int value)
        {
            std::cout << "yield_value呼び出し 値 " << value << " をコピー" << std::endl;

            value_ = value;
            return {};
        }

        void return_void()
        {
            std::cout << "co_voidで終わった呼び出し" << std::endl;
        }

        //reeturn_voidと片方のみ
        /*int return_value(int value)
        {
            std::cout << "co_return呼び出し" << std::endl;
            return value;
        }*/

        void unhandled_exception()
        {
            std::cout << "call unhandled_exception" << std::endl;
            throw;
        }


    };

    generator() = default;
    generator(generator const&) = delete;//copy禁止
    generator& operator=(generator const&) = delete;

    //Move
    generator(generator&& other) noexcept : handle(other.handle)
    {
        other.handle = nullptr;
    }

    generator& operator=(generator&& other) noexcept
    {
        if (this != &other)
        {
            handle = other.handle;
            other.handle = nullptr;
        }

        return *this;
    }

    ~generator()
    {
        if (handle != nullptr)//generatorはHandleを消去する
        {
            handle.destroy();
        }
    }

    //Main内でhandle.resume()を呼ぶためのメソッドを定義
    //moveNext風
    bool move_next()
    {
        std::cout << std::endl;
        std::cout << "generator move_next coroutine_handle Resume呼び出し" << std::endl;
        handle.resume();

        return !handle.done();
    }

    int currentValue()
    {
        return handle.promise().value_;
    }

private:
    explicit generator(promise_type& p)
        : handle(coroutine_handle<promise_type>::from_promise(p)) {}

    coroutine_handle<promise_type> handle;

};
