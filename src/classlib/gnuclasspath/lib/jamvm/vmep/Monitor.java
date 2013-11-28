package jamvm.vmep;

import java.util.Arrays;
import java.util.Hashtable;
import java.lang.reflect.Method;
import java.util.Map.Entry;

public class Monitor{
	public static final int RUNNING=2;
	public static final int STOPPED=1;
	public static final int STOPPED_CACHED=0;
	public static final int OPCODE_COUNT=201;
	private int status=STOPPED; 
	private native void mstart();
	private native void mstop();

	public void start(){
		status=RUNNING;
		mstart();
	}
	public void stop(){
		mstop();
		status=STOPPED;
	}

	private Hashtable<Method,int[]>opcount=new Hashtable<Method,int[]>();

	private int[] getArrayCount(Method m){
		int[] opcodes=opcount.get(m);
		if(opcodes==null){
			opcodes=new int[OPCODE_COUNT];
			opcount.put(m,opcodes);
		}
		return opcodes;	
	}
	public int[] getCountsFor(Method m){
		return opcount.get(m);
	}
	int[] cache=new int[OPCODE_COUNT];
	public int[] getCountsFor(Package p){
		if(status==STOPPED_CACHED) return cache;
		
		for(int i=0;i<cache.length;i++) cache[i]=0;

		for(Entry<Method,int[]> entry:opcount.entrySet()){
			if(entry.getKey().getDeclaringClass().getPackage()==p){
				for(int i=0;i<cache.length;i++)
					cache[i]+=entry.getValue()[i];
			}
		}
		if(status==STOPPED) status=STOPPED_CACHED;

		return cache;

	}
	public int getCountFor(Method m, int opcode){
		return getCountsFor(m)[opcode];
	}
	public int getCountFor(Package p, int opcode){
		return getCountsFor(p)[opcode];
	}
}
