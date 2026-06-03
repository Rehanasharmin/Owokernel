#include "syscall.h"
#include <stddef.h>

void shell_main() {
    char input[64];
    int pos = 0;

    while (1) {
        sys_print("Nova# ");
        pos = 0;

        // Read input from keyboard
        while (1) {
            char c = sys_read_kbd();
            if (c == 0) continue;
            if (c == '\n' || c == '\r') break;
            if (pos < 63) {
                input[pos++] = c;
                sys_print("&c"); // This is a simplified print, in a real shell we'd print the char
            }
        }
        input[pos] = '\0';
        sys_print("\n");

        // Very simple command parsing
        if (input[0] == 'h') {
            sys_print("Commands: help, clear, write <file> <text>, read <file>, send <text>, exit\n");
        } else if (input[0] == 'c') {
            // Clear screen (simplified)
            for(int i=0; i<100; i++) sys_print("\n");
        } else if (input[0] == 'w') {
            // write <file> <text>
            sys_print("Writing to file...\n");
            int fd = sys_open("user.txt");
            sys_write(fd, "Shell data", 10);
            sys_close(fd);
            sys_print("Done.\n");
        } else if (input[0] == 'r') {
            // read <file>
            sys_print("Reading file...\n");
            int fd = sys_open("user.txt");
            char buf[64];
            sys_read(fd, buf, 64);
            sys_print("Content: ");
            sys_print(buf);
            sys_print("\n");
            sys_close(fd);
        } else if (input[0] == 's') {
            // send <text>
            sys_print("Sending network packet...\n");
            sys_net_send("lo", (const uint8_t*)"Shell Hello", 11);
            sys_print("Sent.\n");
        } else if (input[0] == 'e') {
            sys_print("Shutting down Nova Kernel. Goodbye!\n");
            sys_exit();
        } else {
            sys_print("Unknown command.\n");
        }
    }
}
