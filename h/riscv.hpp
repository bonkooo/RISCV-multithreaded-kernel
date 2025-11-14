#include "../lib/hw.h"
#include "../h/BoundedBuffer.hpp"

constexpr uint64 STOP_ADDR = 0x100000;
constexpr uint64 STOP_VAL = 0x5555;

class Riscv {
public:

    // dohvati spp i spie bitove iz sip registra
    static void popSppSpie();



    static void pushRegisters();
    static void popRegisters();

    // bitske maske za sstatus
    enum BitMaskSstatus {
        SSTATUS_SIE = (1 << 1),  // maskiranje spoljasnjih prekida (u user rezimu se ignorise)
        SSTATUS_SPIE = (1 << 5), // prethodna vrednost sie
        SSTATUS_SPP = (1 << 8)   // iz kog rezima se desio skok (0 - user; 1 - sistemski)
    };

    // bitske maske za sip
    // sip sadrzi informaciju o tome koji zahtevi za prekid su trenutno aktivni
    enum BitMaskSip {
        SIP_SSIP = (1 << 1),  // postoji zahtev za softverski prekid
        SIP_STIP = (1 << 5),  // postoji zahtev za prekid tajmera
        SIP_SEIP = (1 << 9)   // postoji zahtev za spoljasnji prekid
    };

    enum syscallOp {
        MEM_ALLOC = 0X01,
        MEM_FREE = 0x02,
        MEM_GET_FREE_SPACE = 0x03,
        MEM_GET_LARGEST_FREE_BLOCK = 0x04,
        THREAD_CREATE = 0x11,
        THREAD_EXIT = 0x12,
        THREAD_DISPATCH = 0x13,
        THREAD_JOIN = 0x14,
        SEM_OPEN = 0x21,
        SEM_CLOSE = 0x22,
        SEM_WAIT = 0x23,
        SEM_SIGNAL = 0x24,
        TIME_SLEEP = 0x31,
        GETC = 0x41,
        PUTC = 0x42
    };

    enum trapCode : uint64 {
        U_ECALL = 0x0000000000000008UL,
        S_ECALL = 0x0000000000000009UL,
        TIMER_INTERRUPT = 0x8000000000000001UL,
        CONSOLE_INTERRUPT = 0x8000000000000009UL

    };

    static uint64 rScause();
    static void wScause(uint64 scause);
    static uint64 rSepc();
    static void wSepc(uint64 sepc);
    static uint64 rStvec();
    static void wStvec(uint64 stvec);
    static uint64 rStval();
    static void wStval(uint64 stval);

    // operacije sa sstatus registrom
    static void msSstatus(uint64 mask);  // postavi masku
    static void mcSstatus(uint64 mask);  // clearuj masku
    static uint64 rSstatus();
    static void wSstatus(uint64 sstatus);

    // operacije sa sip registrom
    static void msSip(uint64 mask);
    static void mcSip(uint64 mask);
    static uint64 rSip();
    static void wSip(uint64 sip);


    // operacije sa registrima a0..7
    static inline uint64 readA0() {
        uint64 value;
        asm("mv %0, a0;" : "=r"(value));
        return value;
    }

    static inline uint64 readA1() {
        uint64 value;
        asm("mv %0, a1;" : "=r"(value));
        return value;
    }

    static inline uint64 readA2() {
        uint64 value;
        asm("mv %0, a2;" : "=r"(value));
        return value;
    }

    static inline uint64 readA3() {
        uint64 value;
        asm("mv %0, a3;" : "=r"(value));
        return value;
    }

    static inline uint64 readA4() {
        uint64 value;
        asm("mv %0, a4;" : "=r"(value));
        return value;
    }

    static inline uint64 readA5() {
        uint64 value;
        asm("mv %0, a5;" : "=r"(value));
        return value;
    }

    static inline uint64 readA6() {
        uint64 value;
        asm("mv %0, a6;" : "=r"(value));
        return value;
    }

    static inline uint64 readA7() {
        uint64 value;
        asm("mv %0, a7;" : "=r"(value));
        return value;
    }

    static inline void writeA0(uint64 value) {
        asm volatile("mv a0, %0" :: "r"(value));
    }

    static inline void writeA1(uint64 value) {
        asm volatile("mv a1, %0" :: "r"(value));
    }

    static inline void writeA2(uint64 value) {
        asm volatile("mv a2, %0" :: "r"(value));
    }

    static inline void writeA3(uint64 value) {
        asm volatile("mv a3, %0" :: "r"(value));
    }

    static inline void writeA4(uint64 value) {
        asm volatile("mv a4, %0" :: "r"(value));
    }

    static inline void writeA5(uint64 value) {
        asm volatile("mv a5, %0" :: "r"(value));
    }

    static inline void writeA6(uint64 value) {
        asm volatile("mv a6, %0" :: "r"(value));
    }

    static inline void writeA7(uint64 value) {
        asm volatile("mv a7, %0" :: "r"(value));
    }

    static inline void wSscratch(uint64 sscratch) {
        asm volatile("csrw sscratch, %[sscratch]" : : [sscratch] "r"(sscratch));
    }

    static BoundedBuffer* putcBuffer;
    static BoundedBuffer* getcBuffer;

    static void supervisorTrap();

    static void endEmulator();

    static bool uMode;
    static int threadCount;
    static int maxThreads;
    static void setMaxThreads (int count) {maxThreads = count;}
private:
    static void handleSupervisorTrap();
    static void handleSyscall(uint64 opCode, uint64 arg1, uint64 arg2, uint64 arg7);
    static void handleTimerInterrupt();
};

inline uint64 Riscv::rScause() {
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::wScause(uint64 scause) {
    __asm__ volatile("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::rSepc() {
    uint64 volatile sepc;
    __asm__ volatile("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::wSepc(uint64 sepc) {
    __asm__ volatile("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::rStvec() {
    uint64 volatile stvec;
    __asm__ volatile("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::wStvec(uint64 stvec) {
    __asm__ volatile("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::rStval() {
    uint64 volatile stval;
    __asm__ volatile("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::wStval(uint64 stval) {
    __asm__ volatile("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Riscv::msSstatus(uint64 mask) {
    uint64 sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    sstatus |= mask;
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline void Riscv::mcSstatus(uint64 mask) {
    uint64 sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    sstatus &= ~mask;
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline uint64 Riscv::rSstatus() {
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::wSstatus(uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline void Riscv::msSip(uint64 mask)
{
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mcSip(uint64 mask)
{
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::rSip()
{
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::wSip(uint64 sip)
{
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}




