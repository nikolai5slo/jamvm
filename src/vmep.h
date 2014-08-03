extern void vmepAddMonitor(Object*);
extern void vmepRemoveMonitor(Object*);
extern void vmepAddCoreListener(Object* listener);
extern void vmepRemoveCoreListener(Object* listener);

extern void vmepExePre(ExecEnv *ee,Instruction *inst,Frame *frame, MethodBlock *mb);
extern void vmepExeMem(ExecEnv *ee,Instruction *inst,Frame *frame, MethodBlock *mb,uintptr_t *ostack);

extern void vmepInitialiseMonitor(Class *class,Class *class1);
extern int initialiseVmep(InitArgs *args);

extern int vmep_mode;
extern Frame* ret_frame;

struct vmep_env{
	int mode;
	int old_opcode;
} typedef VmepEnv;

extern VmepEnv vmep;

struct vmep_monitor{
	int *status;
	MethodBlock *old_mb;
	Object *instance;
	//Object *temparray;
	int *temparray;
	Frame* ret_frame;

	//Mem
	int memarry;
} typedef VmepMonitor;

#define VMEP_MODE_MONITOR 1
#define VMEP_MODE_LISTENER 2

#define VMEP_MSTAT_STOPPED 0
#define VMEP_MSTAT_RUNNING 1
#define VMEP_MSTAT_IDLE 2
#define VMEP_MSTAT_IDLER 3

#define VMEP_EXEPRE()					\
{							\
	Instruction *inst=(Instruction*)pc;		\
	vmepExePre(getExecEnv(),inst,frame,mb);		\
}

#define VMEP_EXEMEM()					\
{							\
	Instruction *inst=(Instruction*)pc;		\
	vmepExeMem(getExecEnv(),inst,frame,mb,ostack);	\
}					
