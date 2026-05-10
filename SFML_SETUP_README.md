# CPU Scheduler Simulator — SFML GUI Setup Guide
## MinGW 11 w64 + SFML 2.6

---

## STEP 1 — Download SFML (exact version)

Go to: https://www.sfml-dev.org/download/sfml/2.6.2/

Download:
**SFML 2.6.2 - GCC 13.1.0 MinGW (SEH) - 64-bit**

File name: `SFML-2.6.2-windows-gcc-13.1.0-mingw-64-bit.zip`

> Why this version? Your MinGW is 11.x w64. SFML's 64-bit GCC build
> is ABI-compatible. If you get linker errors, try the 32-bit build instead.

---

## STEP 2 — Extract SFML

Extract to a simple path with NO spaces, e.g.:

```
C:\SFML\
```

After extracting you should have:
```
C:\SFML\
    bin\
    include\
        SFML\
    lib\
        cmake\
            SFML\
                SFMLConfig.cmake   ← this is what CMake needs
        libsfml-graphics.a
        ...
```

---

## STEP 3 — Open project in CLion

1. Open CLion → **Open** → select the `OperatingSystemProject` folder (the one with CMakeLists.txt)
2. CLion will detect CMake automatically

---

## STEP 4 — Set the SFML path in CMake settings

In CLion:  
**File → Settings → Build, Execution, Deployment → CMake**

In **CMake options** add:

```
-DSFML_DIR=C:/SFML/lib/cmake/SFML
```

*(Change C:/SFML to wherever you extracted SFML)*

Then click **OK** and let CMake reload.

---

## STEP 5 — Build & Run

Click the green **Run** button (or press Shift+F10).

The exe will be in `cmake-build-debug/OperatingSystemProject.exe`

SFML DLLs are copied automatically next to the exe by the CMakeLists.

---

## STEP 6 — Font (arial.ttf)

The GUI uses Arial font. The code tries these paths in order:

1. `arial.ttf` (same folder as the exe)
2. `C:\Windows\Fonts\arial.ttf`
3. `C:\Windows\Fonts\segoeui.ttf`
4. `C:\Windows\Fonts\tahoma.ttf`

On Windows, path #2 will work automatically — no action needed.

---

## Troubleshooting

| Problem | Fix |
|---|---|
| `SFML not found` error | Check `-DSFML_DIR` path in CLion CMake settings |
| `undefined reference to sfml-...` | Make sure you downloaded the 64-bit GCC version |
| App crashes on start | Copy `sfml-graphics-2.dll`, `sfml-window-2.dll`, `sfml-system-2.dll` from `C:\SFML\bin\` next to your `.exe` |
| Blank/invisible text | Font not found — copy `arial.ttf` next to the exe |
| Linker error about `-mwindows` | Remove `WIN32` from CMakeLists add_executable (some MinGW setups need this) |

---

## GUI Features

- **Add Process**: fill ID, Arrival, Burst, Priority → click `+ Add` or press Enter  
- **TAB**: jump between fields  
- **Algorithm**: choose Priority Preemptive / Non-Preemptive / Round Robin / Run All  
- **Run ▶**: runs simulation → Results screen with Gantt chart + table  
- **Scroll**: mouse wheel on Results screen  
- **Back**: return to main screen  
- **Clear**: remove all processes  

