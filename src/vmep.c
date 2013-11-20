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


MonitorItem* first_monitor=NULL;
MonitorItem* last_monitor=NULL;

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

/** Corrected method for finding field **/
FieldBlock *vmepFindField(Class *class, char *fieldname, char *type) {
    return findField(class,newUtf8(fieldname),newUtf8(type));
}

/** Corrected method for finding method **/
MethodBlock *vmepFindMethod(Class *class, char *methodname, char *type) {
   return findMethod(class,newUtf8(methodname),newUtf8(type));
}
int vmepMODE=0;

MethodBlock *oldmb=NULL;
Object *array=NULL;
void vmepCountAdd(Object *monitor,MethodBlock *mb,int opcode){
	MethodBlock *mmb = findMethod(monitor->class,newUtf8("getArrayCount"), newUtf8("(Ljava/lang/reflect/Method;)[I"));   

	if(oldmb!=mb){
		vmepMODE|=VMEP_MODE_MONITOR;	
		array=*((Object**)executeMethod(monitor,mmb,classlibCreateMethodObject(mb)));
		oldmb=mb;
		vmepMODE^=VMEP_MODE_MONITOR;	
	}
	ARRAY_DATA(array, int)[opcode]++;
}

void vmepExeListener(int opcode){

}
void vmepInvokeListener(){

}
