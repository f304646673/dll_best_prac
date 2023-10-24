# DllMain的相关特性

首先列出[《DllMain中不当操作导致死锁问题的分析--进程对DllMain函数的调用规律的研究和分析》](https://blog.csdn.net/breaksoftware/article/details/8142339)中论证的11个特性：

- Dll的加载不会导致之前创建的线程调用其DllMain函数。
- 线程创建后会调用已经加载了的DLL的DllMain，且调用原因是DLL_THREAD_ATTACH。(DisableThreadLibraryCalls会导致该过程不被调用)
- TerminateThread方式终止线程是不会让该线程去调用该进程中加载的Dll的DllMain。
- 线程正常退出时，会调用进程中还没卸载的DLL的DllMain，且调用原因是DLL_THREAD_DETACH。
- 进程正常退出时，会调用（不一定是主线程）该进程中还没卸载的DLL的DllMain，且调用原因是DLL_PROCESS_DETACH。
- 加载DLL进入进程空间时(和哪个线程LoadLibrary无关)，加载它的线程会调用DllMain，且调用原因是DLL_PROCESS_ATTACH。
- DLL从进程空间中卸载出去前，会被卸载其的线程调用其DllMain，且调用原因是DLL_PROCESS_DETACH。
- TerminateProcess 将导致线程和进程在退出时不对未卸载的DLL进行DllMain调用。
- ExitProcess将导致主线程意外退出，子线程对未卸载的DLL进行了DllMain调用，且调用原因是DLL_PROCESS_DETACH。
- ExitThread是最和平的退出方式，它会让线程退出前对未卸载的DLL调用DllMain。
- 线程的创建和退出不会对调用了DisableThreadLibraryCalls的DLL调用DllMain。

# 不要在DllMain中做的事情

- 直接或者间接调用LoadLibrary(Ex)。
  假如我们在A.dll中的DllMain收到DLL_PROCESS_ATTACH时，加载了B.dll；而B.dll中的DllMain在收到DLL_PROCESS_ATTACH时又去加载A.dll。则产生了循环依赖。但是注意不要想当然认为这个过程是A.dll的DllMain调用了B.dll的DllMain，B.dll的DllMain再调用了A.dll的DllMain这样的死循环。即使不出现循环依赖，如果出现[《DllMain中不当操作导致死锁问题的分析——线程中调用GetModuleFileName、GetModuleHandle等导致死锁》](https://blog.csdn.net/breaksoftware/article/details/8167614)中第三个例子的情况，也会死锁的。

- 使用CoInitializeEx。
  在CoInitializeEx底层会调用LoadLibraryEx，原因同A。

- 使用CreateProcess。
  CreateProcess在底层执行了加载DLL的操作。我用IDA查看Kernel32中的CreateProcess可以发现其底层调用的CreateProcessInternalW中有

- 使用User32或Gdi32中的函数。
  User32和Gdi32中部分函数在调用的底层会加载其他DLL。

- 使用托管代码。
  运行托管代码需要加载其他DLL。

- 与其他线程同步执行。
  由[《DllMain中不当操作导致死锁问题的分析--加载卸载DLL与DllMain死锁的关系》](https://blog.csdn.net/breaksoftware/article/details/8163362)、[《DllMain中不当操作导致死锁问题的分析--导致DllMain中死锁的关键隐藏因子》](https://blog.csdn.net/breaksoftware/article/details/8150476)和[《DllMain中不当操作导致死锁问题的分析--线程退出时产生了死锁》](https://blog.csdn.net/breaksoftware/article/details/8163663)可知，进程创建和销毁以及DLL的加载都要进入PEB的LoadLock临界区。如果占用了LoaderLock临界区的线程在等待一个需要经过临界区才能结束的线程时，就发生了死锁。以上3篇博文中均有案例。

- 同步对象。
  如果该同步对象的释放需要获得PEB中的LoaderLock，而占用该临界区的线程又要去等待这个同步对象，则会死锁。其实F中的线程也算是个同步对象。案例详见[《DllMain中不当操作导致死锁问题的分析——线程中调用GetModuleFileName、GetModuleHandle等导致死锁》](https://blog.csdn.net/breaksoftware/article/details/8167614)中例子。

- 使用CreateThread。
  理由同F。

- 使用ExitThread。
  理由同F。

- 寄希望于DisableThreadLibraryCalls解决死锁问题。
  由[《DllMain中不当操作导致死锁问题的分析--DisableThreadLibraryCalls对DllMain中死锁的影响》](https://blog.csdn.net/breaksoftware/article/details/8159088)可知。DisableThreadLibraryCalls的实现逻辑是：找到PEB结构中用于保存加载器信息的结构体对象Ldr。
        
![image](https://github.com/f304646673/dll_best_prac/assets/5725174/384f88be-914e-4bf0-9a29-00202cc2676b)
        
Ldr对象的InMemoryOrderModuleList用户保存已经加载的DLL的链表。
        
![image](https://github.com/f304646673/dll_best_prac/assets/5725174/8443a5bd-0a1b-4540-a804-fd4346a79b40)
       
它遍历这个链表，找到调用DisableThreadLibraryCalls的DLL的信息，将该信息中的Flags字段设置或上0x40000。
       
![image](https://github.com/f304646673/dll_best_prac/assets/5725174/7f77182b-7c6e-446c-9453-78a78074d4c9)
        
而创建线程在底层将调用LdrpInitializeThread(详见[《DllMain中不当操作导致死锁问题的分析--DisableThreadLibraryCalls对DllMain中死锁的影响》](https://blog.csdn.net/breaksoftware/article/details/8159088))。该函数一开始便进入了PEB中LoaderLock临界区，在该临界区中根据PEB中LDR的InMemoryOrderModuleList遍历加载的DLL，然后判断该DLL信息的Flags字段是否或上了0x40000。如果或上了，就不调用DllMain。如果没或上，就调用DllMain。这说明DisableThreadLibraryCalls对创建线程时是否进入临界区无关。
在退出线程时底层将调用LdrShutdownThread（详见[《DllMain中不当操作导致死锁问题的分析--线程退出时产生了死锁》](https://blog.csdn.net/breaksoftware/article/details/8163663)）。该函数逻辑和LdrpInitializeThread相似，只是在调用DllMain时传的是DLL_THREAD_DETACH。所以DisableThreadLibraryCalls对LdrShutdownThread是否进入临界区也是没有影响的。
最后附上实验中的例子和《Best Practices for Creating DLLs》
