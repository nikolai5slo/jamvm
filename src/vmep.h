extern void vmepAddMonitor(Object*);
extern void vmepRemoveMonitor(Object*);
extern void vmepExecd(ExecEnv *ee,Instruction *inst);
extern void vmepInitialize(Class *class,Class *class1);

struct monitor_item{
	Object* value;	
	struct monitor_item* next;
	struct monitor_item* prev;
} typedef MonitorItem;

extern MonitorItem* first_monitor;
extern int vmep_mode;
extern Frame* ret_frame;

#define VMEP_MODE_MONITOR 1
#define VMEP_MODE_LISTENER 2

#define VEMP_EXECD()									\
{											\
	ExecEnv *ee = getExecEnv();							\
	Instruction *inst=(Instruction*)pc;						\
	vmepExecd(ee,inst);								\
											\
}					
