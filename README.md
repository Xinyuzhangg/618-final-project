# Final project for 15618 

Team members: Xinyu Zhang(xinyuzh3), Zhi Lin(zlin2)

#### Video link: https://www.youtube.com/watch?v=rRbvQhK749k

#### Report: [pp_final_report.pdf](https://github.com/Xinyuzhangg/618-final-project/blob/main/pp_final_report.pdf)

### Use

generate trace:

```
mpirun -np 1 ./hashmap -t trace -m 1 -n 100000 -f trace/3.txt -c PUTGET
-c: trace mode: PUTGET PUT GET
-n: lines
-f: file name
```

test:

``` mpirun -np 72 ./hashmap -t parallel -m 8 -h flat -d bsgs
mpirun -np 72 ./hashmap -t parallel -m 8 -h flat -d bsgs
-h: hash mode: linear / flat
-m: master number
```

serial:

```
mpirun -np 1 ./hashmap -t serial
```

## Summary

We are going to implement a K-V storage for parallel computing and do correctness check and performance benchmark for further optimizations.

## Background

In our plan, we may use the K-V storage we design to implement the Shank’s Baby-Step-Giant-Step (BSGS) algorithm to solve the discrete logarithm problems: $a^x \equiv b$ in cryptography. This algorithm will modify the equation to $a^{im} \equiv b a^j \mod p$ and traverse i to find if there’s a j in the hash table. Storing and Finding items in the hash table could be run in parallel because they are independent to each other.

## The Challenge: 

Describe the workload: K-V storage usually map data to different spaces according to the key hash. Describe constraints:  Since the hash is totally different even if the key has only slight difference, it’s very challenging to think of improving locality. Also when huge numbers of threads are doing updates, there are a lot of communication to maintain the coherence.

* high update rates
* low cost by limiting the memory footprint
* support for larger-than memory data
* performance that exceeds pure in-memory data
* structures when the working-set fits in memory

## Resources:

We are still comparing different platforms to use (Cuda, threads, OMP, MPI). Some K-V storage supports concurrency operations like ConcurrentHashmap in Java, but it is unclear about the performance on very high-level parallelism. However, we will investigate some existed solutions and propose our solutions.

GOALS AND DELIVERABLES: Describe the deliverables or goals of your project. This is by far the most important section of the proposal!

1. Implement a k-v storage system in Cuda / MPI, supporting insert, update, search, delete, etc. (75%)
  2. Test the correctness of items and operations, no race conditions. (75%)
  3. Benchmark the performance of different operations. (75%)
  4. Compare with bandwidth and find potential bottlenecks. (100%)
  5. Measure the cache misses and take steps to reduce cache misses. (100%)
  6. Implement an algorithm in parallel using our K-V storage system (we plan to implement Shank’s BSGS algorithm, which is used to find discrete logarithm in cryptography. It has the potential to threaten the secret key of Bitcoin) (125%)
  7. Measure the parallel speedup compared to sequential implementation. (125%)
  8. Provide estimations of real-world applications and illustrate the potential of parallel computing in several fields. (125%)
  9. We will provide specific graphs comparing the speed, through and efficiency before and after parallelization. We will compare different effects of speeding up strategy and attempt to visualize the speeding up by using a web server to make real time test.
  10. We want to extend our work on the basis of a series of papers which would provide us with the insight. We could build upon existent models and see how far we could reach to improve the performance.
  11. We will compare our result and speed up achieved against the SOTA models.


## Platform Choice

We are still comparing platforms like Cuda, OMP, and MPI.

## Schedule

3.27 - 3.31: Investigate existed K-V solutions for concurrent / parallel programs

4.1 - 4.2: Establish the data dependencies and set up models for the project

4.3 - 4.8: Implement the storage

4.9 - 4.10: Correctness test & debug

4.12 - 4.16: Benchmark & performance analysis

4.17 - 4.20: Optimization

4.21 - 4.26: Applications

## Milestone
1. work summary:
   1. Having thoroughly investigated the related work. Including the hash table implementations, methods to optimize collisions and methods to do the parallelization
   2. The setting up the code framework including the starter code where the serial version of the kv store is implemented.
   3. Setting up the tracing part of the code.
   4. Technical path discussion and decision.
   5. Implementation the storage.
   6. Till now, we have finished the sync MPI version of linear hashing and benchmark framework, including different Get/Put patterns and BSGS algorithm.


2. Porject progress
   1. We have mostly lived up to goals we have stated. We think there is no problem to fulfil our initial expection since our implementataion is on track.
   2. Based on the discussion with mentor, we decided to implement the K-V storage based on MPI, and benchmark on PSC. We divided the work into two parts: one implementing linear hashing and one implementing flatten map. We have spent a lot of time tracing recent research and open source projects.
   3. Our updated timeline include:
      - [x] 4.11 - 4.13 Prepare for the exam
      - [x] 4.14 - 4.18 Complete all parallel code for the KV store
      - [x] 4.19 - 4.21 Complete the testing and performance analysis of the work being done
      - [x] 4.22 - 4.25 Based on the analysis done before, seek improvements and optimizations.
      - [x] 4.26 - 4.29 Finish on the report
   4. Our updated poster session goals include:
      - [x] Complete parallelization on more than 2 hash mapping algorithms
      - [x] Improve the parallelism of the code by beating the SOTA serial performance by a factor of 2 using 2 cores
      - [x] The performance of our work tested using a range of bench marks including setting different memory loads.
3. Preliminary results:
   4. Compilable and runnable starting code.
   5. Serial version of the implementation of one algorithm
   6. Trace implementation available.
4. Concerns
   1. The performance of our work is limited subject to the memory load
   2. The parallelization part of the code is not extendible to the situation given the change of test cases
      
