#include <system.h>
#include <threads.h>

process_t* currentProcess = 0;
uint32_t lpid;
int tasks_enabled;

void task_idle() {
	puts("task_idle\n");
	// if ( !tasks_enabled ) {
	// 	tasks_init();
	//
	// 	tasks_enabled = 1;
	//
	// 	load_userspace();
	// }
	load_userspace();
}

void task_showinit() {
	puts("Tasking initialized...\n");
	threads_killcurrent();
}

void task_example() {
	int cnt = 0;

	while(1) {
		if ( cnt % 30 )
			puts("Weird\n");

		cnt ++;
	}
}

void schedule(struct regs *r) {
	currentProcess->r = *r;
	currentProcess = currentProcess->next;
	*r = currentProcess->r;
}

void schedule_noirq() {
	if(!tasks_enabled)
		return;

	__asm__ volatile("int $0x2e");
}

void thread_kill(uint32_t pid) {
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
}

void threads_killcurrent() {
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
}

void task_cleaner() {
	puts("it's killin time :^)");
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
	proc->esp = (uint32_t) malloc(4096) + 4096;

	proc->r.eflags = 0x00000202;
	proc->r.cs = 0x8;
	proc->r.eip = (uint32_t)addr;
	proc->r.eax = 0;
	proc->r.ebx = 0;
	proc->r.ecx = 0;
	proc->r.edx = 0;
	proc->r.esi = 0;
	proc->r.edi = 0;
	proc->r.ebp = proc->esp;
	proc->r.useresp = proc->esp;
	proc->r.ds = 0x10;
	proc->r.fs = 0x10;
	proc->r.es = 0x10;
	proc->r.gs = 0x10;
	proc->started = false;


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
	currentProcess->started = true;
	((void (*)())(currentProcess->eip))();
}

void task_test() {
	tasks_init();
	tasks_enabled = 1;
	puts("boop\n");
	while(1);
}

void task_filler() {
	while(1);
}

void threads_install() {
	lpid = 0;
	tasks_enabled = 0;

	currentProcess = thread_new("test", (uint32_t)task_test);
	currentProcess->next = currentProcess;
	currentProcess->prev = currentProcess;

	thread_add_newenv(thread_new("idle", (uint32_t)task_idle));

	thread_add_newenv(thread_new("task_cleaner", (uint32_t)task_cleaner)); // task cleaner
	// thread_add_newenv(thread_new("task_example", (uint32_t)task_example)); // example

	irq_install_handler(8, schedule);
	threads_start();
}
