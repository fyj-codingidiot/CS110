# Ex 3

## p1

n为100的时候差不多快，后面blocked-version更快

这是因为当matrix的size较小的时候，电脑中的cache可以存储下所有的数据，因此这个时候用不用cache block都不太大会影响性能，而当cache的size变大了的时候，电脑中的cahce没办法存储下所有的数据，因此这个时候cache block很好的运用了spatial locality，使得数据的获取变快了

## p2

性能会先上升后下降，上升是因为随着blocksize的增大，cache中可以存储更多的邻近数据，这很好的运用了spatial locality，因此性能上升，而性能下降是因为blocksize再增大超过了cache能存储的数据的大小，因而就会出现miss的情况，这样性能就下降了