#include <boot/LimineRequests.hpp>
#include <common/log.hpp>
#include <boot/limine.h>
#include <core/pic.hpp>
#include <core/cpu.hpp>
#include <core/idt.hpp>
#include <core/mm.hpp>
#include <dev/fb.hpp>
#include <common/string.hpp>
struct limine_terminal *terminal;
void print(const char* str) {
	TerminalRequest.response->write(terminal, str, strlen(str));
}
extern "C" void main(void) {
      if (TerminalRequest.response == NULL || TerminalRequest.response->terminal_count < 1) {
        asm("hlt");
      }

      terminal = TerminalRequest.response->terminals[0];

      log("[Debug Console]");
      
	  log("Starting PIC..."); 
	  InitPIC();

      log("Starting CPU features..");
      InitCPUfeat();
      mm::pmm::init();
      idt::init();
      
	  log("Starting framebuffer...");
      InitFB();

      print("Hello, world!");
      while (1) {
         asm ("hlt");
      }
}
