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

	/**
	 * Returns array with index as opcode of instruction and value on specifed index as freqency of instruction executed, for specifed method. 
	 * @param	method	Filter result by method
	 * @return		Array of execution frequencies indexed by instructions opcode
	 */
	public int[] getCountsFor(Method method){
		return opcount.get(method);
	}

	/**
	 * Returns array with index as opcode of instruction and value on specifed index as freqency of instruction executed, for specifed package. 
	 * @param	p	Filter result by package 
	 * @return		Array of execution frequencies indexed by instructions opcode
	 */
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


	/**
	 * Returns execution frequency of instruction specified by opcode. 
	 * @param	m	Filter result by method 
	 * @param	opcode	Opcode of instruction 
	 * @return		Execution frequency
	 */
	public int getCountFor(Method m, int opcode){
		return getCountsFor(m)[opcode];
	}

	/**
	 * Returns execution frequency of instruction specified by opcode. 
	 * @param	p	Filter result by package 
	 * @param	opcode	Opcode of instruction 
	 * @return		Execution frequency
	 */
	public int getCountFor(Package p, int opcode){
		return getCountsFor(p)[opcode];
	}

	/**
	 * Returns execution frequency of all monitored scopes. 
	 * @return		Array of execution frequencies indexed by instructions opcode
	 */
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
