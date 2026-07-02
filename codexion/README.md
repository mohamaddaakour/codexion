# **Definition**

- A project in 42 core curriculum done by mdaakour, the main idea of this project is to build an application using multithreading (pthread) to process multiple tasks in the same time.

- In this project we have many coders want to compile specific amount of compiles for each coder before burn out, and they can't compile until they have two dongles left and right.

- Number of coders = number of dongles.

- Per example: here we have many coders that works in the same time, so each one of them is a thread.

- last_compile_start is to track the time when the coder last time compiled to check if he will burn out.

- If the required_compiles equals -1 this means the program will still working until all coders finish its compiles (we don't have burnout).

- If stop is 1 we have to stop the program, if 0 the program will continue.

- If all_done = 1 means all coders are done compiling.

- If required_compiles = -1 the coders will compile infinetly until someone burns out.

# **Instructions**

```shell
# make commands

# to generate the executable file and the .o files
make

# to delete the executable file and the .o files and regcompile them again
make re

# to delete the executable file and the .o files
make fclean

# to delete the .o files only
make clean

# to run the code
./codexion 3 200 200 200 200 3
```

# **Blocking Cases Handled**

- To prevent the classic deadlock scenario where every coder holds their left resource while waiting indefinitely for their right, our solution intentionally breaks Coffman's Circular Wait condition.

- The Mechanism: Instead of picking up resources based on relative position (left then right), threads evaluate the absolute memory addresses of both target t_dongle pointers.

- The Rule: Every thread is forced to acquire the lower-addressed dongle first (first = (left < right) ? left : right) before attempting to lock the second. Because resource acquisition follows a strict, global total ordering, a circular wait dependency chain is mathematically impossible.

- Detecting thread death (burnout) without introduces latencies or high CPU overhead requires tight decoupling between worker threads and the dedicated monitor thread.

# **Thread Synchronization Mechanisms**

- Mutexes act as strict binary locks to protect structural parameters.

- Condition variables are utilized to transition threads into a deep, zero-overhead sleep state when prerequisites are unfulfilled, rather than relying on wasteful busy-waiting loops.

# **Resources**

- GeeksforGeeks.
- Gemini AI to learn the new multithreading concpets.

# **Author**

- Mohamad Daakour
- 42 intra: mdaakour
