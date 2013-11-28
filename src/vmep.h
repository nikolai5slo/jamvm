extern void vmepAddMonitor(Object*);
extern void vmepRemoveMonitor(Object*);
extern FieldBlock *vmepFindField(Class *class, char *fieldname, char *type);
extern MethodBlock *vmepFindMethod(Class *class, char *methodname, char *type);
extern void vmepCountAdd(Object *monitor,MethodBlock *mb,int opcode);

struct monitor_item{
	Object* value;	
	struct monitor_item* next;
	struct monitor_item* prev;
} typedef MonitorItem;

extern MonitorItem* first_monitor;
extern int vmep_mode;

#define VMEP_MODE_MONITOR 1
#define VMEP_MODE_LISTENER 2

#define VEMP_EXECD()									\
{											\
	if(!(vmep_mode && VMEP_MODE_MONITOR)){						\
		Instruction* inst=(Instruction*)pc;					\
		int opcode=inst->opcode;						\
		Object* obj;								\
		MonitorItem* curr=NULL;							\
		for(curr=first_monitor; curr!=NULL; curr=curr->next){			\
			obj=curr->value;						\
											\
			ExecEnv *ee = getExecEnv();					\
			vmepCountAdd(obj,ee->last_frame->mb,opcode);			\
		}									\
	}										\
}					
