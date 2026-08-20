#include "kernel.h"
#include "vfs.h"
#include "net/net_stack.h"
#include "drivers/keyboard.h"
#include "scheduler.h"
#include "pmm.h"
#include "string.h"
#include "io.h"

static void read_line(char* buf, int max) {
    int pos = 0;
    while (1) {
        char c = keyboard_get_char();
        if (!c) {
            __asm__ volatile("hlt");
            continue;
        }
        if (c == '\n' || c == '\r') {
            kputc('\n');
            break;
        }
        if (c == '\b') {
            if (pos > 0) {
                pos--;
                kputc('\b');
            }
            continue;
        }
        if (pos < max - 1) {
            buf[pos++] = c;
            kputc(c);
        }
    }
    buf[pos] = '\0';
}

static char* skip_spaces(char* s) {
    while (*s == ' ' || *s == '\t') s++;
    return s;
}

static int split_cmd(char* line, char** argv, int max) {
    int n = 0;
    char* p = skip_spaces(line);
    while (*p && n < max) {
        argv[n++] = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p++ = '\0';
            p = skip_spaces(p);
        }
    }
    return n;
}

static void cmd_help(void) {
    kprintln("Owokernel shell commands:");
    kprintln("  help                 Show this help");
    kprintln("  clear                Clear the screen");
    kprintln("  ls                   List RAMFS files");
    kprintln("  write <file> <text>  Write text to a file");
    kprintln("  read <file>          Print a file");
    kprintln("  send <text>          Send a loopback packet");
    kprintln("  recv                 Receive a loopback packet");
    kprintln("  mem                  Show PMM usage");
    kprintln("  ps                   List threads");
    kprintln("  echo <text>          Print text");
    kprintln("  exit                 Halt the CPU");
}

static void cmd_write(int argc, char** argv) {
    if (argc < 3) {
        kprintln("usage: write <file> <text>");
        return;
    }
    int fd = vfs_open(argv[1]);
    if (fd < 0) {
        kprintln("open failed");
        return;
    }
    for (int i = 2; i < argc; i++) {
        vfs_write(fd, argv[i], kstrlen(argv[i]));
        if (i + 1 < argc) vfs_write(fd, " ", 1);
    }
    vfs_close(fd);
    kprintln("wrote %s", argv[1]);
}

static void cmd_read(int argc, char** argv) {
    if (argc < 2) {
        kprintln("usage: read <file>");
        return;
    }
    int fd = vfs_open(argv[1]);
    if (fd < 0) {
        kprintln("open failed");
        return;
    }
    char buf[256];
    size_t n = vfs_read(fd, buf, sizeof(buf) - 1);
    buf[n] = '\0';
    kprintln("%s", buf);
    vfs_close(fd);
}

void shell_main(void) {
    char line[128];
    char* argv[16];

    kprintln("Welcome to the Owo Shell. Type 'help'.");
    while (1) {
        kprint("Owo# ");
        read_line(line, (int)sizeof(line));
        int argc = split_cmd(line, argv, 16);
        if (argc == 0) continue;

        if (kstrcmp(argv[0], "help") == 0) {
            cmd_help();
        } else if (kstrcmp(argv[0], "clear") == 0) {
            terminal_clear();
        } else if (kstrcmp(argv[0], "ls") == 0) {
            vfs_list();
        } else if (kstrcmp(argv[0], "write") == 0) {
            cmd_write(argc, argv);
        } else if (kstrcmp(argv[0], "read") == 0) {
            cmd_read(argc, argv);
        } else if (kstrcmp(argv[0], "send") == 0) {
            const char* msg = (argc > 1) ? argv[1] : "hello";
            if (net_send_packet("lo", (const uint8_t*)msg, kstrlen(msg)) == 0) {
                kprintln("sent %d bytes on lo", (int)kstrlen(msg));
            } else {
                kprintln("send failed");
            }
        } else if (kstrcmp(argv[0], "recv") == 0) {
            uint8_t buf[128];
            int n = net_recv_packet("lo", buf, sizeof(buf) - 1);
            if (n < 0) {
                kprintln("no packet");
            } else {
                buf[n] = 0;
                kprintln("recv (%d): %s", n, (char*)buf);
            }
        } else if (kstrcmp(argv[0], "mem") == 0) {
            kprintln("PMM used %d / %d pages", (int)pmm_used_pages(), (int)pmm_total_pages());
        } else if (kstrcmp(argv[0], "ps") == 0) {
            scheduler_dump();
        } else if (kstrcmp(argv[0], "echo") == 0) {
            for (int i = 1; i < argc; i++) {
                kprint("%s", argv[i]);
                if (i + 1 < argc) kputc(' ');
            }
            kputc('\n');
        } else if (kstrcmp(argv[0], "exit") == 0) {
            kprintln("Shutting down Owokernel. Goodbye!");
            __asm__ volatile("cli; hlt");
            while (1) __asm__ volatile("hlt");
        } else {
            kprintln("Unknown command. Type 'help'.");
        }
    }
}
