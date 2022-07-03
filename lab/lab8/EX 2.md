# EX 2

## a.

`jki`和 `kji`二者差不多快，但一般情况下是前者快，但他们的 `inner loop` 其实 A stride 1,B stride 0,C  stride 1,所以运行效率差不多

## b.

`ikj`和 `kij`都是最慢的，二者都 A stride 0,B stride n, C stride n,所以差不多慢

## c

因为cache的空间局部性(spatial locality)导致的，如果stride比较小，那么在读和写入时cache其实存下了相邻的数据，这样访问变快，性能变好，而如果stride比较大，那么读和写入数据的时候cache并没有存下数据，就会导致访问变慢，性能变差