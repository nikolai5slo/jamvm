package jamvm.vmep;

import java.util.Hashtable;
import java.lang.reflect.Method;
import java.util.Map.Entry;

public class MemoryMonitor extends Monitor{
	
	public static final int COUNT=3;

	int[] cache=new int[COUNT];
	private Hashtable<Method,int[]>memcount=new Hashtable<Method,int[]>();

	private int[] getMemCount(Method m){
		int[] memuse=memcount.get(m);
		if(memuse==null){
			memuse=new int[COUNT];
			memcount.put(m,memuse);
		}
		return memuse;	
	}

	/**
	 * Returns sum of sizes for all memory allocations in method.
	 * @param	m	Method to get results for
	 * @return		Size in bytes
	 */
	public int getMemUseFor(Method m){
		int[] memuse=memcount.get(m);
		if(memuse==null) return -1;

		return memuse[0]+memuse[1]+memuse[2];
	}

	/**
	 * Returns sum of sizes for all memory allocations in methods which are in specifed package.
	 * @param	p	Package to get results for
	 * @return		Size in bytes
	 */
	public int getMemUseFor(Package p){
		int sum=-1;
		for(Entry<Method,int[]> entry:memcount.entrySet()){
			if(entry.getKey().getDeclaringClass().getPackage()==p){
				if(sum<0) sum=this.getMemUseFor(entry.getKey());				
				else sum+=this.getMemUseFor(entry.getKey());
			}
		}
		return sum;
	}

	/**
	 * Returns sum of sizes for all memory allocations in all monitored methods.
	 * @return		Size in bytes
	 */
	public int getMemUse(){
		int sum=-1;	

		for(int[] value:memcount.values()){
			if(sum<0) sum=value[0]+value[1]+value[2];
			else sum+=value[0]+value[1]+value[2];
		}

		return sum;
	}

	@Override
	protected void onExecute(){}

}
