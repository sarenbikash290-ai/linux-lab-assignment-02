# Question 2: Process Creation, Monitoring, and Zombie Prevention

## Objective
Design a C program that creates child processes using fork(), monitors 
their execution, prevents zombie processes, and terminates unresponsive 
children using signals.

## Program
See `process_manager.c` in this folder.

## Steps, Commands, and Explanations

### Step 1: Create working folder

mkdir Q2
cd Q2

**Explanation:** Created a dedicated folder for Question 2 to keep files organized.

### Step 2: Write the C program

nano process_manager.c

**Explanation:** Implemented a program that forks multiple child processes, 
uses a SIGCHLD handler with waitpid() to reap terminated children immediately, 
and sends SIGTERM/SIGKILL to any child still running after a timeout.

### Step 3: Compile the program

gcc process_manager.c -o process_manager

**Explanation:** Compiled the C source into an executable using gcc; no errors 
means the syntax and included headers were correct.

### Step 4: Run the program

./process_manager

**Explanation:** The parent process created 3 children. Two children completed 
normal work and exited on their own; one child was made to simulate an 
unresponsive process using an infinite loop. After a 5-second monitoring 
window, the parent detected the hung child was still alive and sent it 
SIGTERM (and would send SIGKILL if SIGTERM was ignored).

### Step 5: Check running processes during execution

ps -ef | grep process_manager

**Explanation:** Confirmed that all child processes were visible as separate 
PIDs under the parent, including the child stuck in the infinite loop.

### Step 6: Confirm no zombie processes remained

ps aux | grep defunct

**Explanation:** No defunct/zombie entries were found, confirming the SIGCHLD 
handler successfully reaped every terminated child using waitpid().

## Explanation: How fork(), wait(), and signal handling work together

- **fork()** creates a child process that is a near-exact copy of the parent. 
  Each call returns twice: 0 in the child, and the child's PID in the parent. 
  This lets the parent track and manage each child individually via its PID.

- **Zombie processes** occur when a child terminates but its exit status hasn't 
  been read by the parent yet — the OS keeps a minimal entry in the process 
  table (marked `<defunct>`) until the parent calls wait()/waitpid(). If the 
  parent never does this, zombies accumulate and can exhaust the process table, 
  which is exactly what causes a server to become unresponsive under load.

- **SIGCHLD** is automatically sent to a parent whenever a child terminates. 
  By registering a handler for SIGCHLD that calls waitpid(-1, &status, WNOHANG) 
  in a loop, the parent reaps every terminated child asynchronously and 
  immediately, without needing to poll or block — this prevents zombies 
  entirely, even under heavy child process load.

- **kill(pid, 0)** is used (with signal 0, meaning "no signal sent") purely to 
  test whether a process is still alive, without disturbing it.

- **SIGTERM** is sent first to unresponsive children, giving them a chance to 
  shut down gracefully (a well-behaved process can catch SIGTERM and clean up). 

- **SIGKILL** is sent only if SIGTERM is ignored, since SIGKILL cannot be caught, 
  blocked, or ignored by the target process — it forces immediate termination, 
  guaranteeing the unresponsive process is removed even if it's misbehaving.

Together, these mechanisms let a server-style program create many workers, 
avoid resource leaks from zombie processes, and forcibly recover from any 
worker that hangs — directly solving the scenario of a web server becoming 
unresponsive due to excessive/stuck child processes.
