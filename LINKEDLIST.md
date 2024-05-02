# LinkedList (doubly-linked)

Templated doubly-linked linked list using sentinel node.

## Iterators

* **Iterator** 
    * Non-const iterator (modifies elements)
    * `iterator_category`: bidirectional
    * Overloads: move ctor, move assignment, `++`, `--` (prefix/postfix), `*`, `==`, `!=`
    * Templated for const-correctness
* **ConstIterator**
    * Alias for `Iterator<const T>` (read-only)

## Constructors

* `LinkedList()` (default)
* `LinkedList(InputIterator first, InputIterator last)` (copy)
* `LinkedList(std::initializer_list<ValueType> init)` (copy)
* `LinkedList(const LinkedList &other)` (copy)
* `LinkedList(LinkedList &&other) noexcept` (move)

## Assignment Operators

* `LinkedList& operator=(const LinkedList &other)` (copy) 
* `LinkedList& operator=(LinkedList &&other) noexcept` (move)

## Modifying Functions

* `PushBack(const ValueType &value)` (copy)
* `PushBack(ValueType &&value)` (move)
* `EmplaceBack(Args&&... args)` (in-place)
* `PushFront(const ValueType &value)` (copy)
* `PushFront(ValueType &&value)` (move) 
* `EmplaceFront(Args&&... args)` (in-place)
* `PopFront()`  
* `PopBack()`
* `Clear()` 
* `begin() noexcept` 
* `end() noexcept` 

## Non-Modifying Functions

* `Front() const noexcept` 
* `Back() const noexcept` 
* `Size() const noexcept`  
* `Empty() const noexcept` 
* `cbegin() const noexcept`
* `cend() const noexcept` 
