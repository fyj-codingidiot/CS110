namespace __detail{     //the namespace of detail
template <typename _Tp>  // use template
class __iterator{       // the implemention of  __iterator
public:
    RingBuffer<_Tp>* rb;
    int index;
    _Tp* m_ptr;        //data pointer
    __iterator(){      // default constructor
        m_ptr = nullptr;
        rb = nullptr;
        index = -1;
    }
    __iterator(const __iterator& other){ //copy constructor
        m_ptr = other.m_ptr;
        rb = other.rb;
        index = other.index;
    }
    __iterator(_Tp* ptr, RingBuffer<_Tp>* buffer){               //Initialize the iterator with a pointer
        m_ptr = ptr;
        rb = buffer;
        ptrdiff_t diff = ptr - (&rb->_M_content[0]);
        index =(int)diff;
    }
    //use id 
    __iterator(int id,RingBuffer<_Tp>* buffer){
        rb = buffer;
        index = id;
        m_ptr = &(rb->_M_content[id]);
    }
    ~__iterator()=default;//use defalut
    __iterator<_Tp>& operator=(const __iterator& rhs){ //copy assignment operator
        m_ptr = rhs.m_ptr;
        rb = rhs.rb;
        index = rhs.index;
        return *this;
    }
    _Tp& operator*(){       //dereference operator
        return *m_ptr;
    }
    _Tp* operator->(){      //arrow operator
        return m_ptr;
    }
    bool operator==(const __iterator& rhs){ //comparison operator 
        return this->m_ptr == rhs.m_ptr;
    }
    bool operator!=(const __iterator& rhs){ //comparison operator
        return this->m_ptr != rhs.m_ptr;
    }
    __iterator<_Tp>& operator++(){   //self increment like (++i)
       index = (index+1+rb->_M_capacity+1) %(rb->_M_capacity+1);
       m_ptr = &(rb->_M_content[index]);
       return *this;
    }
    __iterator<_Tp> operator++(int){ //self increment like(i++)
        __iterator<_Tp> res(*this);
        index = (index+1+rb->_M_capacity+1) %(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return res; //return a new object
    }
    __iterator<_Tp>& operator--(){   //self decrement like(--i)
        index = (index-1+rb->_M_capacity+1) %(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return *this;
    }
    __iterator<_Tp> operator--(int){ //self decremnet like(i--)
        __iterator<_Tp> res(*this);
        index = (index-1+rb->_M_capacity+1) %(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return res; //return a new object
    }
    __iterator<_Tp> operator+(int rhs){   //arithmetic operator
        int new_index = (index+rhs+rb->_M_capacity+1)%(rb->_M_capacity+1);
        //_Tp* new_m_ptr = &(rb->_M_content[index]);
        return __iterator(new_index,rb);
    }
    __iterator<_Tp> operator-(int rhs){  //arithmetic  operator
        int new_index = (index-rhs+rb->_M_capacity+1)%(rb->_M_capacity+1);
        //_Tp* new_m_ptr = &(rb->_M_content[index]);
        return __iterator(new_index,rb);
    }
    __iterator<_Tp>& operator+=(int rhs){   //compound assignment operator
        index = (index+rhs+rb->_M_capacity+1)%(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return *this;
    }
    __iterator<_Tp>& operator-=(int rhs){   //compound assignment operator
        index = (index-rhs+rb->_M_capacity+1)%(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return *this;
    }
};
//the implemention of __const_iter
template <typename _Tp>
class __const_iterator{
public:
    const RingBuffer<_Tp>* rb;//pointer
    int index;//index
    const _Tp* m_ptr;        //data pointer
    __const_iterator(){      // default constructor
        m_ptr = nullptr;
        rb=nullptr;
        index=-1;
    }
    __const_iterator(const __const_iterator& other){ //copy constructor
        m_ptr = other.m_ptr;
        index = other.index;
        rb = other.rb;
    }
    __const_iterator(const __iterator<_Tp>& other){ //copy constructor
        m_ptr = other.m_ptr;
        index = other.index;
        rb = other.rb;
    }
    __const_iterator(const _Tp* ptr,const RingBuffer<_Tp>* buffer){               //Initialize the iterator with a pointer
        m_ptr = ptr;
        rb = buffer;
        ptrdiff_t diff = ptr - (&rb->_M_content[0]);
        index =(int)diff;
    }
    //use id to construct
    __const_iterator(int id,const RingBuffer<_Tp>* buffer){
        rb = buffer;
        index = id;
        m_ptr = &(rb->_M_content[id]);
    }
    ~__const_iterator()=default;//use default
   __const_iterator<_Tp>& operator=(const __const_iterator& rhs){ //copy assignment operator
        m_ptr = rhs.m_ptr;
        rb = rhs.rb;
        index = rhs.index;
        return *this;
    }
    const _Tp& operator*(){       //dereference operator
        return *m_ptr;
    }
    const _Tp* operator->(){      //arrow operator
        return m_ptr;
    }
    bool operator==(const __const_iterator& rhs){ //comparison operator 
        return this->m_ptr == rhs.m_ptr;
    }
    bool operator!=(const __const_iterator& rhs){ //comparison operator
        return this->m_ptr != rhs.m_ptr;
    }
    __const_iterator<_Tp>& operator++(){   //self increment like (++i)
        index = (index+1+rb->_M_capacity+1) %(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return *this;
    }
    __const_iterator<_Tp> operator++(int){ //self increment like(i++)
        __const_iterator<_Tp> res(*this);
        index = (index+1+rb->_M_capacity+1) %(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return res; //return a new object
    }
    __const_iterator<_Tp>& operator--(){   //self decrement like(--i)
        index = (index-1+rb->_M_capacity+1) %(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return *this;
    }
    __const_iterator<_Tp> operator--(int){ //self decremnet like(i--)
        __const_iterator<_Tp> res(*this);
        index = (index-1+rb->_M_capacity+1) %(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return res; //return a new object
    }
    __const_iterator<_Tp> operator+(int rhs){   //arithmetic operator
        int new_index = (index+rhs+rb->_M_capacity+1)%(rb->_M_capacity+1);
        //_Tp* new_m_ptr = &(rb->_M_content[index]);
        return __const_iterator(new_index,rb);
    }
    __const_iterator<_Tp> operator-(int rhs){  //arithmetic  operator
        int new_index = (index-rhs+rb->_M_capacity+1)%(rb->_M_capacity+1);
        //_Tp* new_m_ptr = &(rb->_M_content[index]);
        return __const_iterator(new_index,rb);
    }
    __const_iterator<_Tp>& operator+=(int rhs){   //compound assignment operator
        index = (index+rhs+rb->_M_capacity+1)%(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return *this;
    }
    __const_iterator<_Tp>& operator-=(int rhs){     //compound assignment operator
        index = (index-rhs+rb->_M_capacity+1)%(rb->_M_capacity+1);
        m_ptr = &(rb->_M_content[index]);
        return *this;
    }
};
}

// The implemention for ringbuffer
// Type renaming
//    typedef size_t                                           size_type;
//    typedef _Tp                                              value_type;
//    typedef _Tp*                                             pointer;
//    typedef _Tp&                                             reference;
//    typedef const _Tp&                                       const_reference;
//    typedef ptrdiff_t                                        difference_type;
//    typedef typename __detail::__iterator<_Tp>               iterator;
//    typedef typename __detail::__const_iterator<_Tp>         const_iterator;

/* Create a new ring buffer. You should allocate memory for its _M_content,
      initialize the _M_read_pos, _M_write_pos, and its _M_capacity to RING_BUFFER_INIT_SIZE */
template <typename _Tp>
RingBuffer<_Tp>::RingBuffer(){
    _M_capacity = RING_BUFFER_INIT_SIZE; //init capcity
    _M_content = new _Tp[RING_BUFFER_INIT_SIZE+1]; //init content
    _M_read_pos = 0; //init read pos;
    _M_write_pos = 0; //init write pos;
}
/* Destroy the given ring buffer, free all resources you've allocated. */
template <typename _Tp>
RingBuffer<_Tp>::~RingBuffer(){
    _M_capacity = 0; // set the _M_capacity 0
    _M_read_pos = 0; // set the _M_read_pos 0
    _M_write_pos = 0; // set the _M_write_pos 0
    delete[] _M_content; //free mem
    //_M_content = nullptr; //trun to null
}

/* Returns its _M_capacity */
template <typename _Tp>
size_t RingBuffer<_Tp>:: get_capacity() const{
    return _M_capacity;
}

/* Returns true if the given ring buffer is empty, false otherwise */
template <typename _Tp>
bool RingBuffer<_Tp>::is_empty() const{
    size_t st =(_M_write_pos - _M_read_pos+_M_capacity+1)%(_M_capacity+1); /*calculate the size */
    if(st==0){
        return true; //if st==0 return true
    }
    else{
        return false; //return true otherwise
    }
}

/* Read an element from the ring buffer (i.e., the dequeue operation), *data records 
   the element popped. returns true if the operation succeeded, false otherwise. */
template <typename _Tp>
bool RingBuffer<_Tp>::read(_Tp& data){
     //if data is null return false
    if(is_empty()) return false; //if buffer is empty return false;
    else{
        //std::cout<<"The M_content read is "<<_M_content[_M_read_pos]<<std::endl;
        data = _M_content[_M_read_pos]; //read
        //std::cout<<"The reading data is "<<data<<std::endl;
        //std::cout<<"The read pos is "<<_M_read_pos<<std::endl;
        //_M_content[_M_read_pos] = 0x3f3f3f3f; //pop it
        _M_read_pos = (_M_read_pos+1) % (_M_capacity+1); // update read_pos
        return true; //return true
    }
    return false;
}

/* Grows the ringbuffer _M_capacity according to the growth factor */
template <typename _Tp>
void RingBuffer<_Tp>::grow(){
    size_t st =(_M_write_pos - _M_read_pos+_M_capacity+1)%(_M_capacity+1); //calculate size 
    if(st>=_M_capacity){
        size_t new_capacity = 0;
        if(_M_capacity<1024){ //judge
            new_capacity = _M_capacity*RING_BUFFER_GROW_FACTOR1+1;
        }
        else{  //another case
            new_capacity = _M_capacity*RING_BUFFER_GROW_FACTOR2+1;
        }
        _Tp* new_content = new _Tp[new_capacity]; //apply for a new mem
        // for(size_t i=0;i<_M_capacity;i++){
        //     _M_content[i] = 0x3f3f3f3f;
        // }
        int index = 0;
        while(!is_empty()){  //copy the data
            read(new_content[index]);
            index +=1; //update index
        }
        delete[] _M_content;
        //_M_content=nullptr; //delete
        _M_read_pos = 0; //renew
        _M_write_pos = st;
        _M_content = new_content;
        _M_capacity = new_capacity-1;
    }
    else{
        return;//return otherwise
    }
}

 /* Write an element which value is `data` to the ring buffer (i.e., the enqueue operation),
   returns true if the operation succeeded, false otherwise. Note that when the buffer is 
   full, grow its size to make sure the element can be write successfully. The detailed 
   requirements of size growth are in the homework description. */
template <typename _Tp>
bool RingBuffer<_Tp>::write(const _Tp& data){
     //if data is null return false
     //grow size first
     //if data is null return false
     //grow size first
    grow();
    //std::cout<<"The writing data is "<<data<<std::endl;
    _M_content[_M_write_pos] = data; //write
    //std::cout<<"The data in M_content is "<<_M_content[_M_write_pos]<<std::endl;
    //std::cout<<_M_content[_M_write_pos]<<" "<<_M_write_pos<<" ";
    //std::cout<<"The writing pos "<<_M_write_pos<<std::endl;
    _M_write_pos = (_M_write_pos+1)%(_M_capacity+1); //update write pos
    return true;

}

 /* Read "rdsize" elements from the ring buffer, and fill them to "data".
   Notice that you should only return true when this operation succeeds.
   If there are not enough elements in the buffer, just DO NOTHING and 
   return false. DO NOT modify the ring buffer and data for such cases.
   If size of "data" vector does not match "rdsize", you should resize "data" */
template <typename _Tp>
bool RingBuffer<_Tp>::read_multi(size_t rdsize, std::vector<_Tp>& data){
    size_t st =(_M_write_pos - _M_read_pos+_M_capacity+1)%(_M_capacity+1); //calculate size
    if(rdsize == 0 || is_empty()) return false; //if buffer is null 
    if(st<rdsize) return false;  // if st < rd size
    //if data is null
    if(data.size()!= rdsize){
        data.resize(rdsize);   //resize if data's size can't match rdsize
    }
    for(size_t i=0;i<rdsize;i++){  //read the data 
        read(data[i]);
    }
    return true; //return true
}

/* Write "wrtsize" elements to the ring buffer. The values to write are 
   provided in "data". Returns true if succeeds, false otherwise. Don't 
   forget to grow your buffer size properly. */
template <typename _Tp>
bool RingBuffer<_Tp>::write_multi(size_t wrtsize, const std::vector<_Tp>& data){
    if(data.size()<wrtsize) return false;  //judge whether data size is smaller.
    if(!wrtsize) return false;
    else{
        for(size_t i=0;i<wrtsize;i++){ //write 
            write(data[i]);
        }
        //std::cout<<std::endl;
        return true;
    }
    return false;
}

/* For every element in the ring buffer, apply "func" to it. For example,
   Your buffer currently contains 3 elements 1, 2, 4. If the map function is to 
   "add five", then their values should be 6, 7, 9 after calling this function. */
template <typename _Tp>
bool  RingBuffer<_Tp>::map(std::function<_Tp(_Tp)>&& func){
    //if(!(&func)) return false; //if func is null
    size_t index = _M_read_pos;
    while(index != _M_write_pos){ //begin func
        _M_content[index] = func(_M_content[index]);
        index = (index+1) %(_M_capacity+1); //update
    }
    return true;
}

/* The iterator is used to interate through the ringbuffer. By iterating through the ring buffer, it means start from the oldest element in the ring buffer (i.e. the element pointed by "_M_read_pos"), to the latest one. end() should return the slot after the last element. You can refer to standart library for more info  */
template <typename _Tp>
typename RingBuffer<_Tp>::iterator RingBuffer<_Tp>::begin(){
    return iterator(_M_read_pos,this);  // use pointer to construct 
}

/* The iterator is used to interate through the ringbuffer. By iterating through the ring buffer, it means start from the oldest element in the ring buffer (i.e. the element pointed by "_M_read_pos"), to the latest one. end() should return the slot after the last element. You can refer to standart library for more info  */
template <typename _Tp>
typename RingBuffer<_Tp>::iterator RingBuffer<_Tp>::end(){
    return iterator(_M_write_pos,this);  // use pointer to construct 
}

// Const iterators
template <typename _Tp>
typename RingBuffer<_Tp>::const_iterator RingBuffer<_Tp>::cbegin() const{
    return const_iterator(_M_read_pos,this);  // use pointer to construct 
}

// Const iterators
template <typename _Tp>
typename RingBuffer<_Tp>::const_iterator RingBuffer<_Tp>::cend() const{
    return const_iterator(_M_write_pos,this);  // use pointer to construct 
}