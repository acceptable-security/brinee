#include <system.h>
#include <threads.h>

process_t* currentProcess = 0;
uint32_t lpid;
int tasks_enabled;

void task_idle() {
	puts("task_idle\n");
	if ( !tasks_enabled ) {
		tasks_init();

		tasks_enabled = 1;

		load_userspace();
	}
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
	currentProcess->esp = r;
	currentProcess = currentProcess->next;
	__asm__ volatile("mov %0, %%esp;"
			 "pop %%gs;"
			 "pop %%fs;"
			 "pop %%es;"
			 "pop %%ds;"
			 "mov $0x20, %%al;"
			 "mov $0x20, %%dx;"
			 "outb %%al, %%dx;"
			 "popa;"
			 "add $0x8, %%esp;"
			 "iret"
			 : :"r"(currentProcess->esp));
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
	proc->state = PROCESS_STATE_ALIVE;
	// notify?

	proc->esp = (uint32_t) malloc(4096) + 4096;
	uint32_t stacktop = proc->esp;

	proc->esp -= sizeof(struct regs);

	struct regs *r = proc->esp;

	r->eflags = 0x00000202;
	r->cs = 0x8;
	r->eip = (uint32_t)addr;
	r->eax = 0;
	r->ebx = 0;
	r->ecx = 0;
	r->edx = 0;
	r->esi = 0;
	r->edi = 0;
	r->ebp = stacktop;
	r->useresp = 0xDEADBEEF;
	r->ds = 0x10;
	r->fs = 0x10;
	r->es = 0x10;
	r->gs = 0x10;
	proc->started = false;


	__asm__ volatile("mov %%cr3, %%eax":"=a"(proc->cr3));

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
	// currentProcess->started = true;
	__asm__ volatile("mov %0, %%esp;"
			 "pop %%gs;"
			 "pop %%fs;"
			 "pop %%es;"
			 "pop %%ds;"
			 "mov $0x20, %%al;"
			 "mov $0x20, %%dx;"
			 "outb %%al, %%dx;"
			 "popa;"
			 "add $0x8, %%esp;"
			 "iret"
			 : :"r"(currentProcess->esp));
}

void task_test() {
	puts("boop\n");
	while(1);
}

void task_filler() {
	while(1);
}

void threads_install() {
	lpid = 0;
	tasks_enabled = 0;

	currentProcess = thread_new("idle", (uint32_t)task_idle);
	currentProcess->next = currentProcess;
	currentProcess->prev = currentProcess;

	thread_add_newenv(thread_new("test", (uint32_t)task_test));

	thread_add_newenv(thread_new("task_cleaner", (uint32_t)task_cleaner)); // task cleaner
	// thread_add_newenv(thread_new("task_example", (uint32_t)task_example)); // example

	irq_install_handler(8, schedule);
	threads_start();
}
