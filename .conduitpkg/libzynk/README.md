# `libzynk` 🚀

**`libzynk`** is a foundational C library designed to build the essential components of a scripting language runtime environment. With a keen focus on efficiency, resource management, and a clean architecture, `libzynk` provides robust abstractions for memory handling, dynamic variable management, and fundamental data types crucial for language interpreters and compilers.

-----

## ✨ Key Features

  * **Arena-Based Memory Management (`sysarena`):**
      * Implements an efficient arena allocator, ideal for managing numerous short-lived objects.
      * Minimizes memory fragmentation and amortizes allocation overhead, leading to improved performance compared to standard `malloc`/`free` patterns.
  * **Optimized Hash Tables (`zynk_enviroment`):**
      * A robust open-addressing hash table implementation utilizing linear probing.
      * Designed for efficient symbol and variable management within the runtime, featuring "tombstones" for intelligent deletion and slot reuse.
      * Supports nested environments (scopes), enabling a hierarchical variable visibility system essential for function calls and block scopes.
  * **Unified Primitive Data Types (`types.h`):**
      * Defines a flexible `Value` type to uniformly handle various data kinds, such as floating-point numbers, booleans, and null values.
      * Facilitates seamless data manipulation and interoperability throughout the language runtime.
  * **Core Utilities:** Includes fundamental functionalities like hashing algorithms (`hash.h`) and value assignment operations (`assign.h`) that underpin the library's internal workings.

-----

## 📦 Core Modules

`libzynk` is structured into several modular components, each serving a specific purpose:

### `src/zynk_enviroment` - Environment and Hash Table Management

This module implements the core hash table functionality for managing variables and symbols within the Zynk runtime.

#### How It Works

This hash table uses an **open addressing** scheme with **linear probing** for collision resolution.

  * **Linear Probing:** When two different keys map to the same initial position (a collision), the system sequentially searches for the next available slot. This process continues until an empty slot is found (for new insertions), or the target key is matched.
  * **Tombstones:** To handle deletions efficiently and maintain the integrity of probing sequences, entries are not completely removed. Instead, when an entry is deleted using `zynkTableDelete`, its `name` field is set to `NULL`. These `NULL`-marked slots are referred to as "tombstones." During subsequent search operations (Get, Set, Delete), tombstones are "skipped" (i.e., treated as if an element *were* there for the purpose of continuing the probe), allowing the search to continue beyond a deleted element to find a potentially later-collided active element. Only during new insertions (`zynkTableNew`) can tombstones be reused and overwritten by new entries.
  * **Load Factor:** The efficiency of an open-addressing hash table is greatly influenced by its load factor (number of active elements / total capacity). A high load factor (approaching 100%) can lead to performance degradation due to "clustering" (grouping of elements), which increases average search times. The table's initial capacity can be set, and it will attempt insertions until fully occupied.

#### Key Structures

  * **`ZynkEnvEntry`**:

    ```c
    struct ZynkEnvEntry {
      char *name;  // The key (string) for the entry.
      Value value; // The value associated with the key (e.g., number, boolean, null).
    };
    ```

    Represents a single key-value pair stored within a hash table slot.

  * **`ZynkEnvTable`**:

    ```c
    struct ZynkEnvTable {
      struct ZynkEnvEntry** entries; // An array of pointers to ZynkEnvEntry structs.
      size_t capacity;               // The total number of available slots in the table.
      size_t count;                  // The current number of active (non-tombstone) entries.
    };
    ```

    The core hash table structure, managing the array of entry pointers, its capacity, and the live element count.

  * **`ZynkEnv`**:

    ```c
    struct ZynkEnv {
      struct ZynkEnvTable *local;    // The local hash table for this specific environment scope.
      struct ZynkEnv *enclosing;     // Pointer to the parent environment, enabling scope chaining.
    };
    ```

    Encapsulates a runtime environment, facilitating the creation of nested scopes (e.g., for function calls or isolated code blocks) that can inherit or override variables from parent scopes.

#### Core Functions

  * `bool zynkEnvInit(ZynkEnv *env, size_t capacity, ZynkEnv *enclosing)`: Initializes a new `ZynkEnv` instance, configuring its local hash table with a given `capacity` and linking it to an `enclosing` (parent) environment if provided.
  * `bool initZynkTable(ZynkEnvTable *table, size_t capacity)`: Initializes a `ZynkEnvTable` by allocating memory for its `ZynkEnvEntry` structs and marking all slots as initially empty.
  * `bool freeZynkTable(ArenaManager *manager, ZynkEnvTable *table)`: Deallocates all memory associated with the hash table, including individual `ZynkEnvEntry` structs, key strings, the `entries` array, and the `ZynkEnvTable` itself, using the provided `ArenaManager`.
  * `bool zynkTableNew(ZynkEnv *env, const char *str, Value value)`: Inserts a **new** key-value pair into the table. It will only succeed if the `str` key does not already exist.
  * `bool zynkTableSet(ZynkEnv *env, const char *str, Value value)`: Updates the `value` for an **existing** `str` key. It will fail if the key does not already exist.
  * `Value zynkTableGet(ZynkEnv *env, const char *str)`: Retrieves the `Value` associated with a `str` key. Returns a `ZYNK_NULL` value if the key is not found.
  * `bool zynkTableDelete(ZynkEnv *env, const char *str)`: Deletes a key-value pair from the table by marking its slot as a tombstone (`name = NULL`) and decrementing the active element count.
  * `ZynkEnvEntry* zynkFindEntry(ZynkEnv *env, const char *key, bool niu)`: An internal helper function crucial for all hash table operations. It searches for an entry based on `key`. The `niu` (New If Unknown) parameter dictates its behavior: `true` attempts to find a suitable slot for a *new* insertion (even if it's a tombstone), while `false` only searches for an *existing* entry.

#### Memory Management within `zynk_enviroment`

This module is deeply integrated with the `sysarena` memory management system. All dynamic memory allocations for `ZynkEnvEntry` structs, the `char* name` strings (for keys), the `ZynkEnvEntry** entries` array, and the `ZynkEnvTable` struct itself are performed using `sysarena_alloc`. This approach centralizes memory handling, reducing overhead and fragmentation commonly associated with frequent small allocations and deallocations. `freeZynkTable` is responsible for returning this memory to the arena.

### `src/sysarena` - Arena Memory Allocator

This module provides an efficient arena-based memory allocation system. Arenas allow for very fast allocation of memory blocks from a pre-allocated pool, and then deallocating all memory in an arena at once. This is highly beneficial for scenarios like function calls or block executions where many temporary objects are created and destroyed together.

### `src/types` - Unified Value Type

Defines the `Value` union/struct, which serves as a flexible container for all primitive data types within Zynk (numbers, booleans, null). This abstraction simplifies type handling throughout the runtime.

### `src/runtime/` - Low-Level Runtime Utilities

Contains foundational components for the Zynk runtime, including basic memory utilities, hashing functions (`hash.h`), and value assignment routines (`assign.h`) that facilitate internal data operations.

-----

## 🚀 Getting Started

To build `libzynk` and its associated testing tools, ensure you have a C compiler (like `gcc`) and `make` installed on your system.

### Clone the Repository

```bash
git clone https://github.com/Guille-ux/libzynk
cd libzynk ```

### Compilation

The project can be compiled using the provided `Makefile`.

```bash
make
```

This command will compile the `libzynk` library and any configured executable tests, such as `tenv`.

### Running Tests (Example)

After a successful compilation, you can execute the hash table environment test suite:

```bash
./tenv
```

-----

## 📚 Usage Example: Hash Table Operations

This example demonstrates how to set up and use the core hash table functionalities of `libzynk`.

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For strlen, etc.
#include "src/zynk.h" // Includes main libzynk headers like zynk_enviroment.h, sysarena.h, types.h

// --- IMPORTANT: Global ArenaManager Setup (Typical for testing/simple setups) ---
// In a more complex application, the ArenaManager might be passed through context.
// These declarations must match where g_test_arena_manager and its pool/arenas are defined.
// For your `tenv.c` setup, these are typically defined in `tenv.c` or a common test header.
extern ArenaManager g_test_arena_manager;
extern uint8_t g_test_memory_pool[]; // Assuming a pre-allocated memory pool
extern Arena g_test_arenas[];        // Assuming a pre-allocated array of Arena structs

int main() {
    printf("--- Zynk Hash Table Usage Example ---\n");

    // 1. Initialize the Arena Memory System (sysarena)
    // These sizes should be determined based on your application's memory needs.
    size_t memory_pool_size = 1024 * 1024; // 1MB total memory pool
    size_t num_arenas = 32;                // Number of individual arenas within the manager

    bool sysarena_ok = sysarena_init(
        &g_test_arena_manager,
        g_test_memory_pool,
        g_test_arenas,
        memory_pool_size,
        num_arenas
    );

    if (!sysarena_ok) {
        fprintf(stderr, "Error: Failed to initialize sysarena.\n");
        return 1;
    }
    printf("Sysarena initialized successfully.\n");

    // 2. Initialize the Zynk Environment (ZynkEnv and ZynkEnvTable)
    ZynkEnv env;
    size_t initial_table_capacity = 8; // Initial capacity for the hash table

    // Note: zynkEnvInit might allocate env.local and its entries internally
    // in a full Zynk system. This example explicitly allocates them
    // as is done in the `tenv.c` test suite for clarity on memory ownership.
    env.local = (ZynkEnvTable*)sysarena_alloc(&g_test_arena_manager, sizeof(ZynkEnvTable));
    if (env.local == NULL) { fprintf(stderr, "Error allocating ZynkEnvTable.\n"); return 1; }

    env.local->entries = (ZynkEnvEntry**)sysarena_alloc(&g_test_arena_manager, sizeof(ZynkEnvEntry*) * initial_table_capacity);
    if (env.local->entries == NULL) { fprintf(stderr, "Error allocating ZynkEnvEntry* array.\n"); return 1; }

    // Allocate each ZynkEnvEntry struct and initialize it
    for (size_t i = 0; i < initial_table_capacity; ++i) {
        env.local->entries[i] = (ZynkEnvEntry*)sysarena_alloc(&g_test_arena_manager, sizeof(ZynkEnvEntry));
        if (env.local->entries[i] == NULL) { fprintf(stderr, "Error allocating ZynkEnvEntry.\n"); return 1; }
        env.local->entries[i]->name = NULL;   // Mark as empty/tombstone
        env.local->entries[i]->value = zynkNull(); // Default to ZYNK_NULL
    }
    env.local->capacity = initial_table_capacity;
    env.local->count = 0; // Initially no active elements
    env.enclosing = NULL; // This is the top-level environment for this example

    printf("Zynk Environment initialized with hash table capacity %zu.\n", env.local->capacity);

    // 3. Insert New Elements (zynkTableNew)
    // Important: The `str` key passed to zynkTableNew/Set/Get/Delete must point to
    // a valid, null-terminated string. For sysarena-managed memory, ensure the key string
    // itself is allocated via sysarena_alloc and is copied, not just passed by pointer.
    char *key_var1 = (char*)sysarena_alloc(&g_test_arena_manager, strlen("variable1") + 1); strcpy(key_var1, "variable1");
    char *key_var2 = (char*)sysarena_alloc(&g_test_arena_manager, strlen("variable2") + 1); strcpy(key_var2, "variable2");
    char *key_var3 = (char*)sysarena_alloc(&g_test_arena_manager, strlen("variable3") + 1); strcpy(key_var3, "variable3");


    if (zynkTableNew(&env, key_var1, zynkNumber(123.45))) {
        printf("Successfully inserted 'variable1' with value 123.45\n");
    } else {
        printf("Failed to insert 'variable1'.\n");
    }

    if (zynkTableNew(&env, key_var2, zynkBool(true))) {
        printf("Successfully inserted 'variable2' with value true.\n");
    } else {
        printf("Failed to insert 'variable2'.\n");
    }
    printf("Current active elements in table: %zu\n", env.local->count);


    // 4. Retrieve Elements (zynkTableGet)
    Value val1 = zynkTableGet(&env, "variable1");
    if (val1.type == ZYNK_NUMBER) {
        printf("Retrieved value of 'variable1': %f\n", val1.as.number);
    } else {
        printf("Could not retrieve 'variable1' or it's of an unexpected type.\n");
    }

    Value val_non_existent = zynkTableGet(&env, "non_existent_key");
    if (val_non_existent.type == ZYNK_NULL) {
        printf("'non_existent_key' not found (as expected for a new key).\n");
    }

    // 5. Update an Existing Element (zynkTableSet)
    if (zynkTableSet(&env, key_var1, zynkNumber(999.0))) {
        printf("Successfully updated 'variable1' to 999.0\n");
        val1 = zynkTableGet(&env, "variable1");
        printf("New value of 'variable1' after update: %f\n", val1.as.number);
    } else {
        printf("Failed to update 'variable1'.\n");
    }

    // Attempt to set a new key using zynkTableSet (should fail)
    if (!zynkTableSet(&env, key_var3, zynkNumber(777.0))) {
        printf("Attempt to set 'variable3' (a new key) failed as expected (zynkTableSet is for existing keys).\n");
    }


    // 6. Delete an Element (zynkTableDelete)
    if (zynkTableDelete(&env, "variable2")) {
        printf("Successfully deleted 'variable2'.\n");
    } else {
        printf("Failed to delete 'variable2'.\n");
    }
    printf("Current active elements in table after deletion: %zu\n", env.local->count);

    Value val2_after_delete = zynkTableGet(&env, "variable2");
    if (val2_after_delete.type == ZYNK_NULL) {
        printf("Verification: 'variable2' no longer exists in the table.\n");
    }

    // 7. Cleanup (freeZynkTable)
    if (freeZynkTable(&g_test_arena_manager, env.local)) {
        printf("Hash table memory freed successfully.\n");
    } else {
        printf("Failed to free hash table memory.\n");
    }

    // At this point, the global sysarena_manager would still be active.
    // In a full application, sysarena might have a shutdown function,
    // or its memory pool would be reclaimed upon process exit.

    return 0;
}
```

-----

## 📂 Project Structure

```
libzynk/
├── src/
│   ├── common.h                 # Common definitions and utility macros.
│   ├── zynk.h                   # Main public header for the entire library.
│   │
│   ├── zynk_enviroment.h        # Public interface for hash table and environment management.
│   ├── zynk_enviroment.c        # Implementation of hash table and environment logic.
│   │
│   ├── types.h                  # Defines the unified 'Value' data type.
│   │
│   ├── runtime/                 # Low-level runtime components.
│   │   ├── memory.h             # Basic memory utilities (e.g., for direct allocations outside arena).
│   │   ├── memory.c             # Implementation for basic memory utilities.
│   │   ├── hash.h               # Hashing function declarations.
│   │   ├── hash.c               # Implementation of hashing algorithms.
│   │   ├── assign.h             # Value assignment and manipulation declarations.
│   │   └── assign.c             # Implementation for value assignment.
│   │
│   └── sysarena/                # Arena memory allocator module.
│       ├── sysarena.h           # Public interface for the arena allocator.
│       └── sysarena.c           # Implementation of the arena allocator.
│
├── tenv.c                       # Standalone test suite for the zynk_enviroment module.
├── Makefile                     # Build system configuration.
└── LICENSE                      # Project license file.
```

-----

## 🤝 Contributing

We welcome contributions to `libzynk`\! Whether you're fixing a bug, adding a new feature, or improving documentation, your efforts are appreciated.

To contribute:

1.  **Fork** the repository.
2.  **Clone** your forked repository.
3.  Create a new **branch** for your feature or bug fix (`git checkout -b feature/your-feature-name` or `bugfix/issue-description`).
4.  Make your changes and **test** them thoroughly.
5.  **Commit** your changes with clear, concise messages.
6.  **Push** your branch to your fork.
7.  Open a **Pull Request** to the `main` branch of the original repository.

Please ensure your code adheres to the existing coding style and includes appropriate tests.

-----

## 🔒 Security Policy

At `libzynk`, we are committed to ensuring the security and stability of our library. If you discover a security vulnerability, we urge you to report it responsibly.

Please **DO NOT** open a public issue for security vulnerabilities. Instead, contact the project maintainers directly at [YOUR\_EMAIL\_OR\_SECURITY\_CONTACT\_HERE]. We will work with you to understand and address the issue promptly.

-----

## 🗣️ Community Standards

`libzynk` aims to foster an open, inclusive, and welcoming community. We expect all contributors and users to adhere to the following standards:

  * **Be Respectful:** Treat everyone with courtesy and respect. Disagreements happen, but personal attacks or harassment are unacceptable.
  * **Be Welcoming:** Help make the community a safe and friendly place for everyone, regardless of experience level, background, or identity.
  * **Be Constructive:** Provide feedback and criticism constructively, focusing on the code and ideas, not the person.
  * **Be Collaborative:** Work together to solve problems and build a better library.
  * **Be Professional:** Maintain professionalism in all interactions.

Violations of these standards may result in temporary or permanent exclusion from the community.

-----

## 📄 License

`libzynk` is open-source software licensed under the **LGPL License**.

You are free to use, modify, and distribute this software under the terms of the LGPL license. Please refer to the `LICENSE` file in the repository root for the full license text.

-----
