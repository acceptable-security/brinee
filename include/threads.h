#include <system.h>
#include <stdlib.h>

#define PROCESS_STATE_ALIVE 0
#define PROCESS_STATE_ZOMBIE 1
#define PROCESS_STATE_DEAD 2

struct process;

struct process {
	struct process* prev;
	char* name;

	uint32_t pid;
	uint32_t esp;
	uint32_t stacktop;
	uint32_t eip;
	uint32_t cr3;
	uint32_t state;

	void (*notify)(int);
	struct process* next;

	struct regs r;
	bool started;
};

typedef struct process process_t;

process_t* thread_new(char* name, uint32_t addr);
void threads_install();
int thread_add(process_t* proc);
