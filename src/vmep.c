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

MethodBlock *dofilter_mb=NULL;
MethodBlock *getarraycount_mb=NULL;
FieldBlock *rfilters_fb=NULL;
FieldBlock *status_fb=NULL;

//TODO:Needs to be added to thread dependent
MonitorItem* first_monitor=NULL;
MonitorItem* last_monitor=NULL;
int vmep_mode=0;
MethodBlock *oldmb=NULL;
Object *array=NULL;
Frame* ret_frame=NULL;

/** Add vmep monitor to monitor the system */
void vmepAddMonitor(Object* monitor){
	if(first_monitor==NULL){
		first_monitor=malloc(sizeof(MonitorItem));
		first_monitor->prev=NULL;
		last_monitor=first_monitor;
	}else{
		last_monitor->next=malloc(sizeof(MonitorItem));
		last_monitor->next->prev=last_monitor;
		last_monitor=last_monitor->next;
	}
	last_monitor->value=monitor;
	last_monitor->next=NULL;
}

/** Remove vmep monitor from monitoring the system */
void vmepRemoveMonitor(Object* monitor){
	MonitorItem* curr=first_monitor;
	while(curr!=NULL){
		if(curr->value==monitor){
			// If first to remove
			if(curr->prev==NULL){
				first_monitor=curr->next;
				if(first_monitor!=NULL) first_monitor->prev=NULL;
				free(curr);
				return;
			}

			// Remove
			curr->prev->next=curr->next;
			free(curr);
			return;
		}
		curr=curr->next;
	}
}

void vmepCountAdd(Object *monitor,ExecEnv *ee,Instruction *inst,int opcode){

	int status=INST_DATA(monitor,int,status_fb->u.offset);

	MethodBlock *mb=ee->last_frame->mb;
	if((oldmb!=mb || status==2) && status!=4){
		vmep_mode|=VMEP_MODE_MONITOR;	

		INST_DATA(monitor,int,status_fb->u.offset)=3;

		//Get array of filters from monitor object 
		Object* filters=INST_DATA(monitor,Object*,rfilters_fb->u.offset);

		//Get length of filters array
		int filters_len=ARRAY_LEN(filters);
		int retcode=0;
			

		if(filters_len>0){
			//Get native array from filters array object
			Object** filters_array=ARRAY_DATA(filters,Object*);

			int i;
			//Object *method=classlibCreateMethodObject(mb);
			for(i=0;i<filters_len;i++){
				Object* o=filters_array[i];
				retcode=*((Object**)executeMethod(o,findMethod(o->class,newUtf8("doFilter"),newUtf8("(Ljava/lang/reflect/Method;)I")),classlibCreateMethodObject(mb)));
			}
		}

		//If not blocked
		if(retcode==0){
			// call of getArrayCount table
			array=*((Object**)executeMethod(monitor,getarraycount_mb,classlibCreateMethodObject(mb)));
			oldmb=mb;
			//Add count
			ARRAY_DATA(array, int)[opcode]++;
		//If recursive block
		}else if(retcode==2){
			ret_frame=ee->last_frame;
			INST_DATA(monitor,int,status_fb->u.offset)=4;
			//printf("BLOCKED\n");fflush(stdout);
		}

		vmep_mode^=VMEP_MODE_MONITOR;	
	//Unblock if is blocked,about to return to correct frame and return pocode
	}else if(status==4 && ret_frame==ee->last_frame && inst->opcode>=172 && inst->opcode<=177){ //BLOCKED do release
		INST_DATA(monitor,int,status_fb->u.offset)=3;
		ret_frame=NULL;
		//printf("UNBLOCKED\n");fflush(stdout);
	}else if(status!=4) ARRAY_DATA(array, int)[opcode]++;
}

void vmepExecd(ExecEnv *ee,Instruction *inst){
	if(!(vmep_mode & VMEP_MODE_MONITOR)){								
		int opcode=inst->opcode;					
		Object* obj;								
		MonitorItem* curr=NULL;					
		for(curr=first_monitor; curr!=NULL; curr=curr->next){
			obj=curr->value;						
											
			vmepCountAdd(obj,ee,inst,opcode);					
		}									
	} 
}

void vmepInitialize(Class *class,Class *class1){
	getarraycount_mb = findMethod(class,newUtf8("getArrayCount"), newUtf8("(Ljava/lang/reflect/Method;)[I"));   
	dofilter_mb = findMethod(class1,newUtf8("doFilter"), newUtf8("(Ljava/lang/reflect/Method;)I"));   
	rfilters_fb=findField(class,newUtf8("rFilters"),newUtf8("[Ljamvm/vmep/RuntimeFilter;"));
	status_fb=findField(class,newUtf8("status"),newUtf8("I"));
}

void vmepExeListener(Instruction inst,Frame frame){

}
void vmepInvokeListener(){

}
