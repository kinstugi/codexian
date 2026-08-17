*This project has been created as part of the 42 curriculum by bkusi-fr.*

# Codexion

A 42-philosophers-style C/pthreads project, re-themed around coders and
dongles: every coder is a thread, every dongle is a shared resource guarded
by a mutex, and the simulation stops when a coder burns out or every coder
finishes its required compiles.

## Description

The program simulates a group of coders sharing a pool of USB-C dongles.
Each coder must repeatedly acquire the dongles it needs, compile, debug,
and refactor. If a coder waits too long to start compiling, it burns out
and the simulation ends. The program supports FIFO and EDF scheduling for
dongle allocation and enforces a cooldown on every released dongle.

The implementation follows the classic dining philosophers pattern with a
central scheduler replacing naive per-coder locking, which removes the
circular-wait deadlock and provides fair FIFO/EDF arbitration.

## Instructions

Build with:

```sh
make
```

Run with:

```sh
./codexion number_of_coders time_to_burnout time_to_compile \
           time_to_debug time_to_refactor \
           number_of_compiles_required dongle_cooldown scheduler
```

Example:

```sh
./codexion 5 800 100 50 50 3 20 fifo
./codexion 4 1200 80 40 40 2 0 edf
```

The last argument selects the scheduler:

- `fifo` — first request to wait wins.
- `edf`  — earliest deadline (closest to burnout) wins.

Clean build artifacts with:

```sh
make clean   # remove object files
make fclean  # remove object files and binary
make re      # full rebuild
```

## Resources

- One `pthread_t` per coder (`src/simulation_run.c`).
- One separate monitor thread (`src/simulation_run.c`).
- `number_of_coders` dongles, each protected by its own `pthread_mutex_t`
  (`src/simulation_resources.c`, `src/dongle.c`).
- A single simulation-wide `pthread_mutex_t` and `pthread_cond_t` for
  scheduler state and shutdown signalling (`src/simulation_init.c`).
- A logging mutex for serialized standard output (`src/simulation_init.c`).
- A custom binary-min-heap priority queue used by the scheduler
  (`src/heap.c`, `src/heap_ops.c`, `src/heap_utils.c`).
- Allocated resources: simulation struct, coder array, dongle array,
  scheduler pending buffer, heap items, and configuration
  (`src/simulation_init.c`, `src/simulation_resources.c`,
  `src/simulation_destroy.c`).

## Blocking cases handled

- **Deadlock**: avoided by central arbitration. Coders do not lock dongles
themselves; they submit a request to the scheduler, which checks whether
both required dongles are free and grants them atomically under the
simulation mutex. Because no coder ever holds one dongle while waiting for
another, circular wait cannot form.

- **Starvation**: avoided by granting every currently satisfiable request in
priority order on every state change. A waiting request is re-evaluated
whenever a dongle is released or a new request arrives. Under FIFO the
earliest pending request always wins; under EDF the request with the
earliest deadline always wins. Disjoint pairs can still compile in parallel.

- **Burnout**: detected by the monitor thread. A coder burns out when the
elapsed time since its last compile start exceeds `time_to_burnout`, unless
it is currently compiling or has already finished. The monitor sleeps with
`pthread_cond_timedwait` until the nearest coder deadline, defaulting to
`now + 10` ms when no sooner deadline exists, so the burnout message
appears within roughly 10 ms.

- **Premature shutdown completion**: the simulation stops as soon as every
coder has completed `number_of_compiles_required` compiles, even if some
coders are between phases.

- **Thread creation failure**: if a coder or monitor thread cannot be
created, the stop flag is set, already started threads are joined, and
resources are destroyed safely.

- **Partial initialization failure**: each initialization step records
whether it succeeded. `simulation_destroy` only destroys mutexes, conditions,
dongles, and the scheduler that were actually initialized
(`src/simulation_destroy.c`).

## Thread synchronization mechanisms

### Deadlock prevention / Coffman's conditions

Coffman's four necessary conditions for deadlock are:

1. **Mutual exclusion** — dongles are exclusive, but this is fine because...
2. **Hold and wait** — is removed. A coder never holds a dongle while
   waiting for another. Requests are submitted to the scheduler, and the
   scheduler only grants a request when both dongles are free.
3. **No preemption** — not needed; resources are released voluntarily after
   each compile.
4. **Circular wait** — is removed because there is no per-coder locking
   order; the scheduler owns the only grant decision.

### Starvation prevention

The scheduler pops all pending requests in heap order and grants every
request whose dongles are currently free. Requests that cannot be granted
are pushed back into the heap. This guarantees that the highest-priority
request is granted the moment its resources become available, and lower
priority requests are not skipped indefinitely. Disjoint dongle pairs run
in parallel, so one waiting coder does not block unrelated grants.

### FIFO scheduling

In FIFO mode the scheduler key for each request is a monotonically
increasing sequence number assigned at submission time
(`src/scheduler.c`). The min-heap orders requests by this sequence, so the
coder that submitted first wins. The same sequence number is also used as
the tie-breaker, so a lower sequence number always wins.

### EDF scheduling

In EDF mode the scheduler key is the request deadline, defined as
`last_compile_start + time_to_burnout` (`src/scheduler.c`). The coder whose
next compile start is most urgent gets the next available resources. If two
requests share the same deadline, the earlier sequence number breaks the
tie.

### Dongle cooldown

When a coder releases its dongles, `dongles_release_both` records the
current time plus `dongle_cooldown` as `available_at`
(`src/dongle_release.c`). The scheduler treats a dongle as free only when
`now >= available_at`, so a coder cannot immediately reacquire a dongle
that is still cooling.

### Burnout detection

The monitor thread holds the simulation mutex, reads every coder's
`last_compile_start` and `compile_count`, and checks whether
`now - last_compile_start >= time_to_burnout` for coders that are neither
DONE nor COMPILING and have not reached the required compile count
(`src/monitor.c`). On burnout it sets the coder state to `BURNED_OUT`, sets
the global stop flag, broadcasts the condition variable, prints the burnout
message exactly once, and exits.

### Log serialization

Every standard-output log line is produced by `logger()` or `logger_force()`
(`src/logger.c`). `logger()` first locks `simulation->sync.mutex` to check
`stop_flag` consistently, then locks `logging`, prints the message, and
unlocks both mutexes. `logger_force()` is used by the monitor when it
already owns `sync.mutex`; it only locks `logging` for the actual print.
This guarantees that log lines never interleave and that no regular state
message is printed after shutdown has begun. Error messages printed to
`stderr` by the parser and initialization code are not part of the
serialized log stream.

### Mutex usage

- `simulation->sync.mutex` — protects the scheduler heap, pending buffer,
  `next_seq`, all coder `state`, `compile_count`, `last_compile_start`,
  `granted`, request fields, and the global `stop_flag`.
- `simulation->logging` — protects `printf` so log lines are atomic.
- Each `t_dongle->mutex` — protects `owner`, `available_at`, and the result
  of `dongle_is_free`/`dongle_wake_time`.

The lock hierarchy is: `sync.mutex` first, then at most one dongle mutex
at a time during checks and grants. When two dongle mutexes must both be
locked for release, `dongles_release_both` always acquires them in
increasing order of `dongle->id` (`src/dongle_release.c`). This consistent
ordering prevents circular wait even when two mutexes are nested.

### Condition variables

A single condition variable (`simulation->sync.condition`) is broadcast:

- when a coder releases dongles,
- when the scheduler grants any request,
- when the monitor detects burnout or completion,
- when `simulation_stop` is called.

Waiting coders use `pthread_cond_timedwait` with a deadline based on the
sooner of the next dongle cooldown expiry and a 1000 ms cap. After waking
they re-check the stop flag and whether they have been granted before
waiting again, because spurious wakeups and broadcasts can occur.

### Monitor communication

The monitor is the only thread that declares burnout. It runs inside the
same `sync.mutex` as the scheduler. Coders learn about shutdown either by
seeing `stop_flag` set when they check it or by being woken by the
broadcast. All coders exit their routines cleanly, release any held
dongles, and are joined before the simulation mutex is destroyed.

### Custom heap

The scheduler uses a custom binary min-heap implemented in
`src/heap.c`/`src/heap_ops.c`/`src/heap_utils.c`. It stores items with a
primary `key` and a tie-breaker `tie`. This gives O(log n) insertion and
extraction and makes FIFO/EDF selection trivial: change the key function
and the heap ordering follows automatically.

## AI usage

AI was used during development as an interactive coding assistant: it
helped structure the milestone plan, suggested file splits for Norm
compliance, drafted helper functions, produced reviewer prompts, and
flagged potential race conditions. All generated code was reviewed,
compiled with `-Wall -Wextra -Werror`, tested with Valgrind and
ThreadSanitizer, and adjusted to match the actual implementation. The
final design decisions, synchronization protocol, and README explanations
were reviewed and verified against the subject requirements.
