#pragma once

#include <memory>

#include "tokenizer.h"

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int64_t value);
    ~Number() = default;

    int64_t GetValue() const;

private:
    int64_t value_;
};

class Symbol : public Object {
public:
    Symbol(std::string symbol);
    ~Symbol() = default;

    const std::string& GetName() const;

private:
    std::string symbol_;
};

class Boolean : public Object {
public:
    Boolean(bool value);
    ~Boolean() = default;

    bool GetValue() const;

private:
    bool value_;
};

class Quote : public Object {
public:
    Quote();
    Quote(std::shared_ptr<Object> cmd);
    ~Quote() = default;

    std::shared_ptr<Object> GetCommand() const;

private:
    std::shared_ptr<Object> cmd_;
};

class Cell : public Object {
public:
    Cell();
    Cell(const std::shared_ptr<Object>& lhs, const std::shared_ptr<Object>& rhs);
    ~Cell() = default;

    void SetFirst(std::shared_ptr<Object>&& other);
    void SetSecond(std::shared_ptr<Object>&& other);

    std::shared_ptr<Object> GetFirst() const;
    std::shared_ptr<Object> GetSecond() const;

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;

    friend std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and conversion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return reinterpret_cast<const std::shared_ptr<T>&>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return dynamic_cast<T*>(obj.get()) != nullptr;
}
