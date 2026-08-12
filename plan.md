# Codexion — Implementation Plan

> Goal: implement the mandatory Codexion project from the 42 subject as a clean, understandable C/pthreads program that can be defended line-by-line during peer evaluation.

## Project Constraints

The implementation must follow the subject exactly:

- Language: C.
- Compile with `cc -Wall -Wextra -Werror -pthread`.
- No global variables.
- No libft.
- Every coder is represented by a pthread.
- There is one dongle between every pair of coders.
- A coder normally needs two dongles simultaneously to compile.
- A single-coder simulation has exactly one dongle.
- Every dongle has its own mutex.
- Dongle cooldown is mandatory.
- Access to a contested dongle must use the selected scheduler: `fifo` or `edf`.
- A custom priority queue/heap must be implemented for scheduling.
- A separate monitor thread must detect burnout.
- Logging must be serialized.
- Simulation ends when either one coder burns out or every coder reaches `number_of_compiles_required`.
- All heap memory must be freed.
- No unexpected termination, leaks, deadlocks, or data races.

Command-line arguments:

```text
./codexion number_of_coders time_to_burnout time_to_compile \
           time_to_debug time_to_refactor number_of_compiles_required \
           dongle_cooldown scheduler
```

Valid scheduler values are exactly `fifo` and `edf`.

---

# M1 — Repository and Build Skeleton

## Objective

Create the smallest possible project that has the correct directory structure, Makefile, headers, source files, and executable name. Do not implement concurrency yet.

## Directory structure

Create:

```text
codexion/
├── Makefile
├── README.md
└── coders/
    ├── include/
    │   ├── codexion.h
    │   ├── types.h
    │   ├── parser.h
    │   ├── simulation.h
    │   ├── coder.h
    │   ├── dongle.h
    │   ├── scheduler.h
    │   ├── heap.h
    │   ├── monitor.h
    │   └── logger.h
    └── src/
        ├── main.c
        ├── parser.c
        ├── simulation.c
        ├── coder.c
        ├── dongle.c
        ├── scheduler.c
        ├── heap.c
        ├── monitor.c
        ├── logger.c
        └── time.c
```

Adjust the exact structure if desired, but keep responsibilities separated.

## Makefile requirements

Implement:

- `NAME = codexion`
- `all`
- `clean`
- `fclean`
- `re`

Compile with:

```text
-Wall -Wextra -Werror -pthread
```

Do not relink files unnecessarily.

## Initial program

For now, `main()` should simply verify that the project builds and can execute.

Do not start implementing the actual simulation yet.

## Done when

- `make` produces `codexion`.
- `make clean` removes objects.
- `make fclean` removes objects and executable.
- `make re` rebuilds everything.
- No compiler warnings.
- Norm is clean.
- There are no global variables.

---

# M2 — Data Model and Argument Parsing

## Objective

Define all state required by the simulation and implement strict command-line validation.

The important rule is: **get the data model correct before writing thread code.**

## Configuration structure

Create a configuration structure containing at least:

```c
number_of_coders
time_to_burnout
time_to_compile
time_to_debug
time_to_refactor
number_of_compiles_required
dongle_cooldown
scheduler
```

Represent the scheduler using an enum, for example:

```text
SCHED_FIFO
SCHED_EDF
```

Do not store the scheduler as an arbitrary string throughout the program.

## Coder structure

Each coder should have at least:

```text
id
thread
left_dongle
right_dongle
compile_count
last_compile_start
state
simulation reference
```

The exact fields can evolve, but every piece of state should have a clear owner.

Possible states:

```text
WAITING
COMPILING
DEBUGGING
REFACTORING
DONE
BURNED_OUT
```

## Dongle structure

Each dongle should contain at least:

```text
id
mutex
available_at
scheduler/request state
```

`available_at` represents the earliest time at which the dongle can be acquired after cooldown.

## Simulation structure

Create one central structure containing:

```text
configuration
coder array
dongle array
start timestamp
stop flag
stop reason
monitor thread
logging mutex
shared synchronization state
```

Avoid global state. Pass the simulation through thread arguments/references.

## Argument validation

Reject:

- Wrong argument count.
- Missing arguments.
- Negative numbers.
- Non-integer values.
- Zero where the subject makes the configuration invalid.
- Scheduler values other than `fifo` or `edf`.
- Integer overflow while parsing.

Use a dedicated parser instead of scattering `atoi()` calls throughout the program.

## Done when

Test cases include:

```text
./codexion
./codexion 4
./codexion -1 800 200 200 200 3 0 fifo
./codexion 4 800 200 200 200 3 0 random
./codexion abc 800 200 200 200 3 0 fifo
```

All invalid configurations are rejected cleanly.

---

# M3 — Time and Thread-Safe Logging Infrastructure

## Objective

Build the timing and logging foundation before implementing coder behaviour.

## Timestamp helper

Implement a helper based on `gettimeofday()`.

Required operations:

```text
get_current_time_ms()
get_elapsed_time_ms(simulation)
```

The timestamp printed by the program should be milliseconds since the beginning of the simulation.

Do not repeatedly duplicate timestamp arithmetic in every thread.

## Logging

Implement a single logging function that:

1. Gets the current elapsed timestamp.
2. Locks the logging mutex.
3. Writes one complete message.
4. Unlocks the logging mutex.

Required messages:

```text
X has taken a dongle
X is compiling
X is debugging
X is refactoring
X burned out
```

The output format must be exactly:

```text
timestamp X message
```

## Logging rules

Never call `printf()` directly from random parts of the program once the logger exists.

Every state transition should go through the logger.

The logger must ensure two threads cannot interleave their output.

## Done when

Create a temporary multi-threaded logging test if necessary and verify that every line is complete and readable.

---

# M4 — Dongle Topology and Basic Resource Ownership

## Objective

Implement the physical model of the dongles independently from scheduling.

## Topology

For `N > 1`:

- Create exactly `N` dongles.
- Coder `1` has coder `N` on its left and coder `2` on its right.
- Coder `N` wraps around to coder `1`.

For coder `i`:

```text
left  = dongle[(i - 1 + N) % N]
right = dongle[i]
```

Use whatever indexing convention your implementation chooses, but preserve the circular topology.

For `N == 1`:

- Create exactly one dongle.
- The coder must not attempt to acquire two separate dongles.
- The single dongle is the only resource available.

## Mutex ownership

Every dongle has its own mutex.

The mutex protects the dongle's state, including availability/cooldown and scheduler-related state where applicable.

Do not hold unrelated mutexes longer than necessary.

## Basic acquire/release helpers

Create functions conceptually equivalent to:

```text
dongle_init()
dongle_destroy()
dongle_can_be_acquired()
dongle_release()
```

Do not implement the final FIFO/EDF arbitration yet. This milestone is about the resource model.

## Done when

You can initialize and destroy simulations with:

```text
1 coder
2 coders
3 coders
4 coders
10 coders
```

without leaks or invalid pointer relationships.

---

# M5 — Heap / Priority Queue Implementation

## Objective

Implement the custom priority queue required by the subject before connecting it to real threads.

The subject explicitly requires a heap for FIFO/EDF scheduling.

## Heap requirements

Implement a reusable binary heap with operations equivalent to:

```text
heap_init()
heap_push()
heap_pop()
heap_peek()
heap_empty()
heap_destroy()
```

The heap stores scheduling requests.

A request should contain enough information to determine:

```text
coder id
request sequence number
request timestamp
deadline
```

## FIFO ordering

For FIFO, compare requests using arrival order.

The easiest deterministic method is a monotonically increasing request sequence number.

Earlier sequence number = higher priority.

## EDF ordering

For EDF, priority is:

```text
last_compile_start + time_to_burnout
```

Earlier deadline = higher priority.

If two deadlines are equal, use a deterministic tie-breaker such as request sequence number.

## Important

Do not use the standard library to fake the priority queue. Implement the heap yourself.

## Testing

Write a small heap test that verifies:

- Empty heap.
- One element.
- Multiple elements.
- FIFO ordering.
- EDF ordering.
- Equal deadlines.
- Push/pop repeatedly.
- Destroying a non-empty heap if your API allows it.

## Done when

The heap can be tested independently from pthreads and always returns requests in the expected order.

---

# M6 — Coder Lifecycle Without Real Contention

## Objective

Implement the basic coder state machine and thread lifecycle before introducing complex dongle arbitration.

Each coder must have its own pthread created with `pthread_create()`.

## Coder lifecycle

The intended lifecycle is:

```text
WAITING
   ↓
acquire two dongles
   ↓
COMPILING
   ↓
release both dongles
   ↓
DEBUGGING
   ↓
REFACTORING
   ↓
WAITING
```

Repeat until:

```text
compile_count >= number_of_compiles_required
```

or the simulation stops because of burnout.

## Timing

During compiling:

```text
sleep(time_to_compile)
```

Then debugging:

```text
sleep(time_to_debug)
```

Then refactoring:

```text
sleep(time_to_refactor)
```

Use a helper for sleeping in milliseconds rather than duplicating conversions.

## Compile count

Increment the coder's compile count only when a compile actually starts.

The compile start timestamp must also be updated at the moment the coder begins compiling.

This timestamp is later used for burnout detection and EDF scheduling.

## Completion condition

After a coder completes its required number of compiles, it should stop participating in future compilation attempts.

The simulation should eventually stop when all coders satisfy the requirement.

## Done when

Run a simple configuration where resources are effectively uncontended and verify the state sequence and compile counts.

---

# M7 — Real Dongle Acquisition, Cooldown, and Deadlock-Free Coordination

## Objective

Connect coder threads to real dongles and make resource acquisition safe.

This is the first major concurrency milestone.

## Required behaviour

A coder must acquire both required dongles before printing:

```text
X is compiling
```

The expected order is:

```text
X has taken a dongle
X has taken a dongle
X is compiling
```

Do not allow a coder to start compiling while holding only one dongle.

## Cooldown

When a dongle is released at time `T`, it cannot be acquired again until:

```text
T + dongle_cooldown
```

Store the cooldown deadline in the dongle state.

Cooldown applies every time a dongle is released.

## Avoiding deadlock

The naïve approach of every coder locking one dongle and then waiting for the other can create circular wait.

Do not implement that naïve approach.

Use a centralized arbitration/acquisition mechanism that can decide which coder receives the resources.

The scheduler must be able to reason about both dongles together so that a coder is not permanently stuck holding one dongle while waiting for another.

## Condition variables

Use `pthread_cond_t` where waiting is appropriate instead of continuously spinning.

A waiting coder should sleep until something relevant changes, such as:

- A dongle becomes available.
- A cooldown expires.
- A request with higher priority is served.
- The simulation stops.

## Stop checks

Every blocking operation must have a way to notice that the simulation has ended.

A thread must not remain asleep forever after another thread has requested shutdown.

## Done when

Run multiple coders for a significant number of compile cycles and verify:

- No deadlocks.
- No duplicate dongle ownership.
- No compilation with one dongle.
- Cooldown is respected.
- The program eventually makes progress.

---

# M8 — FIFO and EDF Arbitration

## Objective

Implement the actual scheduling policy required by the subject.

At this point, dongle acquisition should become a proper scheduling problem rather than simple mutex locking.

## Request creation

When a coder cannot immediately obtain the required resources, create a scheduling request containing:

```text
coder
request sequence
request timestamp
deadline
```

The request deadline for EDF is:

```text
last_compile_start + time_to_burnout
```

## FIFO

FIFO means:

> The dongle is granted to the coder whose request arrived first.

Therefore:

```text
earliest request sequence number wins
```

Do not use coder ID as the FIFO order.

Coder 4 requesting before coder 2 means coder 4 has priority.

## EDF

EDF means:

> The coder with the earliest burnout deadline receives priority.

Therefore:

```text
earliest(last_compile_start + time_to_burnout) wins
```

Use the heap implemented in M5.

## Fairness / liveness

Under EDF, a coder must not be permanently starved when the parameters are feasible.

Every scheduling decision must eventually allow waiting requests to progress.

## Resource acquisition design

Centralize scheduling decisions so that the system can answer:

```text
Which coder is allowed to acquire this dongle?
```

and, where necessary:

```text
Can this coder obtain both required dongles now?
```

Do not duplicate arbitration logic inside each coder thread.

## Done when

Create scenarios with several coders competing heavily for resources and verify that:

- FIFO respects request arrival order.
- EDF respects burnout deadlines.
- Cooldown is still respected.
- No coder permanently starves.
- No two coders can own the same dongle simultaneously.

---

# M9 — Burnout Monitor and Precise Simulation Shutdown

## Objective

Implement the dedicated monitor thread required by the subject.

The monitor is responsible for detecting burnout and stopping the simulation.

## Monitor thread

Create a separate pthread using `pthread_create()`.

The monitor repeatedly checks all active coders.

A coder burns out if it has not started compiling within:

```text
time_to_burnout
```

since either:

- the beginning of the simulation, or
- the beginning of its previous compile.

## Important distinction

A coder that is currently compiling is not considered burned out merely because its compile takes a long time.

The deadline concerns the time until the coder **starts its next compile**.

Therefore track:

```text
last_compile_start
```

for every coder.

## Burnout check

Conceptually:

```text
now - last_compile_start >= time_to_burnout
```

If the coder has not reached the required compile count and misses its deadline:

1. Mark it burned out.
2. Print the burnout message exactly once.
3. Set the global simulation stop state.
4. Wake all waiting threads.

## Precision

The subject requires the burnout message to appear no more than approximately 10 ms after the actual burnout time.

Do not use a monitor sleep interval that is obviously larger than the allowed tolerance.

Use a short condition-variable timed wait or another efficient approach that wakes near the next relevant deadline.

Avoid a busy loop that consumes an entire CPU core.

## Successful completion

The simulation must also stop when every coder has completed:

```text
number_of_compiles_required
```

The monitor or central simulation state should detect this condition and request shutdown.

## Shutdown signalling

When stopping:

- Set the stop flag under the appropriate mutex.
- Broadcast to condition variables.
- Make sleeping/waiting acquisition threads return promptly.
- Ensure no new compile begins after shutdown has been established.

## Done when

Test both termination paths:

1. All coders complete the required number of compiles.
2. A coder burns out.

Verify that the program always terminates cleanly.

---

# M10 — Full Resource Lifecycle and Cleanup

## Objective

Make every allocation, mutex, condition variable, thread, heap, and resource have a clear initialization and destruction path.

This milestone is primarily about reliability.

## Initialization order

Use a predictable order such as:

```text
parse arguments
↓
allocate simulation
↓
initialize simulation mutexes/conditions
↓
initialize dongles
↓
initialize coders
↓
initialize scheduler/heaps
↓
start coder threads
↓
start monitor thread
```

## Failure cleanup

Every initialization step can fail.

If initialization fails halfway through, destroy only the objects that were successfully initialized.

Do not blindly destroy uninitialized mutexes or condition variables.

Track initialization state where necessary.

## Normal shutdown

After the simulation ends:

```text
signal stop
↓
join monitor
↓
join every coder
↓
destroy condition variables
↓
destroy mutexes
↓
destroy heaps
↓
free coder array
↓
free dongle array
↓
free simulation
```

The exact order may differ according to ownership, but no object may be destroyed while another thread is still using it.

## Memory checks

Run the program under a memory checker such as Valgrind if available.

Check:

- Invalid reads/writes.
- Use-after-free.
- Double free.
- Definitely lost memory.
- Possibly lost memory.
- Uninitialized mutex usage.

## Done when

Repeatedly start and stop simulations with different configurations without crashes or leaks.

---

# M11 — Concurrency Stress Testing

## Objective

Try to break the implementation deliberately.

Do not add new features here. Find races, deadlocks, starvation, timing problems, and shutdown bugs.

## Test categories

### Very small coder counts

```text
1 coder
2 coders
3 coders
```

Pay particular attention to the single-coder special case.

### Many coders

Test values such as:

```text
5
10
20
50
100
```

subject to the limits of the machine.

### Very small timing values

Use small compile/debug/refactor values to create heavy contention.

### Large timing values

Verify that long sleeps do not cause premature burnout.

### Cooldown

Test:

```text
dongle_cooldown = 0
```

and cooldown values larger than compile/debug/refactor times.

### FIFO

Create many simultaneous requests and inspect whether request arrival order is respected.

### EDF

Create situations where coders have different deadlines and verify the earliest deadline gets priority.

### Burnout

Intentionally create infeasible configurations and verify that the monitor detects burnout quickly.

### Successful completion

Use a small required compile count and verify the simulation stops only after every coder reaches it.

## Stress loop

Run the same simulation many times:

```text
for many iterations:
    start simulation
    verify exit status
    inspect output
```

Timing bugs often appear only after repeated executions.

## Race detection

If available, use ThreadSanitizer or another race detector during development, while remembering that the final build must satisfy the project's required compiler/toolchain constraints.

## Done when

You can run dozens/hundreds of simulations without:

- deadlocks
- crashes
- data races
- duplicate dongle ownership
- impossible state transitions
- missing shutdown
- leaked memory

---

# M12 — Output Validation and Subject Compliance

## Objective

Compare the implementation directly against every mandatory requirement in the subject.

Create a compliance checklist and verify every item manually.

## Logging checklist

Every state change must use the required format:

```text
X has taken a dongle
X is compiling
X is debugging
X is refactoring
X burned out
```

Check that:

- Timestamps are milliseconds.
- Timestamps are relative to simulation start.
- Messages never interleave.
- A coder cannot say `is compiling` before two dongle acquisitions.
- Burnout is printed only once.
- Burnout output is within the required timing tolerance.

## Simulation checklist

Verify:

- Every coder is a pthread.
- Number of dongles is correct.
- Single-coder case uses one dongle.
- Two dongles are required for normal compilation.
- Dongle cooldown is enforced.
- FIFO is implemented.
- EDF is implemented.
- EDF uses burnout deadlines.
- A custom heap is used.
- A separate monitor thread exists.
- Simulation stops on burnout.
- Simulation stops after all required compiles.
- No global variables exist.
- All memory is freed.

## Build checklist

Run:

```text
make
make clean
make
make fclean
make re
```

Compile with:

```text
-Wall -Wextra -Werror -pthread
```

## Norm

Run the required Norm checker over all submitted C/header files.

Do not leave helper/test/bonus files in the submission if they are not allowed or are not norm-compliant.

## Done when

Every requirement from the subject can be pointed to in the source code and explained.

---

# M13 — README and Defence Preparation

## Objective

Prepare the project for peer evaluation and make sure you understand every concurrency decision.

## README.md

The README must be in English and contain at least:

1. Required 42 first line:

```text
*This project has been created as part of the 42 curriculum by <login>.*
```

Use the exact required wording/format from the subject and replace the login appropriately.

2. `Description`
3. `Instructions`
4. `Resources`
5. `Blocking cases handled`
6. `Thread synchronization mechanisms`

Explain:

- Deadlock prevention.
- Coffman's conditions and how your design avoids deadlock.
- Starvation prevention.
- FIFO scheduling.
- EDF scheduling.
- Dongle cooldown.
- Burnout detection.
- Log serialization.
- Mutex usage.
- Condition variables.
- Monitor communication.
- Custom heap.

Also document how AI was used, as explicitly required by the subject.

## Defence questions to prepare for

You should be able to explain without looking at the code:

### Threads

- Why does each coder need a thread?
- Why is the monitor a separate thread?
- What happens if a coder thread exits early?

### Mutexes

- What does each dongle mutex protect?
- Why can't two coders acquire the same dongle?
- What does the logging mutex protect?

### Condition variables

- Why use a condition variable instead of busy waiting?
- What causes a waiting coder to wake up?
- Why must the condition always be checked again after waking?

### Deadlocks

- What is circular wait?
- Why can naïve left-then-right locking deadlock?
- How does your arbitration design prevent it?

### Scheduling

- How is FIFO implemented?
- How is EDF implemented?
- Why is a heap needed?
- What is the EDF deadline?

### Burnout

- What exactly defines burnout?
- Why is `last_compile_start` needed?
- Why does the monitor need to run independently?
- How do you guarantee the burnout message is printed quickly enough?

### Cooldown

- When does cooldown begin?
- Can a coder immediately reacquire a released dongle?
- How does the scheduler account for cooldown?

### Shutdown

- What happens when one coder burns out?
- How are sleeping threads awakened?
- Why must all threads be joined before destroying synchronization primitives?

### Memory

- Who owns each allocation?
- Who frees it?
- What happens if initialization fails halfway through?

## Recode preparation

Practice small changes yourself, because the evaluation may request a modification during the defence.

Examples to practice:

- Add a field to the coder structure.
- Change a log message.
- Change a scheduler comparison rule.
- Add a small validation rule.
- Change how a heap tie is broken.
- Modify a timing helper.

The goal is not merely to have a working program. You must be able to modify and explain it yourself.

---

# Recommended Implementation Order

Follow the milestones strictly rather than jumping directly into the final concurrent implementation.

```text
M1  Build skeleton
 ↓
M2  Data structures + parsing
 ↓
M3  Timing + logging
 ↓
M4  Dongle topology
 ↓
M5  Heap
 ↓
M6  Coder lifecycle
 ↓
M7  Resource acquisition + cooldown
 ↓
M8  FIFO + EDF
 ↓
M9  Burnout monitor
 ↓
M10 Cleanup
 ↓
M11 Stress tests
 ↓
M12 Subject compliance
 ↓
M13 README + defence
```

Do not move to the next milestone until the current milestone can be tested independently.

---

# Final Definition of Done

The project is finished only when all of the following are true:

- [ ] `make` succeeds with no warnings.
- [ ] Norm passes.
- [ ] No global variables are used.
- [ ] All required command-line arguments are validated.
- [ ] All coders are pthreads.
- [ ] Dongle topology is correct.
- [ ] Single-coder case is correct.
- [ ] Every dongle is protected by a mutex.
- [ ] Two dongles are acquired before compilation.
- [ ] Dongle cooldown is enforced.
- [ ] FIFO scheduling works.
- [ ] EDF scheduling works.
- [ ] EDF uses `last_compile_start + time_to_burnout`.
- [ ] Custom priority queue/heap is implemented and used.
- [ ] Waiting coders do not deadlock.
- [ ] Waiting coders do not starve under feasible EDF configurations.
- [ ] Separate monitor thread detects burnout.
- [ ] Burnout output is within the required tolerance.
- [ ] Output lines are serialized.
- [ ] Simulation stops correctly on burnout.
- [ ] Simulation stops correctly when all coders finish.
- [ ] All threads are joined.
- [ ] All mutexes and condition variables are destroyed.
- [ ] All allocated memory is freed.
- [ ] Stress tests pass repeatedly.
- [ ] README satisfies every required section.
- [ ] You can explain the concurrency design without relying on AI-generated explanations.

The final standard should be: **if a peer asks why any synchronization primitive, state field, scheduler rule, or cleanup step exists, you can explain the reason and point to the exact code implementing it.**

