package jamvm.vmep;

import java.util.Arrays;
import java.util.Hashtable;
import java.lang.reflect.Method;
import java.util.Map.Entry;
import java.util.LinkedList;
import java.util.List;

public class Monitor{
	static{
		initMonitors(RuntimeFilter.class);
	}
	private static native void initMonitors(Class c);

	public static final int STARTED=2;
	public static final int RUNNING=3;
	public static final int STOPPED=1;
	public static final int STOPPED_CACHED=0;
	public static final int OPCODE_COUNT=201;
	private int status=STOPPED; 
	private native void mstart();
	private native void mstop();

	public void start(){
		status=STARTED;
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



	private RuntimeFilter[] rFilters=new RuntimeFilter[0];

	public void addRuntimeFilter(RuntimeFilter filter){
		rFilters=Arrays.copyOf(rFilters,rFilters.length+1);	
		rFilters[rFilters.length-1]=filter;
	}

	public void removeRuntimeFilter(RuntimeFilter filter){
		LinkedList<RuntimeFilter> list = new LinkedList<RuntimeFilter>();
		for(RuntimeFilter f:rFilters) if(f!=filter) list.add(f);
		rFilters = list.toArray(new RuntimeFilter[0]);
	}
}
