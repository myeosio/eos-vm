# EOS VM - A Low-Latency, High Performance and Extensible WebAssembly Engine

- Extremely Fast Execution - 极快的执行(比WABT快6倍)
- Extremely Fast Parsing/Loading - 极其快速解析/加载(比WABT快20倍)
- Efficient Time Bound Execution - 有效的时间执行
- Deterministic Execution (Soft Float & Hardware Floating Point options) - 确定性执行(软浮动&硬件浮点选项)
- Standards Compliant - 标准兼容
- Designed for Parallel Execution - 设计为并行执行
- C++ / Header Only - 只需c++头
- Simple API for integrating Native Calls – 为集成本地调用设计的简单API

## Motivation – 动机
EOS VM is designed from the ground up for the high demands of blockchain applications which require far more from a WebAssembly engine than those designed for web browsers or standards development. In the world of blockchain, any non-deterministic behavior, unbounded computation, or unbounded use of RAM can take down the blockchain for everyone, not just a single user's web browser. Single threaded performance, fast compilation/validation of Wasm, and low-overhead calls to native code are critical to blockchains.

EOS VM是为区块链应用程序的高要求而设计的，区块链应用程序对WebAssembly引擎的要求远高于那些为web浏览器或标准开发而设计的应用程序。在区块链的世界里，任何不确定性的行为、无限制的计算或无限制的RAM使用都可能破坏区块链，而不仅仅是单个用户的web浏览器。单线程性能、Wasm的快速编译/验证以及对本地代码的低开销调用对区块链至关重要。

While EOS VM was designed for blockchain, we believe it is ideally suited for any application looking to embed a High Performance WebAssembly engine.

虽然EOS VM是为区块链设计的，但我们相信它非常适合任何希望嵌入高性能WebAssembly引擎的应用程序。

We designed EOS VM to meet the needs of EOSIO blockchains after using three of the most common WebAssembly engines: Binaryen, WABT, and WAVM. These WebAssembly engines were the single largest source of security issues impacting EOSIO blockchains. While WAVM provides extremely fast execution, it is not suited to running a live blockchain because it has extremely long and unpredictable compilation times and the need to recompile all contracts every time the process restarts.  WABT was designed as a toolkit for manipulating WebAssembly first and as an execution engine second.

在使用了Binaryen、WABT和WAVM这三种最常见的WebAssembly引擎后，我们设计了EOS VM来满足EOSIO区块链的需求。这些WebAssembly引擎是影响EOSIO区块链安全问题的最大单一来源。虽然WAVM提供了非常快的执行速度，但它不适合运行一个实时的区块链，因为它有非常长的和不可预测的编译时间，而且每次进程重启时都需要重新编译所有的契约。WABT首先被设计成一个操作WebAssembly的工具包，其次是一个执行引擎。

We considered the WebAssembly engines used by the largest browsers, but they all come with considerable overhead and assumptions which are inappropriate for a reusable library or to be embedded in a blockchain. It is our hope that one day major browsers will opt to switch to EOS VM. 

我们考虑过大型浏览器使用的WebAssembly引擎，但它们都带来了相当大的开销和假设，这对于可重用库或嵌入区块链来说是不合适的。我们希望有一天主流浏览器会选择切换到EOS VM。

All of the existing libraries incorporate a large code base designed and implemented by engineers not trained in the rigor of blockchain development. This makes the code difficult to audit and keeping up with upstream changes / security breaches difficult. 

所有现有的库都包含了一个大型的代码库，这些代码库是由没有接受过区块链开发严格培训的工程师设计和实现的。这使得代码很难审计，也很难跟上上游的变化/安全漏洞。

With WebAssembly (Wasm) becoming ever more ubiquitous, there is a greater need for a succinct implementation of a Wasm backend.  We implemented __EOS VM__ because all existing backends we evaluated fell short in meeting our needs for a Wasm backend best suited for use in a public blockchain environment. 

随着WebAssembly (Wasm)变得越来越普遍，对简洁的Wasm后端实现的需求越来越大。我们实现EOS VM是因为我们评估的所有现有后端都无法满足我们对最适合在公共区块链环境中使用的Wasm后端的需求。

## Deterministic Execution – 确定执行
Given that all programs on the blockchain must be deterministic, floating point operations are of particular interest to us.  Because of the non-deterministic nature of rounding modes, NaNs and denormals, special care has to be made to ensure a deterministic environment on all supported platforms.  This comes in the form of "softfloat", a software implementation of IEEE-754 float point arithmetic, which is constrained further to ensure determinism.  If this determinism is not required, hardware based floating point operations are still available through a compile time define.

由于区块链上的所有程序都必须是确定性的，所以我们对浮点运算特别感兴趣。由于舍入模式、nan和非正常模式的不确定性本质，必须特别注意确保所有支持平台上的确定性环境。这是以“softfloat”的形式出现的，这是一个IEEE-754浮点算法的软件实现，进一步限制以确保确定性。如果不需要这种确定性，则仍然可以通过编译时定义使用基于硬件的浮点运算。

Any secondary limits/constraints (i.e. stack size, call depth, etc.) can cause consensus failures if these restrictions do not match any previous backend that was in place, __EOS VM__ has all of these constraints user definable through either a compile-time system or run-time based on the use case and data type involved.

任何二次限制/约束(即堆栈大小,调用深度,等等)会导致共识失败如果这些限制不匹配任何先前的后端, __EOS_VM__ 拥有所有这些限制用户可定义通过编译时系统或运行时基于用例和数据类型。

## Time Bounded Execution - 时间有限的执行
The ability to ensure that execution doesn't over run the CPU time that is allotted for a given program is a central component of a resource limited blockchain.  This is satisfied by the watchdog timer system (as mentioned below, this mechanism is also useful for general security). EOS VM's implementation is both fast and efficient compared to prior solutions. 

确保执行不会超过分配给给定程序的CPU时间的能力是有限资源区块链的核心组件。看门狗计时器系统可以满足这一要求(如下所述，这种机制对于一般安全性也很有用)。与以前的解决方案相比，EOS VM的实现既快速又高效。

Two mechanisms are available to the user to bound the execution of Wasm:

用户可以使用两种机制来绑定Wasm的执行:

  1) A simple instruction counter based bounding, this incurs a performance penalty, but doesn't require multi-threading.
  
  1) 一个简单的基于指令计数器的边界，这会导致性能损失，但不需要多线程。
  
  2) A watchdog timer solution that incurs no noticeable overhead during Wasm execution.
  
  2) 看门狗计时器解决方案，在Wasm执行期间不会引起明显的开销。

## Secure by Design – 安全设计
WebAssembly was designed to run untrusted code in a browser environment where the worst that can happen is a hung browser. Existing libraries such as WABT, WAVM, and Binaryen were designed with assumptions which can lead to unbounded memory allocation, extremely long load times, and stack overflows from recursive descent parsing or execution.  

WebAssembly被设计成在浏览器环境中运行不受信任的代码，而最坏的情况就是浏览器被挂起。现有的库，如WABT、WAVM和Binaryen，在设计时都有一些假设，这些假设可能导致无限的内存分配、极长的加载时间和由于递归下降解析或执行而导致的堆栈溢出。

The fundamental data types that make up __EOS VM__ are built with certain invariants from the onset.  This means that explicit checks and validations, which can be error-prone because of programmer forgetfulness, are not needed as the data types themselves maintain these invariants and kill the execution if violated.  

构成EOS VM的基本数据类型从一开始就是用某些不变量构建的。这意味着不需要显式的检查和验证，因为数据类型本身会维护这些不变量并在违反时终止执行，而显式的检查和验证可能因为程序员的遗忘而容易出错。

In addition to these core data types, some of the special purpose allocators utilize the security of the CPU and core OS to satisfy that memory is properly sandboxed (a guard paging mechanism).  

除了这些核心数据类型，一些特殊用途的分配器利用CPU和核心操作系统的安全性来满足内存被适当地沙箱化(一种保护分页机制)的要求。

Because of the utilization of guard paging for the memory operations, host function calls that execute natively don't have to explicitly validate pointers that are passed into these functions if access outside of the sandboxed memory occurs, please note special care should be made to ensure that the host function can fail hard, i.e. not call destructors and have no negative impact.

因为利用保护分页的内存操作,主函数调用执行本地不需要显式验证指针传递到这些功能如果访问沙箱以外的内存时,请注意特别注意应努力确保主机功能可以失败,即不调用析构函数,没有负面影响。

At no point during parsing or evaluation does EOS-VM use unbounded recursion or loops, everything is tightly bound to limit or eliminate the ability for a bad or corrupt Wasm to cause a crash or infinitely hang the machine.

在解析或计算过程中，EOS-VM没有使用无界递归或循环，所有的东西都紧密绑定在一起，以限制或消除坏的或损坏的Wasm导致崩溃或无限挂起机器的能力。

All of these solutions are transparent to the developer and allow for more succinct functions that are not cluttered with external checks and only the core logic is needed in most places.  

所有这些解决方案对开发人员来说都是透明的，并且允许更简洁的函数，这些函数不会被外部检查弄得乱七八糟，在大多数地方只需要核心逻辑。

## High-Performance Execution – 高性能执行
Host functions are callable through a thin layer that doesn't incur heavy performance penalties.

主机函数可以通过一个浅层调用，不会导致严重的性能损失。

Because of the utilization of native paging mechanisms, almost all memory operations are very close to native if not at parity with native memory operations.

由于本地分页机制的使用，几乎所有内存操作都非常类似本地操作，哪怕不是本地内存操作相同。

Because of the high compaction and linear nature of the builtin allocators, this allows for a very cache friendly environment and further allows for high performance execution.

由于内置分配器的高度压缩和线性特性，这允许一个非常友好的缓存环境，并进一步允许高性能执行。

Certain design decisions were made to maximize the performance of interpreter implementation.  As mentioned above, __EOS VM__ has custom allocators and memory management that fits the needs and use cases for different access patterns and allocation requirements.  These allocators are used to back the core data types (fast vector, Wasm stack, fast variant, Wasm module), and as such do not "own" the memory that they use for their operations.  These non-owning data structures allow for the ability to use the memory cleanly and not have to concern the data type with destructing when going out of scope, which can increase the performance for certain areas of EOS VM without loss of generality for the developer.  Since the data is held by these allocators and have lifetimes that match that of a Wasm module, no copies of these heavyweight data types are ever needed.  Once an element in an EOS VM is constructed, that is its home and final resting place for the lifetime of the Wasm module.  

某些设计决策是为了最大限度地提高解释器实现的性能。如上所述，EOS VM拥有定制的分配器和内存管理，以满足不同访问模式和分配需求的需求和用例。这些分配器用于支持核心数据类型(fast vector、Wasm堆栈、fast variant、Wasm模块)，因此不“拥有”它们用于操作的内存。这些非拥有的数据结构允许干净地使用内存，而不必在超出作用域时考虑数据类型的销毁，这可以提高EOS VM某些区域的性能，而不会降低开发人员的通用性。由于数据由这些分配器持有，并且其生存期与Wasm模块的生存期相匹配，因此不需要这些重量级数据类型的副本。一旦构建了EOS VM中的一个元素，它就是Wasm模块生命周期中它的归属和最终安息之所。

A fast `variant` or discriminating union type is the fundamental data type that represents a Wasm opcode or a Wasm stack element.  This allows for a clean interface to "visit" each Wasm opcode without any loss of performance.  This visiting is statically derivable and not dynamically dispatched like more classical approaches that use the object-oriented visitor pattern.  In addition to a `visit` function that acts similar to `std::visit`, a custom dispatcher is defined that allows for a similar interface but with __EOS VM__ specific optimizations and assumptions.

快速变量或鉴别联合类型是表示Wasm操作码或Wasm堆栈元素的基本数据类型。这允许一个干净的接口“访问”每个Wasm操作码而不损失任何性能。这种访问是静态派生的，不像使用面向对象访问者模式的更经典的方法那样是动态分派的。除了一个与std::visit类似的访问函数之外，还定义了一个自定义调度程序，它允许类似的接口，但具有EOS VM特定的优化和假设。

## Effortless Integration - 轻松集成
With the exception of the softfloat library, which is an external dependency, __EOS VM__ is a header only implementation.

除了softfloat库(它是一个外部依赖)之外，EOS VM只是一个头文件实现。

Given the needs of the end user, integration can be as simple as pointing to the include directory.

考虑到最终用户的需求，集成可以简单地指向include目录。

__EOS-VM__ utilizes __CMake__ which allows integration into a project to be as little as adding `eos-vm` to the list of targets in the `target_link_libraries`.

EOS-VM利用了CMake，它允许集成到一个项目中，只需将EOS-VM添加到target_link_libraries的目标列表中。

If the need is only single-threaded a self-contained backend type is defined for the user to encapsulate all the components needed, which allows for source code integration to be constructing an instance of that type and adding "host functions" to the `registered_host_functions`.  Registering the host functions is as easy as calling a function with the function/member pointer and supplying the Wasm module name and function name.

如果需要的只是单线程的，则定义一个自包含的后端类型，供用户封装所需的所有组件，这允许源代码集成构造该类型的实例，并向registered_host_functions添加“主机函数”。注册主机函数就像使用函数/成员指针调用函数并提供Wasm模块名和函数名一样简单。

If multi-threaded execution is needed (i.e. multiple backends running at once), then the above integration is needed and the user will have to also construct thread specific watchdog timers and linear memory allocators.  These are also designed to be effortlessly registered to a particular Wasm backend.  

如果多线程执行是必需的(即多个后端同时运行)，那么上面的集成是必需的，用户还必须构造线程特定的看门狗计时器和线性内存分配器。它们也被设计成可以毫不费力地注册到特定的Wasm后端。

## Highly Extensible Design - 高度可扩展的设计
Given the __EOS-VM__ variant type and visitor system, new backends with custom logic can be easily defined and allows the same level of flexibility and code reuse as a much more heavyweight OOP __Visitor__ or __Listener__ design.

有了EOS-VM的变体类型和访问者系统，可以很容易地定义具有自定义逻辑的新后端，并允许与更重量级的OOP访问者或侦听器设计具有相同级别的灵活性和代码重用。

Since the design of __EOS-VM__ is component based, with each component being very self-contained, new backends or tools for Wasm can be crafted from previously defined components while only needing to define the logic for the extended functionality that is needed, with very little, to no, boilerplate needed.

由于EOS-VM的设计是基于组件的，每个组件都是非常自包含的，所以可以从以前定义的组件中制作新的Wasm后端或工具，而只需要为所需要的扩展功能定义逻辑，几乎不需要样板文件。

Extensions to Wasm itself can be made by simply defining the new section (aka C++ class field) for the module and the function needed to parse an element of that section.  This will allow for tooling to be constructed at a rapid pace for custom Wasms for a multitude of needs (debugging, profiling, etc.).

对Wasm本身的扩展可以通过简单地定义模块的新部分(又名c++类字段)和解析该部分元素所需的函数来实现。这将允许为满足多种需求(调试、分析等)的定制wasm快速构建工具。

## Using EOS-VM - 使用EOS-VM
[Quick Overview](./docs/OVERVIEW.md)


## Contributing

[Contributing Guide](./CONTRIBUTING.md)

[Code of Conduct](./CONTRIBUTING.md#conduct)

## Important – 重要

See [LICENSE](./LICENSE) for copyright and license terms.

All repositories and other materials are provided subject to the terms of this [IMPORTANT](./IMPORTANT.md) notice and you must familiarize yourself with its terms.  The notice contains important information, limitations and restrictions relating to our software, publications, trademarks, third-party resources, and forward-looking statements.  By accessing any of our repositories and other materials, you accept and agree to the terms of the notice.

版权声明请参阅许可证。block.one作为EOSIO社区成员的自愿贡献，不负责确保软件或任何相关应用程序的整体性能。我们不就本软件或任何相关文件作出任何明示或暗示的声明、保证、保证或承诺，包括但不限于对适销性、适合某一特定用途及不侵权的保证。在任何情况下，我们都不对任何索赔、损害赔偿或其他责任负责，无论是在合同、侵权或其他行为中，或与软件或文件或软件或文件的使用或其他交易有关的，或由软件或文件引起的或与之有关的。任何测试结果或性能数据都是指示性的，并不能反映在所有条件下的性能。对任何第三方或第三方产品、服务或其他资源的任何引用都不是Block.one的认可或推荐。对于您使用或依赖任何此等资源，我们概不负责，并拒绝承担任何及所有责任和责任。第三方资源可能会随时更新、更改或终止，所以这里的信息可能已经过期或不准确。任何使用或提供本软件与向第三方提供软件、商品或服务有关的人，应将本许可条款、免责声明和免责责任通知该第三方。块。1、EOSIO、EOSIO Labs、EOS、the seven - tahedron and associated logos均为Block.one的商标。
