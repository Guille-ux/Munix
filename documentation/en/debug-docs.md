## 🛠️ Build Scripts: `debug.sh`

This script automates multiple development tasks for Munix, from compilation to execution in QEMU.

### 📋 Basic Use
```bash
./debug.sh [command]
```
### 🔧 Available Commands

| Command | Description | Dependencies |
|---------------|-----------------------------------------------------------------------------|----------------------------------|
| `update` | Updates the dependencies (libcs2) | `../upcpkg` |
| `compile` | Runs the main compilation script (`compile.sh`) | `compile.sh` |
| `build-libs` | Builds all libraries from the `libs/` directory | `libs/build.sh` |
| `run` | Runs Munix on QEMU (i386) with debugging support (`-s`) | `qemu-system-i386`, `munix.iso` |
| `liball` | Complete library preparation: updates, builds, and copies to the project | `mkdir`, `cp`, `rm` |
| `all` | Full compilation and execution | Requires `compile.sh` |
| `-a` | **All-in-One Mode**: Runs `liball` followed by `all` | All of the above |

### 🎯 Usage Examples

1. **Complete development flow:**
```bash
```
./debug.sh -a
```
*Equivalent to:*
```
*Equivalent to:*
```bash
./debug.sh update && \
```
./debug.sh update && \
./debug.sh build-libs && \
./debug.sh compile && \
./debug.sh run
```

2. **Just compile and run:** **Just compile and run:**
```bash
./debug.sh all
```
3. **Rebuild libraries:** **Rebuild libraries:**
```bash
./debug.sh liball
```

### 🔍 Technical Details

1. **Expected Directory Structure:** **Expected Directory Structure:**
```
project/
├── debug.sh
├── compile.sh
├── libs/
└──
│ └── build.sh
├── Munix/
│ └── libs/ # Created automatically
└── .conduitpkg/ # External dependencies
```

2. **QEMU Options:**
- `-s`: Enables the gdb server (port 1234)
- You can add `-serial stdio` for better logging

3. **Useful Environment Variables:**
```bash
export QEMU_OPTS="-m 256M -enable-kvm"
```
export QEMU_OPTS="-m 256M -enable-kvm"
./debug.sh run
```

### ❌ Error Handling
- If an unrecognized command is passed, display: `[ Unrecognized Command ]`
- Each stage clearly shows when it starts and ends

### 🔄 Recommended Workflow
```mermaid
```
gráfico TD
A[./debug.sh -a] --> B[Updates dependencies]
B --> C[Builds libraries]
C --> D[Copies libraries to Munix/]
D --> E[Compile project]
E --> F[Runs in QEMU]
```