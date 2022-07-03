#include<iostream>
#include<cassert>
#include"ringbuffer.hpp"
#include <vector>
int add_five(int x){
    return x + 5;
}
int mul_five(int x){
    return x * 5;
}
int main(){
    RingBuffer<int> rb;
    std::cout<<"========== BEGIN TEST ==========" << std::endl;
    RingBuffer<int> rb2;
    //int i,j,data;
    int data;
    //int round = 0;
    for(int i = 0;i<100;i++){
        //bool flag = false;
        for(int j = 0;j<20*(i+5);j++){
            assert(rb2.write(j*2));   
        }
        for(int j=0;j<20*(i+5);j++){
            assert(rb2.read(data));
            assert(data == j*2);
        }

    }
    //std::cout<<rb._M_content[38]<<std::endl;
    assert(rb.is_empty());
    assert(rb.get_capacity()==8);
    std::vector<int> rbuf,wbuf;
    rbuf.resize(20);
    wbuf.resize(20);
    for(int i=0;i<20;i++){
        wbuf[i] = i+1;
        rbuf[i] = 0;
    }
    rb.write_multi(20,wbuf);
    assert(rb.get_capacity()==32);
    assert(!rb.is_empty());
    assert(rb.read_multi(10,rbuf));
    for(int i=0;i<10;i++){
        assert(rbuf[i] == i+1);
    }
    rb.map(mul_five);
   // std::cout<<std::endl;
    for(int i=10;i<20;i++){
        int x;
        assert(rb.read(x));
        assert(x==(i+1)*5);
    }
    
    assert(rb.is_empty());
    //begin test for iter
    for(int i=0;i<10;i++){
        assert(rb.write(i));
    }
    int index = 0;
    for(__detail::__iterator<int> i =rb.begin();i != rb.end();i++){
        assert(*i == index);
        index+=1;
    }
    index = 0;
    for(__detail::__const_iterator<int> i =rb.cbegin();i != rb.cend();i++){
        assert(*i == index);
        index+=1;
    }
    RingBuffer<int> rb1;
    wbuf.resize(32);
    for(int i=0;i<32;i++){
        wbuf[i] = i+1;
    }
    rb1.write_multi(32,wbuf);
    __detail::__iterator<int> i =rb1.begin();
    i +=2;
    assert(*(i) == 3);
    __detail::__const_iterator<int> j =rb1.cbegin();
    j+=2;
    assert(*(j) == 3);
    __detail::__iterator<int> g =rb1.end();
    assert(*(g-2) == 31);
    g-=2;
    assert(*(g) == 31);
    __detail::__const_iterator<int> k =rb1.cend();
    assert(*(k-2) == 31);
    k-=2;
    assert(*(k) == 31);
    int x;
    rb1.read(x);
    rb1.read(x);
    rb1.read(x);
    rb1.write(1);
    for(__detail::__iterator<int> i = rb1.begin();i!=rb1.end();i++){
        std::cout<<*i<<" ";
    }
    std::cout<<std::endl;
    for(__detail::__iterator<int> i = rb1.begin();i!=rb1.end();++i){
        std::cout<<*i<<" ";
    }
    std::cout<<std::endl;
    for(__detail::__iterator<int> i = rb1.end()-1;i!=rb1.begin()-1;i--){
        std::cout<<*i<<" ";
    }
    std::cout<<std::endl;
    for(__detail::__iterator<int> i = rb1.end()-1;i!=rb1.begin()-1;--i){
        std::cout<<*i<<" ";
    }
    std::cout<<std::endl;
    for(__detail::__const_iterator<int> i =rb1.cbegin();i !=rb1.cend();i++){
        std::cout<<*i<<" ";
    }
    std::cout<<std::endl;
    for(__detail::__const_iterator<int> i =rb1.cbegin();i !=rb1.cend();++i){
        std::cout<<*i<<" ";
    }
    std::cout<<std::endl;
    for(__detail::__const_iterator<int> i =rb1.cend()-1;i !=rb1.cbegin()-1;i--){
        std::cout<<*i<<" ";
    }
    std::cout<<std::endl;
    for(__detail::__const_iterator<int> i =rb1.cend()-1;i !=rb1.cbegin()-1;--i){
        std::cout<<*i<<" ";
    }
    std::cout<<std::endl;
    /*if correct,your output should be 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 1
        for ++ operation */
    /* if correct,your output should be 1 32 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 
        for -- operation */
    std::cout<<"Congratulations! You passed the simple test!"<<std::endl;
    return 0;
}