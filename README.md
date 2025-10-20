# Automotive Firmware OTA (FOTA) System for Infineon TC375

A portfolio project demonstrating a robust, safe Firmware Over-the-Air update solution for an automotive ECU, built from the ground up with a focus on reliability.

---

### ✨ **Technical Skills & Accomplishments**

*   **System Architecture & Safety-Critical Design:**
    *   Designed a **Secure A/B Partitioning** scheme. This ensures the ECU can always recover from a failed or interrupted update by preserving the last known-good application.
    *   Architected a custom **Boot Control Block (BCB)** system (in `/bootloader/Libraries/ota/bcb/`) to create a multi-layered safety net:
        *   **Interrupted Update Recovery:** Uses `updateRequest` and `updateInProgress` flags to detect if a reset occurred *during* a flashing operation, triggering an immediate rollback to the last working firmware.
        *   **Faulty Update Recovery:** Uses a `bootAttempt` counter with the **Watchdog Timer**. If a newly flashed application fails to boot successfully 3 times, the BCB automatically invalidates it and reverts to the previous, stable version.

*   **Low-Level Embedded Programming (Infineon TC375):**
    *   Engineered direct **Flash Programming** routines for erasing and writing firmware.
    *   Configured the memory map using **Linker Scripts (.lsl)** to create the distinct A/B partitions.

*   **Automotive Communication Stack Implementation:**
    *   Built a full communication stack from scratch: **CAN -> CAN-TP -> UDS**.
    *   Implemented the **CAN Transport Protocol (CAN-TP)** to handle the transfer of large firmware files over the CAN bus.
    *   Developed all necessary **UDS (Unified Diagnostic Services)** routines for the FOTA update sequence.

### 📂 **Project Structure & Core Libraries**

```
sota/
├── bootloader/       # 🛡️ ECU Bootloader Application
│   └── Libraries/
│       └── ota/      # ✨ Custom-built FOTA library
│           ├── bcb/
│           │   └── bcb.c       # Manages Boot Control Block (boot logic, rollback)
│           ├── can/
│           │   ├── can.c       # Low-level CAN driver configuration
│           │   ├── cantp.c     # ISO-TP (CAN Transport Protocol) implementation
│           │   └── uds.c       # UDS (Unified Diagnostic Services) implementation
│           └── flash_programming/
│               └── flash_programming.c # Direct flash memory access routines
├── appA/ & appB/     # 🚗 Dual application slots for the A/B system
└── tester/           # 🐍 Python client for orchestrating the FOTA update
```

### ⚙️ **The Secure Update & Rollback Process**

1.  **Prepare:** The Python client initiates a `Programming Session` via UDS on CAN.
2.  **Erase & Flash:** The client commands the bootloader to erase the *inactive* partition, then streams the new firmware using the custom CAN-TP layer.
3.  **Verify & Reboot:** The bootloader verifies the new firmware and reboots. If the new application fails to validate itself (by servicing the watchdog), the boot attempt counter in the BCB increases. After 3 failed attempts, the system automatically reverts to the old, stable version.

### 💻 **Core Technologies**

*   **Embedded:** C, Infineon TC375, Direct Flash Programming, Watchdog Timers, Linker Scripts
*   **Protocols:** CAN, CAN-TP (ISO 15765-2), UDS (ISO 14229)
*   **Hardware:** Infineon TC375 Lite Kit, Raspberry Pi 4