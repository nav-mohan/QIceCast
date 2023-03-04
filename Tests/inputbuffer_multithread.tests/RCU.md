## [RCU Notes from Paul McKinney](https://lwn.net/Articles/262464/)
## [A paper on implementing RCU in C++](https://martong.github.io/high-level-cpp-rcu_informatics_2017.pdf)

* RCU is a synchronization mechanism that allows lock-free concurrency between multiple consumers and one producer.

* It does require more memory though because the proudcer always writes to a new buffer rather than overwriting the same old buffer. It maintains multiple versions of the buffer and holds them until all consumers are done reading from them or some predetermined timeout.

* RCU is meant to be used when you have _far_ more `read` processes that `write` processes. _So, should I use RCU for audio buffers? Or should I use the widely quoted `circular-buffer` or `triple-buffer`?_ I have an `mpeg` encoder, an `aac` encoder, and the `audio-level` meter reading from the same `input-buffer`. If I were to add more encoders then it would be best to use `RCU`.

* RCU is made up of 3 fundamental mechanisms for `insertion`, `deletion`, and `reading`.
    1. Publish-Subscribe : (for `insertions`)
    2. Wait for pre-exisint RCU readers to complete (for `deletion`)
    3. Maintain multiple versions of recently updated objects (for `reading`).

* Usually, a `global pointer` is involved when implementing RCU. This `GP` is pointed to the newly allocated and initialied buffer during `write` processes. So, basically we create a new buffer then we reallocate the `gp` to this new guy `p`.
```
    p = (char*)malloc(PCM_BUFFERSIZE);
    for(int i = 0; i<PCM_BUFFERSIZE; i++)
        p[i] = doSomething(i);
    gp = p;
```
But the compiler could reorder the sequence in which it updates `p[i]`. Basically, we need a special method for reallocating.