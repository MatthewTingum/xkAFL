#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <hal/xbox.h>
#include <xboxkrnl/xboxkrnl.h>
#include <debug.h>
#include <hal/xbox.h>
#include <hal/debug.h>
#include <hal/video.h>
#include <nxdk/mount.h>
#include <windows.h>

#include "kafl_user.h"

char target_file[MAX_PATH] = { 0 };

uint32_t resolve_KeBugCheck() {
    //debugPrint("KeBugCheck @ 0x%x\n", &KeBugCheck);
    return (uint32_t)&KeBugCheck;
}

uint32_t resolve_KeBugCheckEx() {
    //debugPrint("KeBugCheckEx @ 0x%x\n", &KeBugCheckEx);
    return (uint32_t)&KeBugCheckEx;
}

void silly_stack_frame() {
    uint8_t* stack_buffer[16];
    uint8_t* heap_buffer = malloc(1024);
    memset(heap_buffer, 'A', 1024);
    memcpy(stack_buffer, heap_buffer, 1024);

    // This will most likely try to return to 0x41414141
    return;
}

void fuzz_something() {
    kAFL_payload* payload_buffer = (kAFL_payload*)VirtualAlloc(0, PAYLOAD_SIZE, MEM_COMMIT, PAGE_READWRITE);
    memset(payload_buffer, 0xff, PAYLOAD_SIZE);

    /* this hypercall submits the current CR3 value */ 
    kAFL_hypercall(HYPERCALL_KAFL_SUBMIT_CR3, 0);

    /* submit the guest virtual address of the payload buffer */
    kAFL_hypercall(HYPERCALL_KAFL_GET_PAYLOAD, (uint32_t)payload_buffer);

    while(1){
            kAFL_hypercall(HYPERCALL_KAFL_NEXT_PAYLOAD, 0);
            /* request new payload (*blocking*) */
            kAFL_hypercall(HYPERCALL_KAFL_ACQUIRE, 0); 

            /* kernel fuzzing */

            // Something basic to test crash detection
            if(payload_buffer->data[0] == 'X') {
                void (*fun_crash)() = 0x00000000; 
                fun_crash();
            }

            if(payload_buffer->data[0] == 'B' && payload_buffer->data[1] == 'X') {
                // Stack based buffer overflow
                silly_stack_frame();
            }

            /* inform fuzzer about finished fuzzing iteration */
            kAFL_hypercall(HYPERCALL_KAFL_RELEASE, 0);
    }

}

int main(void)
{
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    uint32_t panic_handler1 = 0x00000000;
    uint32_t panic_handler2 = 0x00000000;
    void* program_buffer;

    // TODO: AddVectoredExceptionHandler
    // TODO: Error handling
    // TODO: return values from hypercall

    //debugPrint("Resolving KeBugCheck...\n");
    panic_handler1 = resolve_KeBugCheck();
    //debugPrint("Resolving KeBugCheckEx...\n");
    panic_handler2 = resolve_KeBugCheckEx();

    /* this hypercall will generate a VM snapshot for the fuzzer and subsequently terminate QEMU */
    kAFL_hypercall(HYPERCALL_KAFL_LOCK, 0);

    // Slow down for race condition
    volatile unsigned long long t;
    for (t = 0; t < 1000*1000*10; ++t) {}

    //debugPrint("kAFL lock acquired!\n");

    /***** Fuzzer Entrypoint *****/


    /* initial fuzzer handshake */
    kAFL_hypercall(HYPERCALL_KAFL_ACQUIRE, 0);
    //debugPrint("HYPERCALL_KAFL_ACQUIRE Complete\n");
    kAFL_hypercall(HYPERCALL_KAFL_RELEASE, 0);
    //debugPrint("HYPERCALL_KAFL_RELEASE Complete\n");
    /* submit panic address */
    kAFL_hypercall(HYPERCALL_KAFL_SUBMIT_PANIC, panic_handler1);
    //debugPrint("HYPERCALL_KAFL_SUBMIT_PANIC Complete\n");
    kAFL_hypercall(HYPERCALL_KAFL_SUBMIT_PANIC, panic_handler2);
    //debugPrint("HYPERCALL_KAFL_SUBMIT_PANIC Complete\n");

    // The fuzzer program will be baked in...
    // This makes setup slightly more annoying

    // retail / debug address space
    // 0x00000000 - 0x03FFFFFF / 0x00000000 - 0x07FFFFFF
    // Trace range includes all of it for now (TODO: loader and info components)

    fuzz_something();

    /* bye */
    return 0;
}

