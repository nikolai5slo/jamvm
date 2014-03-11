package jamvm.vmep;

import java.util.Hashtable;
import java.lang.reflect.Method;
import java.util.Map.Entry;

public class InstructionMonitor extends Monitor{
	
	public static final int OPCODE_COUNT=201;
	private static final int CACHE_NO=0;
	private static final int CACHE_ALL=1;
	private static final int CACHE_PATCKAGE=2;
	private int status=0;

	int[] cache=new int[OPCODE_COUNT];
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

	public int[] getCountsFor(Package p){
		if(!isRunning() && status==CACHE_PATCKAGE) return cache;
		
		for(int i=0;i<cache.length;i++) cache[i]=0;

		for(Entry<Method,int[]> entry:opcount.entrySet()){
			if(entry.getKey().getDeclaringClass().getPackage()==p){
				for(int i=0;i<cache.length;i++)
					cache[i]+=entry.getValue()[i];
			}
		}

		if(!isRunning()) status=CACHE_PATCKAGE; 
		else status=CACHE_NO;

		return cache;
	}

	public int getCountFor(Method m, int opcode){
		return getCountsFor(m)[opcode];
	}

	public int getCountFor(Package p, int opcode){
		return getCountsFor(p)[opcode];
	}

	public int[] getCounts(){
		if(!isRunning() && status==CACHE_ALL) return cache;
		
		for(int i=0;i<cache.length;i++) cache[i]=0;

		for(int[] value:opcount.values()){
			for(int i=0;i<cache.length;i++)
				cache[i]+=value[i];
		}

		if(!isRunning()) status=CACHE_ALL; 
		else status=CACHE_NO;

		return cache;
	}

	@Override
	protected void onExecute(){}

}
