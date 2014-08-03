#include <stdlib.h>

#include "jam.h"
#include "vmep.h"
#include "reflect.h"

#include "lock.h"
#include "hash.h"
#include "class.h"
#include "symbol.h"
#include "excep.h"
#include "thread.h"
#include "classlib.h"
#include "properties.h"
#include "frame.h"

#include "interp-direct-common.h"

#include "list.h"

MethodBlock *dofilter_mb=NULL;
MethodBlock *getarraycount_mb=NULL;
MethodBlock *onexecute_mb=NULL;

FieldBlock *rfilters_fb=NULL;
FieldBlock *status_fb=NULL;
FieldBlock *type_fb=NULL;


//TODO:Needs to be added to thread dependent
LinkedList *monitors=NULL;
LinkedList *instmonitors=NULL;
LinkedList *memmonitors=NULL;

VmepEnv vmep={0,NULL,NULL,NULL};

static int element_sizes[] = {1, 2, 4, 8, 1, 2, 4, 8};


/** Add vmep monitor to monitor the system */
void vmepAddMonitor(Object* instance){

	VmepMonitor *monitor=malloc(sizeof(VmepMonitor));
	monitor->instance=instance;
	monitor->old_mb=NULL;
	monitor->memarry=-1;


	int monitorType=INST_DATA(instance,int,type_fb->u.offset);
	switch(monitorType){
		case 0: linkedListAdd(monitors,monitor); break;
		case 1: linkedListAdd(instmonitors,monitor); break;
		case 2: linkedListAdd(memmonitors,monitor); break;
	}
	

	int i=0;	
	LinkedListItem* item=NULL;
	for(item=instmonitors->first; item!=NULL; item=item->next){
		fflush(stdout);
	}
}

/** Find monitor by instance **/
VmepMonitor* vmepFindMonitor(LinkedList *monitors,Object* monitor){
	LinkedListItem* item=NULL;					
	for(item=monitors->first; item!=NULL; item=item->next)
		if(((VmepMonitor*)item->value)->instance==monitor) return item->value;
	return NULL;
}

/** Remove vmep monitor from monitoring the system */
void vmepRemoveMonitor(Object* instance){
	int monitorType=INST_DATA(instance,int,type_fb->u.offset);


	LinkedList *monitorlist=NULL;
	switch(monitorType){
		case 0: monitorlist=monitors; break;
		case 1: monitorlist=instmonitors; break;
		case 2: monitorlist=memmonitors; break;
	}

	VmepMonitor *monitor=vmepFindMonitor(monitorlist,instance);
	linkedListRemove(monitorlist,monitor);
	

	free(monitor);
} 
// Monitor statuses: 0-Stopped 1-Started 2-Running 3-Idle 4-Idle_R
// Return: 0 - false, 1 - cached run, 2 - first run
int vmepCheckMonitorFilter(VmepMonitor *monitor,int idlerec,ExecEnv *ee,int opcode,Frame* frame,MethodBlock *mb){

	// Get current status of Monitor
	int status=INST_DATA(monitor->instance,int,status_fb->u.offset);

	// If status is recursive idle
	if(status==VMEP_MSTAT_IDLER){
		// If program has returned to lock point of recursive idle then unlock 
		if(monitor->ret_frame==ee->last_frame &&
		   opcode>=172 && opcode<=177){ // If opcode is any of return type
			INST_DATA(monitor->instance,int,status_fb->u.offset)=VMEP_MSTAT_RUNNING;
			monitor->ret_frame=NULL;
		}
		// If status is idle then do not run 
		return 0;
		
	}

	// Get new method block
	//MethodBlock *mb=ee->last_frame->mb;

	// Check if method block not cahnged just finish function
	if(monitor->old_mb==mb)  return status==VMEP_MSTAT_RUNNING;

	// Get instance of array containing filters for the monitor
	Object* filters=INST_DATA(monitor->instance,Object*,rfilters_fb->u.offset);

	// Prepare retcode for filters
	int retcode=0;

	// Check if array has any filters
	if(ARRAY_LEN(filters)>0){
		// Get native data from array object of monitor
		Object** filters_array=ARRAY_DATA(filters,Object*);
		
		// Loop trough each filter and run it
		int i;
		for(i=0;i<ARRAY_LEN(filters);i++){

			// Filter instance
			Object* o=filters_array[i];

			// Execute doFilter method on filter instance, and pass Method object
			retcode=*((Object**)executeMethod(o,findMethod(o->class,newUtf8("doFilter"),newUtf8("(Ljava/lang/reflect/Method;)I")),classlibCreateMethodObject(mb)));
		}
	}


	// Handle retcode
	switch(retcode){
		case 0:
			INST_DATA(monitor->instance,int,status_fb->u.offset)=VMEP_MSTAT_RUNNING;
			monitor->old_mb=mb;
			return 2;
		case 1: 
			INST_DATA(monitor->instance,int,status_fb->u.offset)=VMEP_MSTAT_IDLE;
			monitor->old_mb=mb;
			return 0;
		case 2:
			if(!idlerec || (opcode>=172 && opcode<=177)){
				INST_DATA(monitor->instance,int,status_fb->u.offset)=VMEP_MSTAT_IDLE;
				monitor->old_mb=mb;
				return 0;
			}
			monitor->ret_frame=frame;
			INST_DATA(monitor->instance,int,status_fb->u.offset)=VMEP_MSTAT_IDLER;
			return 0;
	}
	printf("EEERRROOOOR!!!!!");
	fflush(stdout);
	return 0;
}

void vmepExePre(ExecEnv *ee,Instruction *inst,Frame *frame,MethodBlock *mb){
	if(!(vmep.mode & VMEP_MODE_MONITOR)){	
		vmep.mode|=VMEP_MODE_MONITOR;	

		vmep.old_opcode=inst->opcode;							
		int opcode=inst->opcode;					
		VmepMonitor* monitor;								
		LinkedListItem* item=NULL;					

		// Instruction monitors call
		for(item=instmonitors->first; item!=NULL; item=item->next){
			monitor=item->value;						

			// Run filtercheck
			switch(vmepCheckMonitorFilter(monitor,1,ee,0,frame,mb)){
				case 2:
					monitor->temparray=ARRAY_DATA(*((Object**)executeMethod(monitor->instance,findMethod(monitor->instance->class,newUtf8("getArrayCount"), newUtf8("(Ljava/lang/reflect/Method;)[I")),classlibCreateMethodObject(mb))),int);
				case 1: 
					monitor->temparray[opcode]++;
					break;
			}
			//vmepCountAdd(obj,ee,inst,opcode);					
		}									



		/*************** Mem instruction **********************/
		if((opcode>=187 && opcode<=189)){

			for(item=memmonitors->first; item!=NULL; item=item->next){
				monitor=item->value;						

				// Run filtercheck
				switch(vmepCheckMonitorFilter(monitor,0,ee,inst->opcode,frame,mb)){
					case 2:
						monitor->temparray=ARRAY_DATA(*((Object**)executeMethod(monitor->instance,findMethod(monitor->instance->class,newUtf8("getMemCount"), newUtf8("(Ljava/lang/reflect/Method;)[I")),classlibCreateMethodObject(mb))),int);
					case 1:
						monitor->memarry=opcode-187;
						break;
					case 0:
						monitor->memarry=-1;
						break;
				}
				//vmepCountAdd(obj,ee,inst,opcode);					
			}
		}


		vmep.mode^=VMEP_MODE_MONITOR;	
	}

}

//static char *array_names[] = {"[Z", "[C", "[F", "[D", "[B", "[S", "[I", "[J"};
//static int element_sizes[] = {1, 2, 4, 8, 1, 2, 4, 8};
int prim_sizes[]={1,1,2,2,4,4,8,8};
void vmepExeMem(ExecEnv *ee,Instruction *inst,Frame *frame, MethodBlock *mb,uintptr_t *ostack){
	if(!(vmep.mode & VMEP_MODE_MONITOR) && vmep.old_opcode>=187 && vmep.old_opcode<=189){	
		int add=0;
		int count=1;
		Object* obj=ostack[-1];
		ClassBlock *cb=CLASS_CB(obj->class);
		switch(vmep.old_opcode){
			case 189:
				count=ARRAY_LEN(obj);
				cb=CLASS_CB(cb->element_class);
			case 187:
				add=cb->object_size;
				break;
			case 188:
				count=ARRAY_LEN(obj);
				cb=CLASS_CB(cb->element_class);
				add=prim_sizes[cb->state-CLASS_PRIM-1];
				break;
					
		}
		add*=count;
		
		// Calculate memuse table index
		int tableindex=vmep.old_opcode-187;
		VmepMonitor* monitor;								
		LinkedListItem *item=NULL;					

		for(item=memmonitors->first; item!=NULL; item=item->next){
			monitor=item->value;						
			if(monitor->memarry>=0){
				monitor->temparray[tableindex]+=add;
			}
		}
	}
	

}
void vmepInitialiseMonitor(Class *class,Class *class1){
	getarraycount_mb = findMethod(class,newUtf8("getArrayCount"), newUtf8("(Ljava/lang/reflect/Method;)[I"));   
	dofilter_mb = findMethod(class1,newUtf8("doFilter"), newUtf8("(Ljava/lang/reflect/Method;)I"));   
	onexecute_mb = findMethod(class1,newUtf8("onExecute"), newUtf8("(I;)V"));   
	rfilters_fb=findField(class,newUtf8("rFilters"),newUtf8("[Ljamvm/vmep/RuntimeFilter;"));
	status_fb=findField(class,newUtf8("status"),newUtf8("I"));
	type_fb=findField(class,newUtf8("type"),newUtf8("I"));

}
// Initialization
int initialiseVmep(InitArgs *args){
	instmonitors=newLinkedList();
	memmonitors=newLinkedList();
	monitors=newLinkedList();
	return TRUE;
}
