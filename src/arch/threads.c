#include <system.h>
#include <threads.h>

process_t* currentProcess = 0;
uint32_t lpid;
int tasks_enabled;

void task_idle() {
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");

	puts("task_idle\n");
	if ( !tasks_enabled ) {
		tasks_init();
		tasks_enabled = 1;
		load_userspace();
	}
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
}

void task_showinit() {
	__asm__ volatile("nop");
	// puts("Tasking initialized...\n");
	// threads_killcurrent();
}

void task_example() {
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	// while(1) {
	// 	puts("Weird\n");
	// 	timer_wait(18);
	// }
}

void schedule() {
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	puts("scheduling ");
	puts(currentProcess->name);
	puts("!\n");
	__asm__ volatile("push %eax");
	__asm__ volatile("push %ebx");
	__asm__ volatile("push %ecx");
	__asm__ volatile("push %edx");
	__asm__ volatile("push %esi");
	__asm__ volatile("push %edi");
	__asm__ volatile("push %ebp");
	__asm__ volatile("push %ds");
	__asm__ volatile("push %es");
	__asm__ volatile("push %fs");
	__asm__ volatile("nop");
	__asm__ volatile("push %gs");
	__asm__ volatile("mov %%esp, %%eax":"=a"(currentProcess->esp));
	currentProcess = currentProcess->next;
	__asm__ volatile("mov %%eax, %%cr3": :"a"(currentProcess->cr3));
	__asm__ volatile("mov %%eax, %%esp": :"a"(currentProcess->esp));
	__asm__ volatile("pop %gs");
	__asm__ volatile("pop %fs");
	__asm__ volatile("pop %es");
	__asm__ volatile("pop %ds");
	__asm__ volatile("pop %ebp");
	__asm__ volatile("pop %edi");
	__asm__ volatile("pop %esi");
	__asm__ volatile("out %%al, %%dx": :"d"(0x20), "a"(0x20)); // send EoI to master PIC
	__asm__ volatile("pop %edx");
	__asm__ volatile("pop %ecx");
	__asm__ volatile("pop %ebx");
	__asm__ volatile("pop %eax");
	__asm__ volatile("iret");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
}

void schedule_noirq() {
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	if(!tasks_enabled)
		return;

	__asm__ volatile("int $0x2e");
	return;
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
}

void thread_kill(uint32_t pid) {
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");

	puts("threads_kill\n");
	if(pid == 1)
		return; // can't kill the idle!
	
	if(pid == currentProcess->pid)
		threads_killcurrent();

	process_t* orig = currentProcess;
	process_t* proc = orig;
	
	while(1) {
		if(proc->pid == pid) {
			proc->state = PROCESS_STATE_ZOMBIE;
			break;
		}
		
		proc = proc->next;
		
		if(proc == orig)
			break;
	}

	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
}

void threads_killcurrent() {
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");

	if(currentProcess->pid == 1) {
		tasks_enable(0);
		return;
	}
	
	tasks_enable(0);
	
	free((void *)currentProcess->stacktop);
	free(currentProcess);
	
	pfree(currentProcess->cr3);
	
	currentProcess->prev->next = currentProcess->next;
	currentProcess->next->prev = currentProcess->prev;
	
	tasks_enable(1);
	schedule_noirq();
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
}

void task_cleaner() {
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");

	reset:;
	process_t *orig = currentProcess;
	process_t *proc = orig;
	while(1) {
		proc = proc->next;

		if(proc == currentProcess) 
			continue;
		
		if(proc->state == PROCESS_STATE_ZOMBIE) {
			tasks_enable(0);

			proc->prev->next = proc->next;
			proc->next->prev = proc->prev;

			free((void *)proc->stacktop);
			free(proc);

			tasks_enable(1);
		}
		
		if(proc == orig)
			goto reset;
		
		schedule_noirq();
	}

	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
	__asm__ volatile("nop");
}

process_t* thread_new(char* name, uint32_t addr) {
	int i;
	process_t* proc = (process_t*) malloc(sizeof(process_t));

	memset(proc, 0, sizeof(process_t));

	proc->name = name;
	proc->pid = ++lpid;
	proc->eip = addr;
	proc->state = PROCESS_STATE_ALIVE;
	// notify?
	proc->esp = (uint32_t) malloc(4096);

	__asm__ volatile("mov %%cr3, %%eax":"=a"(proc->cr3));

	uint32_t* stack = (uint32_t *) (proc->esp + 4096);
	proc->stacktop = proc->esp;
	*--stack = 0x00000202;       // eflags
	*--stack = 0x8;              // cs
	*--stack = (uint32_t)addr;   // eip
	*--stack = 0;                // eax
	*--stack = 0;                // ebx
	*--stack = 0;                // ecx
	*--stack = 0;                // edx
	*--stack = 0;                // esi
	*--stack = 0;                // edi
	*--stack = proc->esp + 4096; // ebp
	*--stack = 0x10;             //  ds
	*--stack = 0x10;             //  fs
	*--stack = 0x10;             //  es
	*--stack = 0x10;             //  gs
	proc->esp = (uint32_t) stack;

	return proc;
}

// add a thread when it's a lonedog
void thread_add_newenv(process_t* proc) {
	proc->next = currentProcess->next;
	proc->next->prev = proc;
	proc->prev = currentProcess;
	currentProcess->next = proc;
}

int thread_add(process_t* proc) {
	tasks_enable(0);
	thread_add_newenv(proc);
	tasks_enable(1);

	return proc->pid;
}

void threads_start() {
	__asm__ volatile("mov %%eax, %%esp": :"a"(currentProcess->esp));
	__asm__ volatile("pop %gs");
	__asm__ volatile("pop %fs");
	__asm__ volatile("pop %es");
	__asm__ volatile("pop %ds");
	__asm__ volatile("pop %ebp");
	__asm__ volatile("pop %edi");
	__asm__ volatile("pop %esi");
	__asm__ volatile("pop %edx");
	__asm__ volatile("pop %ecx");
	__asm__ volatile("pop %ebx");
	__asm__ volatile("pop %eax");
	__asm__ volatile("iret");
}

void threads_install() {
	lpid = 0;
	tasks_enabled = 0;

	currentProcess = thread_new("idle", (uint32_t)task_idle);
	currentProcess->next = currentProcess;
	currentProcess->prev = currentProcess;

	// thread_add_newenv(thread_new("task_showinit", (uint32_t)task_showinit)); // idle task
	// thread_add_newenv(thread_new("task_cleaner", (uint32_t)task_cleaner)); // task cleaner
	// thread_add_newenv(thread_new("task_example", (uint32_t)task_example)); // example

	threads_start();
}