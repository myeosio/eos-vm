## Building EOS-VM - 构建 EOS-VM
To build __EOS-VM__ you will need a fully C++17 compliant toolchain.<br>
为了构建EOS-VM，你将需要一个完全兼容c++ 17的工具链。

Since __EOS-VM__ is designed to be a header only library (with the exception of softfloat), building the __EOS-VM__ CMake project is not necessary to use __EOS-VM__ in a C++ project. But, if you would like to use the softfloat capabilities, then building the library is required.<br>
因为 __EOS-VM__ 被设计成一个只包含头文件的库(除了softfloat)，所以在c++项目中使用 __EOS-VM__ 并不需要构建 __EOS-VM__ 的CMake项目。但是，如果您想使用softfloat功能，那么就需要构建这个库。

## Using The Example Tools - 使用示例工具
Once you have built __EOS-VM__ you will notice 3 tools in the directory **build/tools**. You can run your test WASMs by executing the command `eos-vm-interp <path>/<wasm name>.wasm`, this will then run all exported functions within that WASM.  You can also run `bench-interp <path>/<wasm name>.wasm` and get two times in nanoseconds; the time to parse and instantiate your WASM and the time to execute your WASM.  The last tool is `hello-driver`. It is a prebaked in helloworld WASM and uses user input to bound the number of loops the printing occurs and whether it should assert. This tool is an example of how to setup a fully integrated solution with host functions.<br>
构建完 __EOS-VM__ 后，你会注意到 **build/tools** 目录下有3个工具。您可以通过执行命令 `eos-vm-interp <path>/<wasm name>.wasm` 来运行测试WASM，这将运行该wasm中的所有导出函数。你也可以运行 `bench-interp <path>/<wasm name>.wasm` 纳秒内得到两次解析和实例化WASM的时间，以及执行WASM的时间。最后一个工具是 `hello-driver`。它是helloworld WASM中预生成的，并使用用户输入来绑定打印循环的次数以及是否应该断言。这个工具是如何设置一个具有主机功能的完全集成的解决方案的示例。

These are designed to be modified by the end-user and are simply there to show how to easily integrate __EOS-VM__ into your own project.<br>
它们被设计成由最终用户修改，只是用来演示如何轻松地将EOS-VM集成到您自己的项目中。

## Integrating Into Existing CMake Project - 使用示例工具
Adding __EOS-VM__ as a submodule to your project and adding the subdirectory that contains __EOS-VM__, and adding **eos-vm** to the list of link libraries of your executables/libraries is all that is required to integrate into your project.  CMake options that can be passed into via command line or with CMake **set**.  These can be found in **CMakeLists.txt** and **modules/EosVMBuildUtils.cmake**, or by running `ccmake ..` instead of `cmake ..`.<br>
将 __EOS-VM__作为一个子模块添加到你的项目中，添加包含 __EOS-VM__ 的子目录，并将 **eos-vm** 添加到你的可执行文件/库的链接库列表中，这些都是集成到你的项目中所需要的。CMake选项可以通过命令行传递，也可以通过CMake **set** 传递。这些可以在 **CMakeLists.txt** 和 **modules/EosVMBuildUtils.cmake** 中找到。或者运行 `ccmake ..` 而不是 `cmake ..` 。

### Getting Started - 开始
 1) Start by creating a type alias of `eosio::vm::backend` with the host function class type.<br>
    首先创建一个带有主机函数类类型的 `eosio::vm::backend` 类型别名。<br>
    This class takes an additional optional template argument if wanted.  By default, i.e. left blank, this will create the interpreter `backend`.  If you set this to `eosio::vm::jit`, this will create the JIT based backend.<br>
    如果需要，这个类接受一个额外的可选模板参数。默认情况下，例如留空，这将创建解释器的 `backend`。如果设置为 `eosio::vm::jit`，将创建基于jit的后端。
 2) Next you can create a `watchdog` timer with a specific duration type, and setting the duration to the time interval you need, or use the predefined `null_watchdog` for unbounded execution.  <br>
    接下来，您可以创建一个具有特定持续时间类型的 `watchdog` 计时器，并将持续时间设置为您需要的时间间隔，或者使用预定义的 `null_watchdog` 进行无界执行
 3) You should now read the WASM file.  The `eosio::vm::backend` class has a method to read from a file or you can use a `std::vector<uint8_t>` that already contains the WASM.  This gets passed to the constructor of `eosio::vm::backend`.<br>
    现在应该读取WASM文件。 `eosio::vm::backend` 类有一个方法可以从文件中读取，或者你可以使用 `std::vector<uint8_t>` 它已经包含了WASM。这将被传递给 `eosio::vm::backend`的构造器。
 4) You should register and resolve any host functions, please see the **Adding Host Functions** section.<br>
    你应该注册和解析任何主机函数，请参阅**添加主机函数**部分。
 5) You can now construct your `backend` object by passing in the read WASM code and an instance of the `registered_host_functions` class which houses your imports.<br>
    现在，您可以通过传入读取的WASM代码和存放导入的 `registered_host_functions` 类的实例来构造您的`backend` 对象。
 6) Finally, you can execute a specific export via the `()` operator of `eosio::vm::backend`. This takes the host function instance reference, the module name, the export name and typesafe export arguments. (see **/tools/interp.cpp** and **/tools/hello_driver.cpp** for more details)<br>
    最后，你可以通过 `eosio::vm::backend`的 `()` 操作符来执行特定的导出。它接受宿主函数实例引用、模块名称、导出名称和类型安全导出参数。(见 **/tools/interp.cpp** 和 **/tools/hello_driver.cpp** 了解更多详情)

### Adding Host Functions - 添加主机的功能
Without any host functions, your WASM execution is going to be very limited (you will not be able to observe side effects or get intermediate values).  <br>
如果没有任何宿主函数，WASM的执行将非常有限(您将无法观察副作用或获得中间值)。 

There are three options currently for creating "host functions":<br>
目前有三个选项可以创建“主机函数”:

   1) C-style function - C-风格的功能
   2) static method of a class - 类的静态方法
   3) proper method of a class - 类的正确方法

Currently, you are limited to only one type of host class to encapsulate your host functions, this limitation will be removed in a future release.<br>
目前，您只能使用一种类型的主机类来封装您的主机函数，这个限制将在未来的版本中删除。

#### Registered Host Functions Class -  注册主机函数类
Firstly, you should create a type alias to limit the redundant typing needed.<br>
首先，您应该创建一个类型别名来限制所需的冗余类型。

   - `using rhf_t = eosio::vm::registered_host_functions<ClassType>;`, where **ClassType** is either `nullptr_t` if you are only using C-style functions, or the type of the host class.<br>
   - `using rhf_t = eosio::vm::registered_host_functions<ClassType>;`, 其中 **ClassType** 是 `nullptr_t` 如果你只使用c风格的函数，或主机类的类型。
Then, you will `add` your host functions via `rhf_t::add<ClassType, &function_name, wasm_allocator>("module name", "wasm function name");` for each host function.<br>
然后，你将通过 `rhf_t::add<ClassType, &function_name, wasm_allocator>("module name", "wasm function name");` 为每个主机函数 `add` 你的主机函数。

The last thing to do is to resolve the imports of your `module`, this is handled by `rhf_t::resolve( <reference to module> );`.<br>
最后一件要做的事情是解析 `module`的导入，这由 `rhf_t::resolve( <reference to module> );`处理。

For a more concise example of this, please see **/tools/hello_driver.cpp**.<br>
一个更简洁的例子，请参阅 **/tools/hello_driver.cpp** 。
