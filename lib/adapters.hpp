#include <iostream>

template <typename T, typename _ = void>
struct is_container : std::false_type {};

template <typename... Ts>
struct is_container_helper {};

template<typename T>
struct is_container<
                      T, std::conditional_t
                      <
                        false,
                        is_container_helper
                        <
                          typename T::iterator,
                          typename T::const_iterator,
                          decltype(std::declval<T>().begin()),
                          decltype(std::declval<T>().end())
                        >,
                        void
                    >
                   > : public std::true_type {};

template <typename T, typename _ = void>
struct is_associative_container : std::false_type {};

template <typename... Ts>
struct is_associative_container_helper {};

template<typename T>
struct is_associative_container<
              T, std::conditional_t
                <
                  false,
                  is_associative_container_helper
                  <
                    typename T::iterator,
                    typename T::const_iterator,
                    typename T::key_type,
                    typename T::value_type,
                    decltype(std::declval<T>().begin()),
                    decltype(std::declval<T>().end())
                  >,
                  void             
                >
              > : public std::true_type{};



template<typename const_Iterator, typename Iterator>
class View{
  public:

    using iterator = Iterator;
    using const_iterator = const_Iterator;

    View(const const_iterator& begin, const const_iterator& end): begin_{const_iterator(begin)}, end_{const_iterator(end)} {}

    const_iterator begin() const {
      return begin_;
    }

    const_iterator end() const {
      return end_;
    }

    const_iterator begin_;
    const_iterator end_;
};

template<typename const_Iterator, typename Iterator, typename Operation>
class View_with_operation {
  public:
    using iterator = Iterator;
    using const_iterator = const_Iterator;

    View_with_operation(const Iterator& it, const Operation& op): begin_{const_iterator(it.begin(), it.end(), op)}, end_{const_iterator(it.end(), it.begin(), op)} {}

    const_iterator begin() const{
      return begin_;
    }

    const_iterator end() const {
      return end_;
    }

    const_iterator begin_;
    const_iterator end_;
};


template <typename Operation>
class Transform {
  private: 
    Operation operation_;

  public:
    Transform(Operation op) : operation_(op) {}

    Operation getOperation() const{
      return operation_;
    }
};

template<typename Iterator, typename Operation>
class transformIterator {
  private:
  Iterator iterator_;
  Iterator end_;
  Operation operation_;

  public:
    transformIterator(const Iterator& it, const Iterator& e, const Operation& op):iterator_{it}, end_{e}, operation_{op} {}

    transformIterator<Iterator, Operation> operator++(){
      ++iterator_;
      return *this;
    }

    transformIterator<Iterator, Operation> operator++(int){
      return ++(*this);
    }

    transformIterator<Iterator, Operation> operator--(){
      --iterator_;
      return *this;
    }

    transformIterator<Iterator, Operation> operator--(int){
      return --(*this);
    }

    auto operator*() const {
      return operation_(*iterator_);
    }

    bool operator==(const transformIterator<Iterator, Operation>& other) const{
      return iterator_== other.iterator_;
    }

    bool operator!=(const transformIterator<Iterator, Operation>& other) const{
      return !(iterator_==other.iterator_);
    }
};

template<typename Iterator, typename Operation>
class transformView: public virtual View_with_operation<transformIterator<typename Iterator::const_iterator, Operation>, Iterator, Operation> {
  static_assert(is_container<Iterator>(), "It isn't container.");
  public:
  using const_iter= transformIterator<typename Iterator::const_iterator, Operation>;
  using parent = View_with_operation<const_iter, Iterator, Operation>;
  transformView(const Iterator& it, const Operation& op):
    parent{it, op} {}
};

template<typename Iterator, typename Operation>
transformView<Iterator, Operation> operator| (const Iterator& iterator, const Transform<Operation>& transform){
  return transformView<Iterator, Operation>(iterator, transform.getOperation());
}

template<typename Operation>
class Filter{
  private:
  Operation operation_;

  public:
  Filter(Operation op): operation_{op} {}

  Operation getOperation() const{
    return operation_;
  }
};

template<typename Iterator, typename Operation>
class filterIterator{
  private:
  Iterator iterator_;
  Iterator end_;
  Operation operation_;

  public:

  filterIterator(const Iterator& it, const Iterator& e, const Operation& op): iterator_{it}, end_{e}, operation_{op} {}

  filterIterator<Iterator, Operation> operator++(){
    ++iterator_;
    while (iterator_!=end_ && !operation_(*iterator_)){
      ++iterator_;
    }
    return *this;
  }

  filterIterator<Iterator, Operation> operator--(){
    --iterator_;
    while (iterator_!=end_ && !operation_(*iterator_)) {
      --iterator_;
    }
    return *this;
  }

  auto operator*() const{
    return *iterator_;
  }

  bool operator==(const filterIterator& other) const{
    return iterator_==other.iterator_;
  }

  bool operator!=(const filterIterator& other) const{
    return iterator_!=other.iterator_;
  }
};

template<typename Iterator, typename Operation>
class filterView: public virtual View_with_operation<filterIterator<typename Iterator::const_iterator, Operation>,Iterator, Operation> {
  static_assert(is_container<Iterator>(), "It isn't container.");
  public:
  using const_iter = filterIterator<typename Iterator::const_iterator, Operation>;
  using parent = View_with_operation<const_iter, Iterator, Operation>;
  filterView<Iterator, Operation>(const Iterator& it, const Operation& op): 
    parent{it, op} {
      while (parent::begin_ != parent::end_ && !op(*parent::begin_)) {
        ++parent::begin_;
      }
    }
};

template<typename Iterator, typename Operation>
filterView<Iterator, Operation> operator| (const Iterator& iterator, const Filter<Operation>& filter){
  return filterView<Iterator, Operation>(iterator, filter.getOperation());
}

class Take{
  public:
    Take(size_t mx_cnt): max_count_{mx_cnt} {}

    size_t getMaxCount() const{
      return max_count_;
    }

  private:
    size_t max_count_;
};

template<typename Iterator>
class takeView: public virtual View<typename Iterator::const_iterator, Iterator>{
  static_assert(is_container<Iterator>(), "It isn't container.");
  public:
  using const_iter = Iterator::const_iterator;
  using parent = View<const_iter, Iterator>;
  takeView(const Iterator& it, size_t mx_cnt):
    parent(it.begin(), it.begin()) {
      while (parent::end_ != it.end() && mx_cnt) {
        ++parent::end_;
        --mx_cnt;
      }
    }
};

template<typename Iterator>
takeView<Iterator> operator|(const Iterator& iterator, const Take& take_operation){
  return takeView(iterator, take_operation.getMaxCount());
}

class Drop{
  public:
    Drop(size_t mx_cnt): max_count_{mx_cnt} {}

    size_t getMaxCount() const{
      return max_count_;
    }

  private:
    size_t max_count_;
};

template<typename Iterator>
class dropView: public virtual View<typename Iterator::const_iterator, Iterator>{
  static_assert(is_container<Iterator>(), "It isn't container.");
  public:
  using const_iter = Iterator::const_iterator;
  using parent = View<const_iter, Iterator>;
  dropView(const Iterator& it, size_t mx_cnt):
    parent(it.begin(), it.end()){
      while (parent::begin_!=parent::end_ && mx_cnt) {
        ++parent::begin_;
        --mx_cnt;
      }
  }
};

template<typename Iterator>
dropView<Iterator> operator|(const Iterator& iterator, const Drop& take_operation){
  return dropView(iterator, take_operation.getMaxCount());
}

class Reverse{};

template<typename Iterator>
class reverseIterator{
  private:
    Iterator iterator_;

  public:
    reverseIterator(const Iterator& it): iterator_{it} {}

    reverseIterator<Iterator> operator++(){
      --iterator_;
      return *this;
    }

    reverseIterator<Iterator> operator--(){
      ++iterator_;
      return *this;
    }

    reverseIterator<Iterator> operator++(int){
      return ++(*this);
    }

    reverseIterator<Iterator> operator--(int){
      return --(*this);
    }

    auto operator*() const{
      auto temp = iterator_;
      return *(--temp);
    }

    bool operator==(const reverseIterator& other) const {
      return iterator_==other.iterator_;
    }

    bool operator!=(const reverseIterator& other) const {
      return iterator_!=other.iterator_;
    }
};

template<typename Iterator>
class reverseView: public virtual View<reverseIterator<typename Iterator::const_iterator>, Iterator>{
  static_assert(is_container<Iterator>(), "It isn't container.");
  public:
  using const_iter = reverseIterator<typename Iterator::const_iterator>;
  using parent = View<const_iter, Iterator>;
  reverseView(const Iterator& it):
    parent(it.end(), it.begin()) {}
};

template<typename Iterator>
reverseView<Iterator> operator|(const Iterator& iterator, const Reverse& reverse){
  return reverseView(iterator);
}

class Keys{};

template<typename Iterator>
class keysIterator{
  private:
    Iterator iterator_;
  public:
    keysIterator(const Iterator& it): iterator_{it} {}

    keysIterator<Iterator> operator++(){
      ++iterator_;
      return *this;
    }
    
    keysIterator<Iterator> operator--(){
      --iterator_;
      return *this;
    }

    keysIterator<Iterator> operator++(int){
      return ++(*this);
    }

    keysIterator<Iterator> operator--(int){
      return --(*this);
    }

    auto operator*() const {
      return iterator_->first;
    }

    bool operator==(const keysIterator<Iterator>& other) const {
      return iterator_ == other.iterator_;
    }

    bool operator!=(const keysIterator<Iterator>& other) const {
      return !(iterator_ == other.iterator_);
    }
};

template<typename Iterator>
class keysView: public virtual View<keysIterator<typename Iterator::const_iterator>, Iterator>{
  using key_type = Iterator::key_type;
  using value_type = Iterator::value_type;
  static_assert(is_associative_container<Iterator>(), "It isn't associative container");
  public:
  using const_iter = keysIterator<typename Iterator::const_iterator>;
  using parent = View<const_iter, Iterator>;
  keysView(const Iterator& it):
     parent(it.begin(), it.end()){}
};

template<typename Iterator>
keysView<Iterator> operator| (const Iterator& iterator, const Keys& keys){
  return keysView(iterator);
}

class Values{};

template<typename Iterator>
class valuesIterator{
  private:
    Iterator iterator_;
  public:
    valuesIterator(const Iterator& it): iterator_{it} {}

    valuesIterator<Iterator> operator++(){
      ++iterator_;
      return *this;
    }
    
    valuesIterator<Iterator> operator--(){
      --iterator_;
      return *this;
    }

    valuesIterator<Iterator> operator++(int){
      return ++(*this);
    }

    keysIterator<Iterator> operator--(int){
      return --(*this);
    }

    auto operator*() const {
      return iterator_->second;
    }

    bool operator==(const valuesIterator<Iterator>& other) const {
      return iterator_ == other.iterator_;
    }

    bool operator!=(const valuesIterator<Iterator>& other) const {
      return iterator_ != other.iterator_;
    }
};

template<typename Iterator>
class valuesView: public virtual View<valuesIterator<typename Iterator::const_iterator>, Iterator>{
  using key_type = Iterator::key_type;
  using value_type = Iterator::value_type;
  static_assert(is_associative_container<Iterator>(), "It isn't associative container");
  public:
  using const_iter = valuesIterator<typename Iterator::const_iterator>;
  using parent = View<const_iter, Iterator>;
  valuesView(const Iterator& it):
     parent(it.begin(), it.end()){}
};

template<typename Iterator>
valuesView<Iterator> operator| (const Iterator& iterator, const Values& keys){
  return valuesView(iterator);
}