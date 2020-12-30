# PM_example
## PM test
```
cd pmtest
make
numactl -N 0 ./pm_test
```
注意，这里运行PM程序最好使用numactl，将其绑定在socket 0上运行。这是因为在不同的numa结点上访问PM有性能差异，因为服务器的PM插在socket 0上，所以这条命令可以得到最优的PM性能。

关于numa对PM的性能影响，参考论文： https://www.usenix.org/system/files/fast20-yang.pdf
## FAST_FAIR B+tree
持久化B+ tree，全部节点都在PM上。
原论文： https://www.usenix.org/system/files/conference/fast18/fast18-hwang.pdf
```
cd fast_btree
make 
numactl -N 0 ./btree_ycsb 1 6000000
```
同理，绑定numa运行，后边两个参数为线程数和key的数量
## FP tree
只有叶结点在PM上

开源复现 https://github.com/liumx10/ICPP-RNTree/blob/master/include/fptree.h

原论文地址：https://wwwdb.inf.tu-dresden.de/misc/papers/2016/Oukid_FPTree.pdf
